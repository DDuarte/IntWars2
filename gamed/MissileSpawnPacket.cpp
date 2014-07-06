#include "stdafx.h"
#include "MissileSpawnPacket.h"
#include "ObjectManager.h"
#include "Hero.h"
#include "common.h"

MissileSpawnPacket::MissileSpawnPacket(CHero *hero, DWORD dwMissileHash, float fX, float fY) : SendablePacket(CHL_S2C, 0) {
    this->clear();
    auto obj1 = CObjectManager::NewObject();
    auto obj2 = CObjectManager::NewObject();
    this->write<unsigned char>(0x3A);
    this->write<unsigned int>(obj1->GetNetworkID());				//netID
    //spell start
    this->write<float>(hero->GetX());
    this->write<float>(0);
    this->write<float>(hero->GetY());
    //hero pos
    this->write<float>(hero->GetX());
    this->write<float>(0);
    this->write<float>(hero->GetY());
    //.. angles
    this->write<float>(0);
    this->write<float>(0);
    this->write<float>(0);
    //.. unk
    this->write<float>(1738.0);
    this->write<float>(-166.66);
    this->write<float>(988.31);
    //hero head
    this->write<float>(hero->GetX());
    this->write<float>(0);
    this->write<float>(hero->GetY());
    //spell end
    this->write<float>(fX);
    this->write<float>(0);
    this->write<float>(fY);
    //hero pos
    this->write<float>(hero->GetX());
    this->write<float>(0);
    this->write<float>(hero->GetY());
    //
    this->write<float>(0);//cast delay
    this->write<float>(2000.0);//proj speed
    this->write<DWORD>(0);
    this->write<DWORD>(0);
    this->write<DWORD>(0x7F7FFFFF);
    this->write<char>(0);
    //Same as SpellCmd from here on
    //..
    this->write<unsigned short>(0x66); //wFlags
    this->write<unsigned int>(dwMissileHash);
    this->write<unsigned int>(obj2->GetNetworkID()); //netID1
    //..
    this->write<unsigned char>(0);
    this->write<float>(1.0);
    //..
    this->write<unsigned int>(hero->GetNetworkID());				//netID
    this->write<unsigned int>(hero->GetNetworkID());				//netID
    //..
    this->write<unsigned int>(hero->GetHash());
    this->write<unsigned int>(obj1->GetNetworkID());				//netID2
    //..
    this->write<float>(fX);
    this->write<float>(0);
    this->write<float>(fY);
    this->write<float>(fX);
    this->write<float>(0);
    this->write<float>(fY);
    //..
    this->write<unsigned char>(0);
    this->write<float>(-1.0); //fCastTime
    this->write<float>(0); //delay betwen spell and animation/sound
    this->write<float>(1.0); //0 invisible - more than 0 visible
    //Dunno these 14 bytes
    this->write<DWORD>(0);
    this->write<DWORD>(0);
    this->write<DWORD>(0);
    this->write<WORD>(0);
    this->write<float>(hero->GetX());
    this->write<float>(0);
    this->write<float>(hero->GetY());
    this->write<unsigned int>(0);
    this->write<unsigned int>(0);
}
MissileSpawnPacket::MissileSpawnPacket(CHero *hero, const char *szMissile, float fX, float fY) : MissileSpawnPacket(hero, getHash(szMissile), fX, fY) {
}


MissileSpawnPacket::~MissileSpawnPacket() {
}