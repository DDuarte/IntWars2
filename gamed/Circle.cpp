#include "stdafx.h"
#include "Circle.h"
#include "PS_Circle.h"
#include "VS_WorldViewProj.h"
#include "Renderer.h"

struct QUAD_VERTEX {
    D3DXVECTOR3 pos;
    D3DXVECTOR2 tex;
};

static LPDIRECT3DPIXELSHADER9 pixelShader = NULL;
static LPDIRECT3DVERTEXSHADER9 vertexShader = NULL;
static LPDIRECT3DVERTEXDECLARATION9 vertexDecl3D = NULL;
static LPDIRECT3DVERTEXDECLARATION9 vertexDecl2D = NULL;

D3DVERTEXELEMENT9 decl3D[] = { {
        0,
        0,
        D3DDECLTYPE_FLOAT3,
		D3DDECLMETHOD_DEFAULT,
        D3DDECLUSAGE_POSITION,
        0
    },
    {
        0,
        3 * sizeof(float),
        D3DDECLTYPE_FLOAT2,
        D3DDECLMETHOD_DEFAULT,
        D3DDECLUSAGE_TEXCOORD,
        0
    },
    D3DDECL_END()
};

D3DVERTEXELEMENT9 decl2D[] = { {
	0,
	0,
	D3DDECLTYPE_FLOAT3,
	D3DDECLMETHOD_DEFAULT,
	D3DDECLUSAGE_POSITIONT,
	0
},
{
	0,
	3 * sizeof(float),
	D3DDECLTYPE_FLOAT2,
	D3DDECLMETHOD_DEFAULT,
	D3DDECLUSAGE_TEXCOORD,
	0
},
D3DDECL_END()
};

static int refCountPixelShader = 0;

