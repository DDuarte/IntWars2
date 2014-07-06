#include "stdafx.h"
#include "NetworkListener.h"
#include "ObjectManager.h"
#include "UpdateStatsPacket.h"
#include "LuaVM.h"
#include "Server.h"


unsigned int getHash(const char *str) {
    if(str == 0) { return 0; }
    unsigned int hash = 0;
    unsigned int mask = 0xF0000000;
    for(unsigned int i = 0; i < strlen(str); i++) {
        hash = tolower(str[i]) + (0x10 * hash);
        if(hash & mask) {
            hash ^= hash & mask ^ ((hash & mask) >> 24);
        }
    }
    return hash;
}
DWORD getCharacterHash(char *szName, UINT nSkin) {
    char szSkin[32];
    sprintf(szSkin, "%02d", nSkin);
    unsigned int hash = 0;
    const char *gobj = "[Character]";
    for(unsigned int i = 0; i < strlen(gobj); i++) {
        hash = tolower(gobj[i]) + (0x1003F * hash);
    }
    for(unsigned int i = 0; i < strlen(szName); i++) {
        hash = tolower(szName[i]) + (0x1003F * hash);
    }
    for(unsigned int i = 0; i < strlen(szSkin); i++) {
        hash = tolower(szSkin[i]) + (0x1003F * hash);
    }
    return hash;
}
namespace CObjectManager {
    std::vector<CObject *> vUnits;
    std::vector<CTower *> vTowers;
    std::vector<CHero *> vPlayers;
    std::vector<CUnit *> vProps;
    WORD GetNewIndex() {
        return vUnits.size();
    }
    CHero *GetPlayer(UINT nPeer) {
        return nPeer < vPlayers.size() ? vPlayers.at(nPeer) : NULL;
    }
    DWORD GetPlayerNetID() {
        return vPlayers.at(0)->GetNetworkID();
    }
    CObject *GetUnit(DWORD index) {
        if(VALID_NWID(index)) {
            index = index & ~NWID_BASE;
        }
        return vUnits.at(index);
    }

