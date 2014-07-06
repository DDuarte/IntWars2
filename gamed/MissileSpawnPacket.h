#pragma once
#include "SendablePacket.h"
#include "Hero.h"

class CHero;
class MissileSpawnPacket : public SendablePacket {
    public:
        MissileSpawnPacket(CHero *hero, DWORD dwMissileHash, float fX, float fY);
        MissileSpawnPacket(CHero *hero, const char *szMissile, float fX, float fY);
        ~MissileSpawnPacket();
};
