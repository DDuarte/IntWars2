#pragma once

struct NavNode {
    Vector3f triangle[3];
    unsigned short adjacency[3];
};

class AiPathReader {
    public:
        AiPathReader(unsigned char *data, unsigned int lenght);
        ~AiPathReader();
        HRESULT CreateMesh(LPDIRECT3DDEVICE9 device, LPD3DXMESH &mesh);
        std::vector<NavNode> nodes;
};

