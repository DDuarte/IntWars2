#pragma once
#include <vector>
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
struct Face {
    std::string type;
    std::vector<int> indices;
    std::vector<Vector2f> uv;
};

class ScoReader {
    public:
        ScoReader(unsigned char *data, unsigned int length);
        ~ScoReader();
        HRESULT CreateMesh(LPDIRECT3DDEVICE9 pD3DDevice, LPD3DXMESH &pMesh);
        std::string name;
        Vector3f centralPoint;
        std::vector <Vector3f> vertices;
        std::vector<Face> faces;
};

