#pragma once
#include "SendablePacket.h"

class CHero;
class CParticle {
    public:
        CHero *src;
        DWORD dwHeroHash;
        DWORD dwHash;
        DWORD dwFlags;
        DWORD netID;
        DWORD applyToId;
        float fX;
        float fY;

        float fTimer;//for custom use

        CParticle(CHero *hero, DWORD dwVar);
        CParticle(CHero *hero, char *szParticle);
        void write(SendablePacket *p);
        void Broadcast();
};

//Add a particle group