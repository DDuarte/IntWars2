#include "stdafx.h"
#include "NetworkListener.h"
#include "MapView.h"
#include "RiotArchiveFileHandler.h"
#include "Server.h"
#include "ScoReader.h"
#include "ObjectManager.h"
#include "AiPathReader.h"
#include "Pathfinder.h"
#include "Renderer.h"
#include "Circle.h"
#define ENABLE_VIEW


//this code is dirty, alot of global state that need to be fixed

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;


static bool isWindowOpen = false;
static bool renderWindow = true;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static HWND showWindow() {
    const char *wndClass = "wndclass";
    HWND hWnd;
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
    wc.lpszClassName = wndClass;
    RegisterClassEx(&wc);
    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    unsigned int dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
    hWnd = CreateWindowEx(NULL, wndClass, "Map View", dwStyle, 300, 300, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow(hWnd, SW_SHOWNORMAL);
    return hWnd;
}
struct BoundingInformation {
    D3DXVECTOR3 center;
    float radius;

    D3DXVECTOR3 minBBox;
    D3DXVECTOR3 maxBBox;
};

const D3DXCOLOR COLOR_WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR COLOR_BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR COLOR_RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR COLOR_LIGHT_RED(D3DCOLOR_XRGB(255, 163, 70));
const D3DXCOLOR COLOR_GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR COLOR_BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR COLOR_LIGHT_BLUE(D3DCOLOR_XRGB(70, 163, 255));

D3DMATERIAL9 initMaterial(D3DXCOLOR ambient, D3DXCOLOR diffuse, D3DXCOLOR specular, D3DXCOLOR emissive, float power) {
    D3DMATERIAL9 material;
    material.Ambient = ambient;
    material.Diffuse = diffuse;
    material.Specular = specular;
    material.Emissive = emissive;
    material.Power = power;
    return material;
}

const D3DMATERIAL9 MATERIAL_WHITE = initMaterial(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK, 2.0f);
const D3DMATERIAL9 MATERIAL_RED = initMaterial(COLOR_RED, COLOR_RED, RED, COLOR_BLACK, 2.0f);
const D3DMATERIAL9 MATERIAL_LIGHT_RED = initMaterial(COLOR_LIGHT_RED, COLOR_LIGHT_RED, RED, COLOR_BLACK, 2.0f);
const D3DMATERIAL9 MATERIAL_GREEN = initMaterial(COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_BLACK, 2.0f);
const D3DMATERIAL9 MATERIAL_BLUE = initMaterial(COLOR_BLUE, COLOR_BLUE, BLUE, COLOR_BLACK, 2.0f);
const D3DMATERIAL9 MATERIAL_LIGHT_BLUE = initMaterial(COLOR_LIGHT_BLUE, COLOR_LIGHT_BLUE, BLUE, COLOR_BLACK, 2.0f);
LPD3DXMESH triangleMesh = nullptr;

LPD3DXMESH g_pMesh = nullptr;
std::unique_ptr<BoundingInformation> BBox;
D3DLIGHT9 light;
D3DMATERIAL9 material;
D3DXVECTOR3 eyePos;
D3DXVECTOR3 lookAt;
D3DXMATRIX matView;
D3DXMATRIX matProj;
Vector3f circlePos;

struct Ray {
    D3DXVECTOR3 origin;
    D3DXVECTOR3 direction;
};

struct Frustrum {
    float nearPlane;
    float farPlane;
};

D3DXVECTOR2 Project(D3DXVECTOR3 pos) {
    D3DXMATRIX world, view, project;
    D3DVIEWPORT9 vp;
    d3ddev->GetViewport(&vp);
    d3ddev->GetTransform(D3DTS_WORLD, &world);
    d3ddev->GetTransform(D3DTS_VIEW, &view);
    d3ddev->GetTransform(D3DTS_PROJECTION, &project);
    D3DXVECTOR3 output;
    D3DXVec3Project(&output, &pos, &vp, &project, &view, &world);
    return D3DXVECTOR2(output.x, output.y);
}

Ray UnProject(D3DXVECTOR2 pos) {
    D3DXMATRIX world, view, project;
    D3DVIEWPORT9 vp;
    d3ddev->GetViewport(&vp);
    d3ddev->GetTransform(D3DTS_WORLD, &world);
    d3ddev->GetTransform(D3DTS_VIEW, &view);
    d3ddev->GetTransform(D3DTS_PROJECTION, &project);
    D3DXVECTOR3 pos1(pos.x, pos.y, vp.MinZ);
    D3DXVECTOR3 pos2(pos.x, pos.y, vp.MaxZ);
    D3DXVec3Unproject(&pos1, &pos1, &vp, &project, &view, &world);
    D3DXVec3Unproject(&pos2, &pos2, &vp, &project, &view, &world);
    Ray ray;
    ray.origin = pos1;
    ray.direction = pos2 - pos1;
    D3DXVec3Normalize(&ray.direction, &ray.direction);
    return ray;
}

HRESULT draw_triangle(D3DXVECTOR3 pos, float direction = 0, D3DMATERIAL9 material = MATERIAL_BLUE) {
    D3DXMATRIX translation, rotation, world;
    D3DXMatrixRotationY(&rotation, D3DXToRadian((direction - 180)));
    D3DXMatrixTranslation(&translation, pos.x, pos.y, pos.z);
    world = rotation * translation;
    d3ddev->SetTransform(D3DTS_WORLD, &world);
    d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    d3ddev->SetFVF(triangleMesh->GetFVF());
    d3ddev->LightEnable(0, true);
    d3ddev->SetMaterial(&material);
    triangleMesh->DrawSubset(0);
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    d3ddev->SetTransform(D3DTS_WORLD, &matWorld);
    return S_OK;
}

std::unique_ptr<BoundingInformation> GetBoundingInformation(LPD3DXMESH mesh) {
    if(!mesh) { return nullptr; }
    std::unique_ptr<BoundingInformation> bb(new BoundingInformation());
    D3DXVECTOR3 center;
    LPD3DXMESH pD3DXMesh = mesh;
    DWORD numVertices = pD3DXMesh->GetNumVertices();
    DWORD fvfSize = D3DXGetFVFVertexSize(pD3DXMesh->GetFVF());
    void *pData = nullptr;
    if(FAILED(pD3DXMesh->LockVertexBuffer(0, &pData))) {
        Logging->writeLine("Failed to lock mesh vertex buffer.", __FILE__, __LINE__);
        return nullptr;
    }
    D3DXComputeBoundingSphere(reinterpret_cast<D3DXVECTOR3 *>(pData), numVertices, fvfSize, &bb->center, &bb->radius);
    D3DXComputeBoundingBox(reinterpret_cast<D3DXVECTOR3 *>(pData), numVertices, fvfSize, &bb->minBBox, &bb->maxBBox);
    if(FAILED(pD3DXMesh->UnlockVertexBuffer())) {
        Logging->writeLine("Failed to unlock mesh vertex buffer.", __FILE__, __LINE__);
        return nullptr;
    }
    return bb;
}
float zoom = 1;

CONST D3DCOLOR Green = D3DCOLOR_ARGB(255, 0, 255, 0);
void DrawPoint(IDirect3DDevice9 *Device, int baseX, int baseY, int baseW, int baseH, D3DCOLOR color) {
    D3DRECT BarRect = { baseX - baseW / 2, baseY - baseH / 2, baseX + (baseW + 1) / 2, baseY + (baseH + 1) / 2 };
    Device->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 0, 0);
}