inline int _fastcall CeilPow2(int x) {
    if(x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

CCircle::CCircle(float x, float y, float z, int minRadius, int maxRadius, D3DXCOLOR innerColor, D3DXCOLOR outerColor) {
	this->is3D = true;
	this->LoadRessources();
	this->x = x, this->y = y, this->z = z;
	this->SetRadius(minRadius, maxRadius);
	this->SetColor(innerColor, outerColor);
	this->SetSmoothness(1.0f);
}

CCircle::CCircle(int x, int y, int minRadius, int maxRadius, D3DXCOLOR innerColor, D3DXCOLOR outerColor) {
	this->is3D = false;
    this->LoadRessources();
	this->x = (float)x, this->y = (float)y;
    this->SetRadius(minRadius, maxRadius);
    this->SetColor(innerColor, outerColor);
	this->SetSmoothness(1.0f);
}


CCircle::CCircle(int x, int y, int radius, D3DXCOLOR color) {
	this->is3D = false;
    this->LoadRessources();
    this->x = (float)x, this->y = (float)y;
    this->SetRadius(radius);
    this->SetColor(color);
	this->SetSmoothness(1.0f);
}

CCircle::~CCircle() {
    this->UnloadRessources();
}
void CCircle::SetSmoothness(float smoothness) {
	this->radius[3] = smoothness >= 1.0f ? smoothness : 1.0f;
}

void CCircle::SetRadius(int iRadius) {
    this->SetRadius(iRadius - 1, iRadius);
}
void CCircle::SetRadius(int iMinRadius, int iMaxRadius) {
    if(iMinRadius > iMaxRadius)
        return;
    LPDIRECT3DDEVICE9 pDevice = MainRenderer.GetDevice();
    SAFE_RELEASE(d3d9Texture);
    radius[0] = (float)iMinRadius - .5f;
    radius[1] = (float)iMaxRadius + .5f;
    radius[2] = (float)CeilPow2(iMaxRadius * 2);
    LoadRessources();
}
void CCircle::SetPosition(int x, int y) {
    this->x = (float)x;
	this->y = (float)y;
	this->is3D = false;
    this->SetRadius((int)radius[0] + 1, (int)radius[1]);
}

void CCircle::SetPosition(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->is3D = true;
	this->SetRadius((int)(radius[0] + 1), (int)radius[1]);
}

void CCircle::SetColor(D3DXCOLOR Color) {
    this->SetColor(Color, Color);
}

void CCircle::SetColor(D3DXCOLOR InnerColor, D3DXCOLOR OuterColor) {
    this->innerColor = InnerColor;
    this->outerColor = OuterColor;
}

void CCircle::Draw() {
    if(this->isLoaded) {
		LPDIRECT3DDEVICE9 pDevice = MainRenderer.GetDevice();
        /*Shader*/
		pDevice->SetPixelShaderConstantF(0, radius, 1);
		pDevice->SetPixelShaderConstantF(1, innerColor, 1);
		pDevice->SetPixelShaderConstantF(2, outerColor, 1);
        pDevice->SetPixelShader(pixelShader);
		pDevice->SetVertexDeclaration(is3D ? vertexDecl3D : vertexDecl2D);
		pDevice->SetVertexShader(vertexShader); //Assuming WorldViewProjection is in c0
		/*RenderStates*/ //Not all might be needed
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        /*TextureStates*/
        pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        /*Buffer*/
        pDevice->SetStreamSource(0, vertexBuffer, 0, sizeof(QUAD_VERTEX));
        /*Drawing*/
        pDevice->SetTexture(0, d3d9Texture); //not sure if i even need a texture
        pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		/*Reset, this should be moved in another class, resets are often unecessary*/
		pDevice->SetPixelShader(NULL);
		pDevice->SetVertexShader(NULL);
		pDevice->SetTexture(0, NULL);
    }
}

void CCircle::LoadRessources() {
    LPDIRECT3DDEVICE9 pDevice = MainRenderer.GetDevice();
    if(pDevice) {
        if(!this->isLoaded)
            refCountPixelShader++;
        this->isLoaded = true;
        if(!pixelShader) {
            if(FAILED(pDevice->CreatePixelShader((DWORD *)PS_CIRCLE, &pixelShader)))
                pixelShader = NULL;
        }
		if (!vertexShader) {
			if (FAILED(pDevice->CreateVertexShader((DWORD *)VS_WORLDVIEWPROJ, &vertexShader)))
				vertexShader = NULL;
		}
		if (!vertexDecl2D) {
			if (FAILED(pDevice->CreateVertexDeclaration(decl2D, &vertexDecl2D)))
				vertexDecl2D = NULL;
		}
		if (!vertexDecl3D) {
			if (FAILED(pDevice->CreateVertexDeclaration(decl3D, &vertexDecl3D)))
				vertexDecl3D = NULL;
		}
        UINT textureSize = (UINT)radius[2];
        if(!d3d9Texture) {
            SAFE_RELEASE(vertexBuffer);
            if(FAILED(D3DXCreateTexture(pDevice, textureSize, textureSize, 1, 0, MainRenderer.GetRenderTargetDescription().Format, D3DPOOL_MANAGED, &d3d9Texture)))
                d3d9Texture = NULL;
        }
        if(!vertexBuffer) {
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(QUAD_VERTEX)* 4, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vertexBuffer, NULL)))
				vertexBuffer = NULL;
        }
		if (vertexBuffer) {
			QUAD_VERTEX *v;
			float fTextureRadius = textureSize / 2.f;
			vertexBuffer->Lock(0, 0, (void **)&v, NULL);
			if (!is3D) {
				float fx = x - .5f;
				float fy = y - .5f;
				v[0].pos = D3DXVECTOR3(fx - fTextureRadius, fy - fTextureRadius, .0f);
				v[1].pos = D3DXVECTOR3(fx + fTextureRadius, fy - fTextureRadius, .0f);
				v[2].pos = D3DXVECTOR3(fx - fTextureRadius, fy + fTextureRadius, .0f);
				v[3].pos = D3DXVECTOR3(fx + fTextureRadius, fy + fTextureRadius, .0f);
			} else {
				v[0].pos = D3DXVECTOR3(x - fTextureRadius, y, z + fTextureRadius);
				v[1].pos = D3DXVECTOR3(x + fTextureRadius, y, z + fTextureRadius);
				v[2].pos = D3DXVECTOR3(x - fTextureRadius, y, z - fTextureRadius);
				v[3].pos = D3DXVECTOR3(x + fTextureRadius, y, z - fTextureRadius);
			}
			v[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			v[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			v[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			v[3].tex = D3DXVECTOR2(1.0f, 1.0f);
			vertexBuffer->Unlock();
		}
    }
}

void CCircle::UnloadRessources() {
    if(this->isLoaded)
        refCountPixelShader--;
    this->isLoaded = false;
	if (refCountPixelShader <= 0) {
        SAFE_RELEASE(pixelShader);
		SAFE_RELEASE(vertexShader);
		SAFE_RELEASE(vertexDecl2D);
		SAFE_RELEASE(vertexDecl3D);
	}
    SAFE_RELEASE(d3d9Texture);
    SAFE_RELEASE(vertexBuffer);
}