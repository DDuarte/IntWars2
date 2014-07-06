#ifndef _OBJECT_MANAGER_H
#define _OBJECT_MANAGER_H

#include "Unit.h"
#include "Hero.h"
#include "Tower.h"

namespace CObjectManager {
    WORD GetNewIndex();
    std::vector<CTower *> GetTowers(Team tTeam = TEAM_COUNT); //0x40000001 - 0x40000018
    std::vector<CHero *> GetPlayers(Team tTeam = TEAM_COUNT); //0x40000019 - 0x4000001A
    CHero *GetPlayer(UINT nPeer = 0);
    DWORD GetPlayerNetID(); //0x40000019
    std::vector<CUnit *> GetProps(); //0x4000001A - 0x4000001D
    CObject *AddObject(CObject *unit);
    CObject *NewObject();
    bool Init();
    CObject *GetUnit(DWORD index);
};

class CLane {
    public:
        std::vector<CTower *> vUnits;
        CLane();
        CUnit *GetLiveTurret();
        CTower *AddTower(CTower *unit);
};

class CTeam {
    public:
        Team team;
        CTeam(Team t);

        CLane lanes[3];
        CLane *GetTopLane();
        CLane *GetMidLane();
        CLane *GetBotLane();
};
extern unsigned int getHash(const char *str);
extern DWORD getCharacterHash(char *szName, UINT nSkin);

#endif