bool moving = false;
bool rotating = false;
D3DXVECTOR2 startMousePos;
D3DXVECTOR3 currentCameraTranslation;
D3DXVECTOR3 cameraTranslation;
int cameraSlopeX = 10;
int cameraSlopeY = 0;
std::list<Vector3f> navPath;
CHero *selection;

Vector3f GetHitPos(LPARAM lParam) {
    Vector3f result;
    if(d3ddev == nullptr) {
        return result;
    }
    D3DXVECTOR2 mouse(LOWORD(lParam), HIWORD(lParam));
    Ray hitRay = UnProject(mouse);
    //Ray hitRay = UnprojectPoint(LOWORD(lParam), HIWORD(lParam));
    BOOL pHit;
    DWORD faceIndex, countOfHits;
    FLOAT pU, pV, pDist;
    LPD3DXBUFFER buf;
    HRESULT r = D3DXIntersect(g_pMesh, &hitRay.origin, &hitRay.direction, &pHit, &faceIndex, &pU, &pV, &pDist, nullptr, nullptr);
    if(pHit) {
        auto pos = hitRay.origin + hitRay.direction * pDist;
        result = Vector3f(pos.x, pos.y, pos.z);
    }
    return result;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
        case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
            break;
        case WM_MOUSEMOVE: {
                if(d3ddev == nullptr) {
                    break;
                }
                circlePos = GetHitPos(lParam);
                D3DXVECTOR2 mouse(LOWORD(lParam), HIWORD(lParam));
                if(moving) {
                    currentCameraTranslation = D3DXVECTOR3(mouse.x - startMousePos.x, 0, mouse.y - startMousePos.y);
                    currentCameraTranslation *= 20;
                    currentCameraTranslation.x = -currentCameraTranslation.x;
                    currentCameraTranslation.y = -currentCameraTranslation.y;
                } else if(rotating) {
                    cameraSlopeX += (mouse.y - startMousePos.y) / 2;
                    cameraSlopeY += (mouse.x - startMousePos.x) / 4;
                    startMousePos = D3DXVECTOR2(LOWORD(lParam), HIWORD(lParam));
                    cameraSlopeX = max(5, min(85, cameraSlopeX));
                    cameraSlopeY = cameraSlopeY % 360;
                }
            }
            break;
        case WM_MOUSEWHEEL: {
                int fwKeys = GET_KEYSTATE_WPARAM(wParam);
                int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                zoom += zDelta * 4;
            }
            break;
        case WM_LBUTTONDOWN: {
                auto hitPos = GetHitPos(lParam);
                if(hitPos.x || hitPos.y || hitPos.z) {
                    auto hitPos2D = Vector2f(hitPos.x, hitPos.z);
                    selection = nullptr;
                    float closestDist = 200;
                    for(auto p : CObjectManager::GetPlayers())	{
                        if(p != NULL) {
                            auto dist = (p->GetPos() - hitPos2D).length();
                            if(dist < closestDist) {
                                closestDist = dist;
                                selection = p;
                            }
                        }
                    }
                }
                if(!selection) {
                    startMousePos = D3DXVECTOR2(LOWORD(lParam), HIWORD(lParam));
                    moving = true;
                }
            }
            break;
        case WM_LBUTTONUP: {
                moving = false;
                cameraTranslation += currentCameraTranslation;
                currentCameraTranslation = D3DXVECTOR3(0, 0, 0);
            }
            break;
        case WM_RBUTTONDOWN: {
                if(selection) {
                    auto hitPos = GetHitPos(lParam);
                    if(hitPos.x || hitPos.y || hitPos.z) {
                        selection->moveTo(Vector2f(hitPos.x, hitPos.z));
                    }
                } else {
                    rotating = true;
                    startMousePos = D3DXVECTOR2(LOWORD(lParam), HIWORD(lParam));
                }
            }
            break;
        case WM_RBUTTONUP: {
                rotating = false;
                if(selection) {
                    //move selection todo
                }
            }
            break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
HRESULT SetViewMatrix(D3DXMATRIX &mat, D3DXVECTOR3 &vFrom, D3DXVECTOR3 &vAt, D3DXVECTOR3 &vWorldUp) {     //Better D3DXMatrixLookAtLH(&matView, &eyePos, &lookAt, &vWorldUp);
    D3DXVECTOR3 vView = vAt - vFrom;
    FLOAT fLength = D3DXVec3Length(&vView);
    if(fLength < 1e-6f) {
        return E_INVALIDARG;
    }
    vView /= fLength;
    FLOAT fDotProduct = D3DXVec3Dot(&vWorldUp, &vView);
    D3DXVECTOR3 vUp = vWorldUp - fDotProduct * vView;
    if(1e-6f > (fLength = D3DXVec3Length(&vUp))) {
        vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f) - vView.y * vView;
        if(1e-6f > (fLength = D3DXVec3Length(&vUp))) {
            vUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f) - vView.z * vView;
            if(1e-6f > (fLength = D3DXVec3Length(&vUp))) {
                return E_INVALIDARG;
            }
        }
    }
    vUp /= fLength;
    D3DXVECTOR3 vRight;
    D3DXVec3Cross(&vRight, &vUp, &vView);
    D3DXMatrixIdentity(&mat);
    mat._11 = vRight.x;
    mat._12 = vUp.x;
    mat._13 = vView.x;
    mat._21 = vRight.y;
    mat._22 = vUp.y;
    mat._23 = vView.y;
    mat._31 = vRight.z;
    mat._32 = vUp.z;
    mat._33 = vView.z;
    mat._41 = -D3DXVec3Dot(&vFrom, &vRight);
    mat._42 = -D3DXVec3Dot(&vFrom, &vUp);
    mat._43 = -D3DXVec3Dot(&vFrom, &vView);
    return S_OK;
}
void init_frame() {
    d3ddev->Clear(0, nullptr, D3DCLEAR_TARGET, 0x1e1e1e1e, 1.0f, 0);
    d3ddev->Clear(0, nullptr, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    d3ddev->BeginScene();
    d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
    d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    float fov = 80.0f;
    D3DXMatrixIdentity(&matView);
    D3DXVECTOR3 cameraDirection = D3DXVECTOR3(0, 1, 0);
    D3DXMATRIX matX, matY;
    D3DXMatrixRotationX(&matX, -D3DXToRadian(cameraSlopeX));
    D3DXMatrixRotationY(&matY, -D3DXToRadian(cameraSlopeY));
    D3DXVECTOR4 outp;
    D3DXVec3Transform(&outp, &cameraDirection, &matX);
    cameraDirection = D3DXVECTOR3(outp.x, outp.y, outp.z);
    D3DXVec3Transform(&outp, &cameraDirection, &matY);
    cameraDirection = D3DXVECTOR3(outp.x, outp.y, outp.z);
    D3DXVec3Normalize(&cameraDirection, &cameraDirection);
    eyePos = BBox->center;
    //cameraDirection = -cameraDirection;
    float dist = BBox->radius / sin(fov / 2.0);
    zoom = fmin(dist - 20, zoom);
    cameraDirection *= dist - zoom;
    eyePos += cameraDirection;
    lookAt = BBox->center;
    static D3DXVECTOR3 coordSystem = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMATRIX translation;
    D3DXMatrixIdentity(&translation);
    D3DXVECTOR3 t = cameraTranslation + currentCameraTranslation;
    D3DXMatrixTranslation(&translation, t.x, t.y, t.z);
    D3DXVec3Transform(&outp, &eyePos, &translation);
    eyePos = D3DXVECTOR3(outp.x, outp.y, outp.z);
    D3DXVec3Transform(&outp, &lookAt, &translation);
    lookAt = D3DXVECTOR3(outp.x, outp.y, outp.z);
    SetViewMatrix(matView, eyePos, lookAt, coordSystem);
    //D3DXMatrixLookAtLH(&matView, &eyePos, &lookAt, &coordSystem);
    d3ddev->SetTransform(D3DTS_VIEW, &matView);
    //
    //Projection
    auto lengthToCenter = D3DXVec3Length(&cameraDirection);
    D3DXMatrixIdentity(&matProj);
    float nearPlane = fmax(1, lengthToCenter - BBox->radius);
    float farPlane = lengthToCenter + BBox->radius;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(fov), static_cast<FLOAT>(SCREEN_WIDTH) / static_cast<FLOAT>(SCREEN_HEIGHT), nearPlane, farPlane);
    d3ddev->SetTransform(D3DTS_PROJECTION, &matProj);
    //
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    d3ddev->SetTransform(D3DTS_WORLD, &matWorld);
    //
}
void draw_map() {
    //d3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    d3ddev->SetFVF(g_pMesh->GetFVF());
    d3ddev->SetLight(0, &light);
    d3ddev->LightEnable(0, true);
    //d3ddev->SetTexture(0, m_pTxMesh);
    d3ddev->SetMaterial(&material);
    g_pMesh->DrawSubset(0);
}
void draw_waypoints() {
    //Waypoints
    d3ddev->LightEnable(0, false);
    d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    for(auto p : CObjectManager::GetPlayers())	{
        if(p != NULL) {
            std::vector<D3DXVECTOR3> line;
            for(auto waypoint : p->GetWaypoints()) {
                D3DXVECTOR3 rayPos = D3DXVECTOR3(waypoint.x, BBox->minBBox.y, waypoint.y);
                D3DXVECTOR3 rayPosEnd = D3DXVECTOR3(waypoint.x, BBox->maxBBox.y, waypoint.y);
                D3DXVECTOR3 direction = rayPosEnd - rayPos;
                D3DXVec3Normalize(&direction, &direction);
                BOOL pHit;
                DWORD faceIndex, countOfHits;
                FLOAT pU, pV, pDist;
                LPD3DXBUFFER buf;
                D3DXIntersect(g_pMesh, &rayPos, &direction, &pHit, &faceIndex, &pU, &pV, &pDist, nullptr, nullptr);
                if(pHit) {
                    line.push_back(rayPos + direction * (pDist + 10));
                }
            }
            if(line.size() > 1) {
                LPD3DXLINE d3dline;
                D3DXCreateLine(d3ddev, &d3dline);
                d3dline->SetAntialias(true);
                d3dline->SetWidth(4.0f);
                d3dline->Begin();
                d3dline->DrawTransform(line.data(), line.size(), &(matView * matProj), p->tTeam == BLUE ? COLOR_BLUE : COLOR_RED);
                d3dline->End();
                d3dline->Release();
            }
            if(line.size() >= 1) {
                D3DXVECTOR3 pos3d(line[0].x, line[0].y, line[0].z);
                auto material = p->tTeam == BLUE ? p->GetPeer() ? MATERIAL_BLUE : MATERIAL_LIGHT_BLUE : p->GetPeer() ? MATERIAL_RED : MATERIAL_LIGHT_RED;
                if(selection == p) {
                    material = MATERIAL_GREEN;
                }
                draw_triangle(pos3d, p->GetRotation(), material);
            }
        }
    }
}
void draw_navpath() {
    //D3DXVECTOR2 pos2d = Project(D3DXVECTOR3(pathFindingStart.x, pathFindingStart.y, pathFindingStart.z));
    //DrawPoint(d3ddev, pos2d.x, pos2d.y, 8, 8, Green);
    //pos2d = Project(D3DXVECTOR3(pathFindingEnd.x, pathFindingEnd.y, pathFindingEnd.z));
    //DrawPoint(d3ddev, pos2d.x, pos2d.y, 8, 8, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
    std::vector<D3DXVECTOR3> line;
    for(auto waypoint : navPath) {
        line.push_back(D3DXVECTOR3(waypoint.x, waypoint.y, waypoint.z));
    }
    if(line.size() > 1) {
        LPD3DXLINE d3dline;
        D3DXCreateLine(d3ddev, &d3dline);
        d3dline->SetAntialias(true);
        d3dline->SetWidth(1.0f);
        d3dline->Begin();
        d3dline->DrawTransform(line.data(), line.size(), &(matView * matProj), Green);
        d3dline->End();
        d3dline->Release();
    }
}

