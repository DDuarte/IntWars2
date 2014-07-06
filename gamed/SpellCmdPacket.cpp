#include "stdafx.h"
#include "SpellCmdPacket.h"
#include "ObjectManager.h"
#include "common.h"

SpellCmdPacket::SpellCmdPacket(unsigned int networkID, unsigned short wFlag, char spellIndex, unsigned int dwSpellHash, unsigned int dwSpellHash2, unsigned int dwNetId1, unsigned int dwNetId2, Vector3f from, Vector3f to,
                               float fChannelTime, float fDelay, float fVisible, float fCooldown, float fMana) : SendablePacket(CHL_S2C, 0) {
    this->netID = dwNetId1;
    this->clear();
    this->write<unsigned char>(0xB4);
    this->write<unsigned int>(networkID);				//netID
    this->write<unsigned int>(clock());
    this->write<unsigned char>(0);				//ok
    //..
    this->write<unsigned short>(wFlag);
    this->write<unsigned int>(dwSpellHash);
    this->write<unsigned int>(dwNetId2); //netID1
    //..
    this->write<unsigned char>(0);
    this->write<float>(1.0);
    //..
    this->write<unsigned int>(networkID);				//netID
    this->write<unsigned int>(networkID);				//netID
    //..
    this->write<unsigned int>(dwSpellHash2);
    this->write<unsigned int>(dwNetId1);				//netID2
    //..
    this->write<float>(to.x); //EndPosition by server
    this->write<float>(to.y);
    this->write<float>(to.z);
    this->write<float>(to.x); //EndPosition by client
    this->write<float>(to.y);
    this->write<float>(to.z);
    //..
    this->write<unsigned char>(0);
    this->write<float>(fChannelTime); //0.25
    this->write<float>(fDelay); //delay betwen spell and animation/sound
    this->write<float>(fVisible); //0 invisible - more than 0 visible
    this->write<float>(fCooldown); //cd , does not work for some spells
    this->write<unsigned int>(0);
    this->write<unsigned char>(0);
    this->write<unsigned char>(spellIndex);
    this->write<float>(fMana); //mana
    this->write<float>(from.x); // real from, confirmed
    this->write<float>(from.y);
    this->write<float>(from.z);
    this->write<unsigned int>(1);
    this->write<unsigned int>(0);
}

SpellCmdPacket::SpellCmdPacket() : SendablePacket(CHL_S2C, 0) {
    netID = 0;
}

SpellCmdPacket *SpellCmdPacket::_BuildSpell(CHero *hero, unsigned int spellIndex, DWORD spellHash, unsigned int dwFlag, float fX, float fY, float fCastTime, float fDelay, float fVisible, float fCooldown, float fMana) {
    auto obj1 = CObjectManager::NewObject();
    auto obj2 = CObjectManager::NewObject();
    return new SpellCmdPacket(hero->GetNetworkID(), dwFlag, spellIndex, spellHash, hero->GetHash(), obj1->GetNetworkID(), obj2->GetNetworkID(), { hero->GetX(), 0, hero->GetY() }, { fX, 0, fY }, fCastTime, fDelay, fVisible, fCooldown, fMana);
}
SpellCmdPacket SpellCmdPacket::BuildSpell(CHero *hero, unsigned int spellIndex, DWORD spellHash, unsigned int dwFlag, float fX, float fY, float fCastTime, float fDelay, float fVisible, float fCooldown, float fMana) {
    auto obj1 = CObjectManager::NewObject();
    auto obj2 = CObjectManager::NewObject();
    return SpellCmdPacket(hero->GetNetworkID(), dwFlag, spellIndex, spellHash, hero->GetHash(), obj1->GetNetworkID(), obj2->GetNetworkID(), { hero->GetX(), 0, hero->GetY() }, { fX, 0, fY }, fCastTime, fDelay, fVisible, fCooldown, fMana);
}

SpellCmdPacket SpellCmdPacket::BuildSpell(CHero *hero, unsigned int spellIndex, unsigned int dwFlag, float fX, float fY, float fCastTime, float fDelay, float fVisible, float fCooldown, float fMana) {
    return SpellCmdPacket::BuildSpell(hero, spellIndex, getHash((char *)hero->GetSpellName(spellIndex).c_str()), dwFlag, fX, fY, fCastTime, fDelay, fVisible, fCooldown, fMana);
}
SpellCmdPacket::~SpellCmdPacket() {
}