#include "stdafx.h"
#include "AiPathReader.h"
#include <vector>
#include <iostream>
#include <intsafe.h>

template<typename T> inline T read(unsigned char *buf, int &pos) {
    T val = *reinterpret_cast<T *>(buf + pos);
    pos += sizeof(T);
    return val;
}

AiPathReader::AiPathReader(unsigned char *data, unsigned int lenght) {
    if(lenght <= 8 || !std::string("r3d2aims").compare(reinterpret_cast<char *>(data))) {
        return;
    }
    int pos = 8;
    if(read<unsigned int>(data, pos) != 2) {
        return;
    }
    int len = read<unsigned int>(data, pos);
    bool bUnk = read<unsigned int>(data, pos) != 0;
    unsigned int unk1 = read<unsigned int>(data, pos);
    if(pos + 42 * len != lenght) {
        return;
    }
    nodes.reserve(len);
    for(int i = 0; i < len; i++) {
        NavNode node;
        for(int j = 0; j < 3; j++) {
            auto &vert = node.triangle[j];
            vert.x = read<float>(data, pos);
            vert.y = read<float>(data, pos);
            vert.z = read<float>(data, pos);
            //std::cout << vert.x << "\t|" << vert.y << "\t|" << vert.z << std::endl;
        }
        for(int j = 0; j < 3; j++) {
            node.adjacency[j] = read<unsigned short>(data, pos);
            //std::cout << std::hex << node.children[j] << "\t|";
        }
        //std::cout << std::endl;
        nodes.push_back(node);
    }
}

AiPathReader::~AiPathReader() {
}

HRESULT AiPathReader::CreateMesh(LPDIRECT3DDEVICE9 pD3DDevice, LPD3DXMESH &pMesh) {
    //Todo: ID3DXMesh::SetAttributeTable  (set materials how they are defined in the face struct)
    DWORD dwFVF = (D3DFVF_XYZ | D3DFVF_NORMAL);
    struct D3DVERTEX {
        D3DXVECTOR3 p;
        D3DXVECTOR3 n;
    };
    HRESULT r = D3DXCreateMeshFVF(3 * nodes.size(), 3 * nodes.size(), D3DXMESH_MANAGED, dwFVF, pD3DDevice, &pMesh);
    if(FAILED(r)) {
        return r;
    }
    D3DVERTEX *vertexBuffer;
    WORD *indexBuffer = nullptr;
    unsigned long *pAdjacency = new unsigned long[nodes.size() * 3];
    pMesh->LockIndexBuffer(0, reinterpret_cast<void **>(&indexBuffer));
    pMesh->LockVertexBuffer(0, reinterpret_cast<void **>(&vertexBuffer));
    for(int i = 0; i < nodes.size(); i++) {
        auto face = nodes[i];
        for(int j = 0; j < 3; j++) {
            D3DVERTEX &vert = vertexBuffer[3 * i + j];
            vert.p.x = face.triangle[j].x;
            vert.p.y = face.triangle[j].y;
            vert.p.z = face.triangle[j].z;
            indexBuffer[3 * i + j] = 3 * i + j;
            pAdjacency[3 * i + j] = (face.adjacency[j] == 0xffff) ? 0xffffffffUL : face.adjacency[j];
        }
    }
    //D3DXWeldVertices(pMesh, D3DXWELDEPSILONS_WELDALL, nullptr, pAdjacency, nullptr, nullptr, nullptr);
    //pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_IGNOREVERTS | D3DXMESHOPT_STRIPREORDER, newAdjacency, pAdjacency, nullptr, nullptr);
    delete[] pAdjacency;
    HRESULT hr = D3DXComputeNormals(pMesh, nullptr);
    D3DXMATERIAL *m_pMaterials = nullptr;
    DWORD         m_dwNumMaterials = 0;
    hr = D3DXSaveMeshToX("NavMesh.x", pMesh, nullptr, m_pMaterials, nullptr, m_dwNumMaterials, D3DXF_FILEFORMAT_BINARY);
    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();
    return S_OK;
}