D3DXCOLOR COLOR_VISION = 0x30000000u;
float visionRange = 1350;
CCircle *visionCircle;
CCircle *visionCircle1;
CCircle *visionCircle2;
CCircle *visionCircle3;
CCircle *towerCircle;

static void draw_circle(CCircle *circle, Vector2f pos, D3DXCOLOR color) {
    if(!circle) {
        return;
    }
    Vector3f pos3D = (*server).GetMap().Get3DPosition(pos);
    D3DXMATRIX wvp;
    D3DXMatrixTranslation(&wvp, pos3D.x, pos3D.y, pos3D.z);
    wvp *= matView * matProj;
    D3DXMatrixTranspose(&wvp, &wvp);
    d3ddev->SetVertexShaderConstantF(0, wvp, 4);
    circle->SetColor(color);
    circle->Draw();
}

void draw_vision() {
    for(auto p : CObjectManager::GetPlayers())	{
        if(p != NULL) {
            D3DXCOLOR color = p->tTeam == BLUE ? COLOR_BLUE : COLOR_RED;
            color.a = 0.1;
            draw_circle(visionCircle, p->GetPos(), color);
        }
    }
    for(auto p : CObjectManager::GetTowers())	{
        if(p != NULL) {
            D3DXCOLOR color = p->tTeam == BLUE ? COLOR_BLUE : COLOR_RED;
            color.a = 0.1;
            if(p->GetRange() == 800) {
                draw_circle(visionCircle1, p->GetPos(), color);
            } else if(p->GetRange() == 1000) {
                draw_circle(visionCircle2, p->GetPos(), color);
            } else if(p->GetRange() == 1905) {
                draw_circle(visionCircle3, p->GetPos(), color);
            }
        }
    }
}

