#pragma once
#include "DrawObject.h"

class CCircle : DrawObject {
    public:
		CCircle::CCircle(float x, float y, float z, int minRadius, int maxRadius, D3DXCOLOR innerColor, D3DXCOLOR outerColor);
        CCircle(int x, int y, int minRadius, int maxRadius, D3DXCOLOR innerColor, D3DXCOLOR outerColor);
        CCircle(int x, int y, int radius, D3DXCOLOR color);
        ~CCircle();
		void CCircle::SetSmoothness(float smoothness);
        void CCircle::SetRadius(int iRadius);
        void CCircle::SetRadius(int iMinRadius, int iMaxRadius);
        void CCircle::SetPosition(int x, int y);
		void CCircle::SetPosition(float x, float y, float z);
        void CCircle::SetColor(D3DXCOLOR InnerColor);
        void CCircle::SetColor(D3DXCOLOR InnerColor, D3DXCOLOR OuterColor);
        void CCircle::Draw();
        void CCircle::UnloadRessources();
        void CCircle::LoadRessources();
    private:
        float x;
        float y;
		float z;
		bool is3D = false;
		float radius[4];
        D3DXCOLOR innerColor;
        D3DXCOLOR outerColor;
        LPDIRECT3DTEXTURE9 d3d9Texture = NULL;
        IDirect3DVertexBuffer9 *vertexBuffer = NULL;
};

