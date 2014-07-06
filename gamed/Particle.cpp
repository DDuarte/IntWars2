#include "stdafx.h"
#include "Particle.h"
#include "ObjectManager.h"
#include "EmitParticlesPacket.h"
#include "Server.h"

CParticle::CParticle(CHero *hero, DWORD dwVar) {
    ZeroMemory(this, sizeof(CParticle));
    auto obj = CObjectManager::NewObject();
    netID = obj->GetNetworkID();
    dwHash = dwVar;
    src = hero;
    dwHeroHash = hero->GetHash();
    fX = hero->GetX();
    fY = hero->GetY();
}
CParticle::CParticle(CHero *hero, char *szParticle) {
    ZeroMemory(this, sizeof(CParticle));
    auto obj = CObjectManager::NewObject();
    netID = obj->GetNetworkID();
    dwHash = getHash(szParticle);
    dwFlags = 0x20;
    src = hero;
    dwHeroHash = hero->GetHash();
    fX = hero->GetX();
    fY = hero->GetY();
}
void CParticle::write(SendablePacket *p) {
    //... header
    p->write<DWORD>(dwHeroHash);
    p->write<DWORD>(dwHash);
    p->write<DWORD>(dwFlags);
    p->write<DWORD>(0);
    p->write<WORD>(0);
    p->write<char>(1); //1 single particle
    //64 bites data
    p->write<DWORD>(0); //srcID?
    p->write<DWORD>(netID);
    p->write<DWORD>(src->GetNetworkID()); //srcID
    p->write<DWORD>(applyToId); //applyToID
    p->write<DWORD>(0);
    //Here come 3 position vectors, not sure how do get position to work.
    for(UINT i = 0; i < 3; i++) {
        p->write<WORD>((fX - (*server).GetMap().GetSize().x) / 2);
        p->write<float>(0);// i == 1 ? 0 : 109.403847);
        p->write<WORD>((fY - (*server).GetMap().GetSize().y) / 2);
    }
    //..
    p->write<float>(0);
    p->write<float>(0);
    p->write<float>(0);
    p->write<float>(0);
    p->write<float>(1.0);
}

void CParticle::Broadcast() {
    EmitParticlesPacket p;
    p.vParticles.push_back(*this);
    p.Broadcast();
}