#pragma once
#include "SendablePacket.h"
#include "Hero.h"
class EmitParticlesPacket : public SendablePacket {
    public:
        std::vector<CParticle> vParticles;

        EmitParticlesPacket();
        ~EmitParticlesPacket();

        DWORD AddParticle(CHero *hero, const char *szParticle);
        DWORD AddParticle2(CHero *hero, DWORD dwVar);
        virtual void encode();
};