void draw_towers() {
    for(auto p : CObjectManager::GetTowers())	{
        if(p != NULL) {
            D3DXCOLOR color = 0xFF000000;
            draw_circle(towerCircle, p->GetPos(), color);
        }
    }
}

static void render_frame(void) {
    init_frame();
    draw_map();
    //draw_navmesh();
    draw_waypoints();
    draw_navpath();
    draw_vision();
    draw_towers();
    d3ddev->EndScene();
}
static void initD3D() {
    //Open window
    HWND hWnd = showWindow();
    //Create D3D
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_8_SAMPLES;
    d3dpp.MultiSampleQuality = 0;
    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
    MainRenderer.SetDevice(d3ddev);
    //Read map mesh
    g_pMesh = (*server).GetMap().GetMesh(d3ddev);
    BBox = GetBoundingInformation(g_pMesh);
    D3DXVECTOR3  *vertexBuffer;
    g_pMesh->LockVertexBuffer(0, reinterpret_cast<void **>(&vertexBuffer));
    D3DXComputeBoundingBox(vertexBuffer, g_pMesh->GetNumVertices(), 20, &BBox->minBBox, &BBox->maxBBox);
    g_pMesh->UnlockVertexBuffer();
    //
    {
        DWORD dwFVF = (D3DFVF_XYZ | D3DFVF_NORMAL);
        struct D3DVERTEX {
            D3DXVECTOR3 p;
            D3DXVECTOR3 n;
        };
        HRESULT r = D3DXCreateMeshFVF(1, 3, D3DXMESH_MANAGED, dwFVF, d3ddev, &triangleMesh);
        if(FAILED(r)) {
            return;
        }
        D3DVERTEX *vertexBuffer;
        WORD *indexBuffer = nullptr;
        D3DXVECTOR3 a(-100, 0, 250);
        D3DXVECTOR3 b(100, 0, 250);
        D3DXVECTOR3 c(0, 0, 0);
        D3DXVECTOR3 mid = (a + b + c) / 3;
        triangleMesh->LockIndexBuffer(0, reinterpret_cast<void **>(&indexBuffer));
        triangleMesh->LockVertexBuffer(0, reinterpret_cast<void **>(&vertexBuffer));
        vertexBuffer[0].p = a - mid;
        vertexBuffer[1].p = b - mid;
        vertexBuffer[2].p = c - mid;
        indexBuffer[0] = 0;
        indexBuffer[1] = 1;
        indexBuffer[2] = 2;
        HRESULT hr = D3DXComputeNormals(triangleMesh, nullptr);
        triangleMesh->UnlockVertexBuffer();
        triangleMesh->UnlockIndexBuffer();
    }
    //
    d3ddev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
    //d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    d3ddev->SetRenderState(D3DRS_AMBIENT, RGB(15, 15, 15));
    ZeroMemory(&material, sizeof(D3DMATERIAL9));
    material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    material.Ambient = D3DXCOLOR(0.7f, 0.7f, 1.0f, 1.0f);
    ZeroMemory(&light, sizeof(light));
    light.Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;
    light.Position = eyePos;
    D3DXVECTOR3 direction = (BBox->maxBBox + BBox->minBBox) / 2.0f;
    direction.z -= 1000;
    direction.y = BBox->maxBBox.y - 6500;
    direction = (BBox->maxBBox + BBox->minBBox) / 2.0f - direction;
    D3DXVec3Normalize(&direction, &direction);
    light.Direction = direction;
    light.Range = 100.0f;
    light.Attenuation0 = 0.0f;
    light.Attenuation1 = 0.125f;
    light.Attenuation2 = 0.0f;
    light.Phi = D3DXToRadian(40.0f);
    light.Theta = D3DXToRadian(20.0f);
    light.Falloff = 1.0f;
    visionCircle = new CCircle(0, 0, 0, 0, visionRange, COLOR_VISION, COLOR_VISION);
    visionCircle->SetSmoothness(15);
    visionCircle1 = new CCircle(0, 0, 0, 0, 800, COLOR_VISION, COLOR_VISION);
    visionCircle1->SetSmoothness(15);
    visionCircle2 = new CCircle(0, 0, 0, 0, 1000, COLOR_VISION, COLOR_VISION);
    visionCircle2->SetSmoothness(15);
    visionCircle3 = new CCircle(0, 0, 0, 0, 1905, COLOR_VISION, COLOR_VISION);
    visionCircle3->SetSmoothness(15);
    towerCircle = new CCircle(0, 0, 0, 0, 88.4f, COLOR_VISION, COLOR_VISION);
    towerCircle->SetSmoothness(5);
}
void cleanD3D(void) {
    delete visionCircle;
    delete visionCircle1;
    delete visionCircle2;
    delete visionCircle3;
    delete towerCircle;
    triangleMesh->Release();
    d3ddev->Release();
    int c = d3d->Release();
    if(c > 0) {
        Logging->writeLine("D3D Warning: %d leaked resources.", c);
    }
}
MapView::MapView() {
}
MapView::~MapView() {
}
void MapView::Update() {
    #ifdef ENABLE_VIEW
    if(!isWindowOpen && renderWindow) {
        initD3D();
        isWindowOpen = true;
    }
    if(renderWindow) {
        render_frame();
    }
    if(isWindowOpen) {
        MSG msg;
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if(msg.message == WM_QUIT) {
            renderWindow = false;
            isWindowOpen = false;
            cleanD3D();
            return;
        }
    }
    #endif
}
void MapView::Present() {
    if(renderWindow && isWindowOpen) {
        d3ddev->Present(nullptr, nullptr, nullptr, nullptr);
    }
}