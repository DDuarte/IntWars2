#include "stdafx.h"
#include "GameMap.h"
#include "SummonersRift.h"
#include "Server.h"
#include "RafDB.h"
GameMap::GameMap() {
    navigation = nullptr;
    sco = nullptr;
    mesh = nullptr;
}

GameMap::~GameMap() {
    SAFE_DELETE(sco);
    SAFE_DELETE(navigation);
    SAFE_RELEASE(mesh);
}

std::unique_ptr<GameMap> GameMap::Create(GameMapType type) {
    if(type == SUMMONERS_RIFT) {
        SummonersRift *rift = new SummonersRift();
        return std::unique_ptr<GameMap>(rift);
    }
    return std::unique_ptr<GameMap>();
}

Pathfinder &GameMap::Navigation() {
    if(!navigation) {
        RiotArchiveFileEntry aimeshfile = *(*RafDB::GetHandler())[navMeshFileName];
        auto extracted = aimeshfile.extractData();
        AiPathReader navmesh(reinterpret_cast<unsigned char *>(extracted.data()), static_cast<unsigned int>(extracted.size()));
        navigation = new Pathfinder(navmesh);
    }
    return *navigation;
}

LPD3DXMESH GameMap::GetMesh(LPDIRECT3DDEVICE9 device) {  //todo: move this into the renderer class once we got one
    if(!mesh) {
        if(FAILED(GetSco().CreateMesh(device, mesh))) {
            mesh = nullptr;
        }
    }
    return mesh;
}
ScoReader &GameMap::GetSco() {
    if(!sco) {
        RiotArchiveFileEntry file = *(*RafDB::GetHandler())[meshFileName];
        auto extracted = file.extractData();
        sco = new ScoReader(reinterpret_cast<unsigned char *>(extracted.data()), static_cast<unsigned int>(extracted.size()));
    }
    return *sco;
}

Vector3f GameMap::Get3DPosition(Vector2f position) {
    //todo: move this into the renderer class once we got one
    //Todo: use bbox as default height (requires mesh class)
    Vector3f result = Vector3f(position.x, 0, position.y);
    if(!mesh) {
        #ifdef DEBUG
        //this function requires directx
        DebugBreak();
        #endif
        return result;
    }
    //BBox->minBBox.y
    //BBox->maxBBox.y
    D3DXVECTOR3 rayPos = D3DXVECTOR3(position.x, 100000.0f, position.y);
    D3DXVECTOR3 rayPosEnd = D3DXVECTOR3(position.x, -100000.0f, position.y);
    D3DXVECTOR3 direction = rayPosEnd - rayPos;
    D3DXVec3Normalize(&direction, &direction);
    BOOL pHit;
    DWORD faceIndex, countOfHits;
    FLOAT pU, pV, pDist;
    LPD3DXBUFFER buf;
    D3DXIntersect(mesh, &rayPos, &direction, &pHit, &faceIndex, &pU, &pV, &pDist, nullptr, nullptr);
    if(pHit) {
        auto vec = rayPos + direction * pDist;
        result.y = vec.y;
    }
    return result;
}
