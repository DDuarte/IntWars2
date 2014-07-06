#pragma once
#include "ScoReader.h"
#include "Pathfinder.h"
enum GameMapType {
    SUMMONERS_RIFT,
};
class GameMap {
    public:
        GameMap();
        virtual ~GameMap();
        virtual Vector2i GetSize() = 0;

        Pathfinder &Navigation();
        LPD3DXMESH GetMesh(LPDIRECT3DDEVICE9 device);
        ScoReader &GetSco();

        Vector3f Get3DPosition(Vector2f position);

        static std::unique_ptr<GameMap> Create(GameMapType type);
    protected:
        char *meshFileName;
        char *navMeshFileName;
    private:
        ScoReader *sco;
        Pathfinder *navigation;
        LPD3DXMESH mesh;
};

