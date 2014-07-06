#include "stdafx.h"
#include "ScoReader.h"
#include <fstream>
#include <sstream>
enum ScoProperty {
    NONE,
    NAME,
    CENTRAL_POINT,
    VERTS,
    FACES
};

//Todo: optimizing
//Todo: add scb from here: https://github.com/nddrylliog/scb2obj

ScoReader::ScoReader(unsigned char *data, unsigned int length) {
    std::string content(reinterpret_cast<char *>(data), length);
    std::istringstream file(content);
    std::string line;
    ScoProperty currentProperty = NONE;
    while(std::getline(file, line)) {
        if(line[0] == '[') {
            continue;
        }
        int keyPos = line.find('=');
        if(keyPos != -1) {
            std::string key = line.substr(0, keyPos);
            auto value = std::istringstream(line.substr(keyPos + 2, line.length() - keyPos - 3));
            if(key == "Name") {
                currentProperty = NAME;
                value >> name;
            } else if(key == "CentralPoint") {
                currentProperty = CENTRAL_POINT;
                value >> centralPoint.x >> centralPoint.y >> centralPoint.z;
            } else if(key == "Verts") {
                currentProperty = VERTS;
                unsigned int vertCount = 0;
                value >> vertCount;
                vertices.reserve(vertCount);
            } else if(key == "Faces") {
                currentProperty = FACES;
                unsigned int faceCount = 1;
                value >> faceCount;
                faces.reserve(faceCount);
            }
        } else {
            auto value = std::istringstream(line);
            switch(currentProperty) {
                case VERTS: {
                        Vector3f vert;
                        value >> vert.x >> vert.y >> vert.z;
                        vertices.push_back(vert);
                    }
                    break;
                case FACES: {
                        Face cFace;
                        int indiceCount = 0;
                        value >> indiceCount;
                        if(indiceCount <= 0) {
                            break;
                        }
                        cFace.indices.reserve(indiceCount);
                        cFace.uv.reserve(indiceCount);
                        for(int i = 0; i < indiceCount; i++) {
                            int indice = 0;
                            value >> indice;
                            cFace.indices.push_back(indice);
                        }
                        value >> cFace.type;
                        for(int i = 0; i < indiceCount; i++) {
                            Vector2f uv;
                            value >> uv.x >> uv.y;
                            cFace.uv.push_back(uv);
                        }
                        faces.push_back(cFace);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}


ScoReader::~ScoReader() {
}

HRESULT ScoReader::CreateMesh(LPDIRECT3DDEVICE9 pD3DDevice, LPD3DXMESH &pMesh) {
    //Todo: ID3DXMesh::SetAttributeTable  (set materials how they are defined in the face struct)
    DWORD dwFVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
    struct D3DVERTEX {
        D3DXVECTOR3 p;
        D3DXVECTOR3 n;
        FLOAT       tu, tv;
    };
    HRESULT r = D3DXCreateMeshFVF(faces.size(), vertices.size(), D3DXMESH_MANAGED, dwFVF, pD3DDevice, &pMesh);
    if(FAILED(r)) {
        return r;
    }
    D3DVERTEX *vertexBuffer;
    WORD *indexBuffer = nullptr;
    pMesh->LockIndexBuffer(0, reinterpret_cast<void **>(&indexBuffer));
    pMesh->LockVertexBuffer(0, reinterpret_cast<void **>(&vertexBuffer));
    for(int i = 0; i < pMesh->GetNumVertices(); i++) {
        Vector3f &vert = vertices[i];
        vertexBuffer[i].p.x = vert.x;
        vertexBuffer[i].p.y = vert.y;
        vertexBuffer[i].p.z = vert.z;
    }
    for(int faceIndex = 0; faceIndex < pMesh->GetNumFaces(); faceIndex++) {
        auto face = faces[faceIndex];
        #ifdef _DEBUG
        if(face.indices.size() != 3 || face.uv.size() != 3) {
            DebugBreak();
            return D3DERR_CANNOTPROTECTCONTENT;
        }
        #endif
        for(int vertTriangleIndex = 0; vertTriangleIndex < 3; vertTriangleIndex++) {
            auto vertexIndex = face.indices[vertTriangleIndex];
            auto &uv  = face.uv[vertTriangleIndex];
            auto &vertex = vertexBuffer[vertexIndex];
            vertex.tu = uv.x;
            vertex.tv = uv.y;
            indexBuffer[3 * faceIndex + vertTriangleIndex] = vertexIndex;
        }
    }
    HRESULT hr = D3DXComputeNormals(pMesh, nullptr);
    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();
    return S_OK;
}