    std::vector<CHero *> GetPlayers(Team tTeam) {
        std::vector<CHero *> _vPlayers;
        for(auto t : vPlayers) {
            if(t->tTeam == tTeam || tTeam == TEAM_COUNT) {
                _vPlayers.push_back(t);
            }
        }
        return _vPlayers;
    }
    //0x40000001 - 0x40000018
    std::vector<CTower *> GetTowers(Team tTeam) {
        std::vector<CTower *> _vTowers;
        for(auto t : vTowers) {
            if(t->tTeam == tTeam || tTeam == TEAM_COUNT)
            { _vTowers.push_back(t); }
        }
        return _vTowers;
    }
    std::vector<CUnit *> GetProps() {
        return vProps;
    }
    CObject *AddObject(CObject *unit) {
        vUnits.push_back(unit);
        return unit;
    }
    CObject *NewObject() {
        return AddObject(new CObject(GetNewIndex(), 0, 0));
    }
    CTower *AddTower(char *szName, Team tTeam, float x = 0, float y = 0, float fRange = 0) {
        CTower *unit = (CTower *)AddObject(new CTower(GetNewIndex(), szName, tTeam));
        unit->x = x;
        unit->x = x;
        unit->fRange = fRange;
        vTowers.push_back(unit);
        return unit;
    }
    CHero *AddPlayer(char *szName, char *szType, DWORD skinId, Team tTeam) {
        CHero *unit = (CHero *)AddObject(new CHero(GetNewIndex(), szName, szType, tTeam, skinId));
        unit->playerIndex = vPlayers.size();
        vPlayers.push_back(unit);
        return unit;
    }
    CUnit *AddProp(char *szName, char *szType, Team tTeam) {
        CUnit *unit = (CUnit *)AddObject(new CUnit(GetNewIndex(), szName, szType, tTeam));
        vProps.push_back(unit);
        return unit;
    }
    struct SpawnInfo {
        BYTE bPlayer;
        BYTE bTeam;
        float fX;
        float fY;
        float fAngle1;
        float fAngle2;
    };
    bool Init() {
        static Vector3f blueSpawn[15] = {
            //single player 01
            { 35.902802f, 0, 273.551910f }, ///35.902802,273.551910 / 0.766044,0.642788
            //2 players 02,03
            { -74.097198f, 0, 383.551910f }, //-74.097198,383.551910 / 0.707107,-0.707107
            { 145.902802f, 0, 163.551910f }, //145.902802,163.551910 / -0.707107,0.707107
            //3 players 04,05,06
            { -70.197197f, 0, 167.451904f }, //-70.197197,167.451904 / 0.707107,0.707107
            { 180.802795f, 0, 234.751907f}, //180.802795,234.751907 / -0.965926,0.258819
            { -2.897200f, 0, 418.451904f }, //-2.897200,418.451904 / 0 .500000,-0.866025
            //4 players 07,08.9,10
            { 145.902802f, 0, 383.551910f }, //145.902802,383.551910 / -0.707107,-0.707107
            { 145.902802f, 0, 163.551910f }, //145.902802,163.551910 / -0.707107,0.707107
            { -74.097198f, 0, 163.551910f }, //-74.097198,163.551910 / 0.707107,0.707107
            { -74.097198f, 0, 383.551910f }, //-74.097198,383.551910 / 0.707107,-0.707107
            //5 players 11,12,13,14,15
            { 192.302795f, 0, 383.551910f }, //192.302795,383.551910 / -0.809017,-0.587785
            { 192.302795f, 0, 163.551910f }, //192.302795,163.551910 / -0.809017,0.587785
            { -16.897200f, 0, 95.551910f }, //-16.897200,95.551910 / 0.309017,0.951057
            { -146.297195f, 0, 273.551910f }, //-146.297195,273.551910 / 1.000000,-0.000000
            { -56.897202f, 0, 451.551910f }, //-56.897202,451.551910 / 0.309017,-0.951057
            //6 players 16,17,18,19,20,21 TODO
            /*{}, //
            {}, //
            {}, //
            {}, //
            {}, //
            {}, //*/
        };
        /* OLD*/
        /*  19 0 192.302795  , 383.551910, -0.809017,-0.587785
        	1A 1 192.302795  , 163.551910  -0.809017,0.587785
        	1B 2 -16.897200  , 95.551910    0.309017,0.951057
        	1C 3 -146.297195 , 273.551910   1.000000,-0.000000
        	1D 4 -56.897202  , 451.551910   0.309017,-0.951057*/
        static Vector3f blueStartPos[7] = {
            { 35.902802f, 0, 273.551910f },
            { 192.302795f, 184.615952f, 383.551910f }, //P1, top right
            { 192.302795f, 184.615921f, 163.551910f }, //P2, bottom right
            { -16.897200f, 184.615967f, 95.551910f }, //P3, bottom left
            { -146.297195f, 184.615982f, 273.551910f }, //P4, top left
            { -56.897202f, 184.615982f, 451.551910f }, //P5, top
            { 35.902802f, 0, 273.551910f },
        };
        static Vector3f redStartPos[7] = {
            { 13923.790039f, 0, 14170.091797f },
            { 14146.096680f, 184.973404f, 14314.807617f }, //Cassiopeia Bot
            { 14080.190430f, 184.973404f, 14070.091797f}, //Renekton Bot
            { 13870.990234f, 184.973419f, 14002.091797f}, //Galio Bot
            { 13741.589844f, 184.973419f, 14180.091797f}, //Master Yi Bot
            { 13830.990234f, 184.973389f, 14358.091797f}, //Jax Bot
            { 13923.790039f, 0, 14170.091797f },
        };
        AddObject(new CUnit()); //blank object - 0x40000000
        CTeam *b = server->blueTeam;
        CTeam *r = server->redTeam;
        //Towers
        CMovable *tower = b->GetBotLane()->AddTower(AddTower("@@Turret_T1_R_03_A", BLUE, 10097.618164, 808.732910, 1905)); //0x40000001 - T3 Bottom
        tower->SetPos(10097.618164, 808.732910, 190);
        tower = b->GetBotLane()->AddTower(AddTower("@@Turret_T1_R_02_A", BLUE, 6512.527344, 1262.614624, 1905)); //0x4000002 - T2 Bottom
        tower->SetPos(6512.527344, 1262.614624, 1905);
        tower = b->GetBotLane()->AddTower(AddTower("@@Turret_T1_C_07_A", BLUE, 3747.254883, 1041.043945, 800)); //0x4000003 - T1 Bottom
        tower->SetPos(3747.254883, 1041.043945, 800);
        //..
        tower = r->GetBotLane()->AddTower(AddTower("@@Turret_T2_R_03_A", RED, 13459.614258, 4284.239258, 1905)); //0x4000004
        tower->SetPos(13459.614258, 4284.239258, 1905);
        tower = r->GetBotLane()->AddTower(AddTower("@@Turret_T2_R_02_A", RED, 12920.789063, 8005.291992, 1905)); //0x4000005
        tower->SetPos(12920.789063, 8005.291992, 1905);
        tower = r->GetBotLane()->AddTower(AddTower("@@Turret_T2_R_01_A", RED, 13205.825195, 10474.619141, 800)); //0x4000006
        tower->SetPos(13205.825195, 10474.619141, 800);
        //..
        //..
        tower = b->GetMidLane()->AddTower(AddTower("@@Turret_T1_C_05_A", BLUE, 5435.023926, 6179.100098, 1905)); //0x4000007
        tower->SetPos(5435.023926, 6179.100098, 1905);
        tower = b->GetMidLane()->AddTower(AddTower("@@Turret_T1_C_04_A", BLUE, 4633.664063, 4591.909180, 1905)); //0x4000008
        tower->SetPos(4633.664063, 4591.909180, 1905);
        tower = b->GetMidLane()->AddTower(AddTower("@@Turret_T1_C_03_A", BLUE, 3233.994141, 3447.242188, 800)); //0x4000009
        tower->SetPos(3233.994141, 3447.242188, 800);
        tower = b->GetMidLane()->AddTower(AddTower("@@Turret_T1_C_01_A", BLUE, 1341.632568, 2029.984375, 1000)); //0x400000A
        tower->SetPos(1341.632568, 2029.984375, 1000);
        tower = b->GetMidLane()->AddTower(AddTower("@@Turret_T1_C_02_A", BLUE, 1768.191650, 1589.465576, 1000)); //0x400000B
        tower->SetPos(1768.191650, 1589.465576, 1000);
        //.
        tower = r->GetMidLane()->AddTower(AddTower("@@Turret_T2_C_05_A", RED, 8548.804688, 8289.496094, 1905)); //0x40000C
        tower->SetPos(8548.804688, 8289.496094, 1905);
        tower = r->GetMidLane()->AddTower(AddTower("@@Turret_T2_C_04_A", RED, 9361.072266, 9892.624023, 1905)); //0x40000D
        tower->SetPos(9361.072266, 9892.624023, 1905);
        tower = r->GetMidLane()->AddTower(AddTower("@@Turret_T2_C_03_A", RED, 10743.581055, 11010.062500, 800)); //0x40000E
        tower->SetPos(10743.581055, 11010.062500, 800);
        tower = r->GetMidLane()->AddTower(AddTower("@@Turret_T2_C_02_A", RED, 12662.488281, 12442.701172, 1000)); //0x40000F
        tower->SetPos(12662.488281, 12442.701172, 1000);
        tower = r->GetMidLane()->AddTower(AddTower("@@Turret_T2_C_01_A", RED, 12118.146484, 12876.628906, 1000)); //0x400010
        tower->SetPos(12118.146484, 12876.628906, 1000);
        //..
        //..
        AddTower("@@Turret_OrderTurretShrine_A", BLUE); //0x400011
        tower = AddTower("@@Turret_ChaosTurretShrine_A", RED, 14157.025391, 14456.352539, 1600); //0x400012
        tower->SetPos(14157.025391, 14456.352539, 1600);
        tower = b->GetTopLane()->AddTower(AddTower("@@Turret_T1_L_03_A", BLUE, 574.655029, 10220.470703, 1905)); //0x400013 - T3 Top
        tower->SetPos(574.655029, 10220.470703, 1905);
        tower = b->GetTopLane()->AddTower(AddTower("@@Turret_T1_L_02_A", BLUE, 1106.263428, 6485.252930, 1905)); //0x400014 - T2 Top
        tower->SetPos(1106.263428, 6485.252930, 1905);
        tower = b->GetTopLane()->AddTower(AddTower("@@Turret_T1_C_06_A", BLUE, 802.810364, 4052.360596, 800)); //0x400015 - T1 Top
        tower->SetPos(802.810364, 4052.360596, 800);
        //..
        tower = r->GetTopLane()->AddTower(AddTower("@@Turret_T2_L_03_A", RED, 3911.675293, 13654.815430, 1905)); //0x400016
        tower->SetPos(3911.675293, 13654.815430, 1905);
        tower = r->GetTopLane()->AddTower(AddTower("@@Turret_T2_L_02_A", RED, 7536.523438, 13190.815430, 1905)); //0x400017
        tower->SetPos(7536.523438, 13190.815430, 1905);
        tower = r->GetTopLane()->AddTower(AddTower("@@Turret_T2_L_01_A", RED, 10261.900391, 13465.911133, 800)); //0x40000018
        tower->SetPos(10261.900391, 13465.911133, 800);
        //Players
        //BlueTeam
        AddPlayer("Blue 1", "Nidalee", 2, BLUE);
        AddPlayer("Blue 2", "Ezreal", 2, BLUE);
        AddPlayer("Blue 3", "Elise", 2, BLUE);
        AddPlayer("Blue 4", "Braum", 2, BLUE);
        AddPlayer("Blue 5", "Karthus", 2, BLUE);
        AddPlayer("Blue 6", "Ziggs", 2, BLUE);
        //Rea Team
        AddPlayer("Red 1", "Leesin", 2, RED);
        AddPlayer("Red 2", "Gragas", 2, RED);
        AddPlayer("Red 3", "Riven", 2, RED);
        AddPlayer("Red 4", "Udyr", 2, RED);
        AddPlayer("Red 5", "Caitlyn", 2, RED);
        AddPlayer("Red 6", "Lux", 2, RED);
        //Props
        CUnit *prop = AddProp("LevelProp_Yonkey", "Yonkey", NEUTRAL); //0x4000001A
        prop->SetPos(12465.734375f, 101.585403f, 14422.256836f);
        prop = AddProp("LevelProp_Yonkey1", "Yonkey", NEUTRAL); //0x4000001B
        prop->SetPos(-76.055199f, 94.400597f, 1769.158936f);
        prop = AddProp("LevelProp_ShopMale", "ShopMale", NEUTRAL); //0x4000001C
        prop->SetPos(13374.174805f, 194.974091f, 14245.672852f);
        prop = AddProp("LevelProp_ShopMale1", "ShopMale", NEUTRAL); //0x4000001D
        prop->SetPos(-99.561302f, 191.403900f, 855.663208f);
        for(auto t : vTowers) {
            AddObject(t->GetActor(GetNewIndex())); //created border around towers
        }
        //Tower Actors..
        //set blue players positions
        auto vBlue = GetPlayers(BLUE);
        if(vBlue.size() == 1) {
            vBlue[0]->SetSpawn(0, blueStartPos[0]);
        } else {
            UINT iSpawn = 1;
            for(auto p : vBlue) {
                p->SetSpawn(iSpawn - 1, blueStartPos[iSpawn++]);
                p->RotateTo(Vector2f(blueStartPos[0].x, blueStartPos[0].z));
            }
        }
        //set red players positions
        auto vRed = GetPlayers(RED);
        if(vRed.size() == 1) {
            vRed[0]->SetSpawn(0, redStartPos[0]);
            vRed[0]->SetSpawn(0, blueStartPos[0]);//tp him to blue
        } else {
            UINT iSpawn = 1;
            for(auto p : vRed) {
                p->SetSpawn(iSpawn - 1 , redStartPos[iSpawn++]);
                p->RotateTo(Vector2f(redStartPos[0].x, redStartPos[0].z));
            }
        }
        for(auto p : GetPlayers()) {
            p->CalculateStats();
        }
        return true;
    }
};

CTower *CLane::AddTower(CTower *unit) {
    vUnits.push_back(unit);
    return unit;
}

CUnit *CLane::GetLiveTurret() {
    return NULL;
}

CLane::CLane() {
}

CLane *CTeam::GetBotLane() {
    return &lanes[2];
}

CLane *CTeam::GetMidLane() {
    return &lanes[1];
}

CLane *CTeam::GetTopLane() {
    return &lanes[0];
}

CTeam::CTeam(Team t) {
    team = t;
}
