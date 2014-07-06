#include "stdafx.h"
#include "EmitParticlesPacket.h"

#include "Packets.h"
#include "Particle.h"

EmitParticlesPacket::EmitParticlesPacket() : SendablePacket(CHL_S2C, 0) {
    this->clear();
}

void EmitParticlesPacket::encode() {
    this->write<unsigned char>(0x86);
    this->write<unsigned int>(0);				//netID
    this->write<BYTE>(vParticles.size());
    for(auto p : vParticles) {
        p.write(this);
    }
}


EmitParticlesPacket::~EmitParticlesPacket() {
}

DWORD EmitParticlesPacket::AddParticle2(CHero *hero, DWORD dwVar) {
    CParticle p(hero, dwVar);
    vParticles.push_back(p);
    return p.netID;
}

DWORD EmitParticlesPacket::AddParticle(CHero *hero, const char *szParticle) {
    CParticle p(hero, (char *)szParticle);
    vParticles.push_back(p);
    return p.netID;
}
