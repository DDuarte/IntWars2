/*
IntWars playground server for League of Legends protocol testing
Copyright (C) 2012  Intline9 <Intline9@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "Packets.h"
#include "ObjectManager.h"
#include <enet/enet.h>
#include <time.h>
#include <intlib/general.h>

_SpawnParticle::_SpawnParticle(const char *szParticle, DWORD netID1, DWORD netID2, DWORD srcID) : _SpawnParticle(getHash(szParticle), netID1, netID2, srcID) {
}

_SpawnParticle::_SpawnParticle() {
    ZeroMemory(this, sizeof(_SpawnParticle));
    flags = 0x66;
    unk7 = 1.0;
    unk8 = 0;
}

_SpawnParticle::_SpawnParticle(DWORD dwHash, DWORD netID1, DWORD netID2, DWORD srcID) : _SpawnParticle() {
    objHash = dwHash;
    netId1 = netID2;
    //...
    srcId1 = srcID;
    srcId2 = srcID;
    //..
    netId2 = netID1;
}

void _SpawnParticle::SetPos(float fX, float fY) {
    toX = toX2 = fX;
    toZ = toZ2 = fY;
}


_RemoveBuffPacket::_RemoveBuffPacket(DWORD netID, BYTE bSlot, DWORD buffHash) {
    ZeroMemory(this, sizeof(_RemoveBuffPacket));
    header.cmd = (PacketCmd)0x7A;
    header.netId = netID;
    this->bSlot = bSlot;
    this->buffHash = buffHash;
    fUnk = 0;
}

_ApplyBuffPacket::_ApplyBuffPacket(DWORD netID, BYTE bSlot, BYTE bType, BYTE nStacks, BYTE bVisible, DWORD dwHash1, DWORD dwHash2, float fTime) {
    ZeroMemory(this, sizeof(_ApplyBuffPacket));
    header.cmd = (PacketCmd)0xB6;
    header.netId = netID;
    this->bSlot = bSlot;
    this->bType = bType;
    this->nStacks = nStacks;
    this->bVisible = bVisible;
    buffID = dwHash1;
    targetID = dwHash2;
    this->fTime = fTime;
    srcID = netID;
}

_UpdateDash::_UpdateDash(DWORD netID, float fX, float fY, float fZ, float fRotation) {
    ZeroMemory(this, sizeof(_UpdateDash));
    header.cmd = (PacketCmd)0x4F;
    header.netId = netID;
    this->f1 = fX;
    this->f2 = fY;
    this->f3 = fZ;
    this->f4 = fRotation;
}

_UpdateDrawPos::_UpdateDrawPos(DWORD netID, float fX, float fY, BYTE bUnk, DWORD netId2) {
    ZeroMemory(this, sizeof(_UpdateDrawPos));
    header.cmd = (PacketCmd)0xFE;
    header.netId = netID;
    unk1 = 0x0F; //bHeader (0x10F)
    unk2 = 1;
    unk3 = bUnk;
    this->fX = fX;
    this->fZ = fY;
    this->netId = netId2;
}

_AttackProjectilePacket::_AttackProjectilePacket(DWORD srcId, DWORD targetId, DWORD projectileId, BYTE bAnimation, float fX, float fY) {
    ZeroMemory(this, sizeof(_AttackProjectilePacket));
    header.cmd = (PacketCmd)0x1A;
    header.netId = srcId;
    this->targetId = targetId;
    bUnk = 0x80;
    this->projectileId = projectileId;
    spellId = bAnimation; //0x40 for ezreal,0x41 for nidalee, probably attack animation
    this->fX = fX;
    this->fY = fY;
}

_MeleeAttackPacket::_MeleeAttackPacket(DWORD srcId, DWORD targetId, DWORD projectileId, BYTE bAnimation, float fX, float fY) {
    ZeroMemory(this, sizeof(_MeleeAttackPacket));
    header.cmd = (PacketCmd)0x0C;
    header.netId = srcId;
    this->targetId = targetId;
    bUnk = 0x80;
    this->projectileId = projectileId;
    spellId = bAnimation; //0x40 for ezreal,0x41 for nidalee, probably attack animation
    this->fX = fX;
    this->fY = fY;
}

_AggroPacket::_AggroPacket(DWORD srcId, DWORD targetId) {
    ZeroMemory(this, sizeof(_AggroPacket));
    header.cmd = (PacketCmd)0xBF;
    header.netId = srcId;
    this->targetId = targetId;
}

_AbortAnimationPacket::_AbortAnimationPacket(DWORD srcId) {
    ZeroMemory(this, sizeof(_AbortAnimationPacket));
    header.cmd = (PacketCmd)0x33;
    header.netId = srcId;
}

_SetSpellPacket::_SetSpellPacket(DWORD netID, DWORD slotId, char *szSpell) {
    ZeroMemory(this, sizeof(_SetSpellPacket));
    header.cmd = PKT_S2C_SetSpellName;
    header.netId = netID;
    this->slotId = slotId;
    bUnk = 1;
    strncpy(this->szSpell, szSpell, 32);
}

_AnimateSpellPacket::_AnimateSpellPacket(DWORD netID, DWORD spellID, char *szSpell) {
    ZeroMemory(this, sizeof(_AnimateSpellPacket));
    header.cmd = PKT_S2C_AnimateSpell; //PKT_S2C_AnimateSpell
    header.netId = netID;
    spellId = spellID;
    strncpy(this->szSpell, szSpell, 32);
}

_DisplaySpellPacket::_DisplaySpellPacket(DWORD dwHash, DWORD srcID, DWORD objId1, DWORD objId2) {
    ZeroMemory(this, sizeof(_DisplaySpellPacket));
    header.cmd = (PacketCmd)0xB4;
    header.netId = srcID;
    unk1 = clock();
    spawn = _SpawnParticle(dwHash, objId1, objId2, srcID);
    unk17 = 1;
}

_ReleaseMissilePacket::_ReleaseMissilePacket(DWORD netID, DWORD objID) {
    ZeroMemory(this, sizeof(_ReleaseMissilePacket));
    header.cmd = (PacketCmd)0x6D;
    header.netId = netID;
    objId = objID;
}

_EliseSpider3::_EliseSpider3(DWORD netID, DWORD unk3) {
    ZeroMemory(this, sizeof(_EliseSpider3));
    header.cmd = (PacketCmd)0x17;
    header.netId = netID;
    unk1 = 3;
    unk2 = 5;
    this->unk3 = unk3;
}

_RestoreModel::_RestoreModel(DWORD netID) {
    ZeroMemory(this, sizeof(_RestoreModel));
    header.cmd = (PacketCmd)0x66;
    header.netId = netID;
    id = netID & ~0x40000000;
}

_UpdateModel::_UpdateModel(DWORD netID, const char *szModel) {
    ZeroMemory(this, sizeof(_UpdateModel));
    header.cmd = (PacketCmd)0x96;
    header.netId = netID;
    id = netID & ~0x40000000;
    bOk = 1;
    unk1 = -1;
    strncpy((char *)szName, szModel, 32);
}

_PingCheckResponse::_PingCheckResponse(float fTime1, float fTime2) {
    ZeroMemory(this, sizeof(_PingCheckResponse));
    header.cmd = (PacketCmd)0x75;
    this->fTime1 = fTime1;
    this->delta = fTime2 - fTime1;
    this->fTime2 = fTime2;
}

_EmotionResponse::_EmotionResponse() {
    header.cmd = PKT_S2C_Emotion;
}

_SwapItemAns::_SwapItemAns(DWORD netID, int iSlot1, int iSlot2) {
    ZeroMemory(this, sizeof(_SwapItemAns));
    header.cmd = PKT_C2S_SwapItemAns;
    header.netId = netID;
    this->iSlot1 = iSlot1;
    this->iSlot2 = iSlot2;
}

_SellItemAns::_SellItemAns(DWORD netID, int slot, int stack) {
    ZeroMemory(this, sizeof(_SellItemAns));
    header.cmd = PKT_S2C_SellItemAns;
    header.netId = netID;
    slotId = slot;
    this->stack = stack;
}

_BuyItemAns::_BuyItemAns(DWORD netID, int item, int slot, int stack) : _BuyItemAns() {
    header.netId = netID;
    itemId = item;
    slotId = slot;
    this->stack = stack;
}

_BuyItemAns::_BuyItemAns() {
    ZeroMemory(this, sizeof(_BuyItemAns));
    header.cmd = PKT_S2C_BuyItemAns;
}

_SkillUpResponse::_SkillUpResponse(DWORD netID) {
    header.cmd = PKT_S2C_SkillUp;
    header.netId = netID;
    skill = 0;
    level = 0;
    pointsLeft = 0;
}

SpellBook::SpellBook(DWORD netID) {
    ZeroMemory(this, sizeof(SpellBook));
    header.cmd = PacketCmd(0xB7); // TODO: Update static to common.h variables 
    header.netId = netID;
    fZ = 25000;//1;
    this->netID = netID;
}

SpellSetPacket::SpellSetPacket(DWORD netID, DWORD _spellID, DWORD _level) {
    ZeroMemory(this, sizeof(SpellSetPacket));
    header.cmd = PacketCmd(0x5A);
    header.netId = netID;
    spellID = _spellID;
    level = _level;
}

void LevelPropSpawn::SetProp(char *szName, char *szType, float fX, float fY, float fZ) {
    ZeroMemory(this, sizeof(LevelPropSpawn));
    header.cmd = PKT_S2C_LevelPropSpawn;
    netId = 0;
    bFlag = 0x40;
    x = fX;
    y = fY;
    z = fZ;
    f1 = 1.0;
    f2 = 1.0;
    f3 = 1.0;
    dwUnk1 = 300;
    bPos = 1;
    if(szName) {
        strcpy(reinterpret_cast<char *>(name), szName);
    }
    if(szType) {
        strcpy(reinterpret_cast<char *>(type), szType);
    }
}

LevelPropSpawn::LevelPropSpawn() {
    ZeroMemory(this, sizeof(LevelPropSpawn));
    header.cmd = PKT_S2C_LevelPropSpawn;
    netId = 0;
}

VisionRange::VisionRange(DWORD netID, DWORD actorID, DWORD dwTeam, float x, float y, float fRange) {
    ZeroMemory(this, sizeof(VisionRange));
    header.cmd = PKT_S2C_FogUpdate2;
    header.netId = 0;
    this->dwTeam = dwTeam;
    dwUnk2 = -1;
    this->netID = netID;
    this->actorID = actorID;
    this->x = x;
    this->y = y;
    z = 25000.0f;
    xRange = 88.400002f;
    yRange = 130.0f;
    zRange = 1.0f;
    dwUnk4 = 0;
    dwUnk5 = 7;
    fUnk = fRange;
}

GameTimerUpdate::GameTimerUpdate(float fTime) {
    header.cmd = PKT_S2C_GameTimerUpdate;
    header.netId = 0;
    this->fTime = fTime;
}

GameTimer::GameTimer(float fTime) {
    header.cmd = PKT_S2C_GameTimer;
    header.netId = 0;
    this->fTime = fTime;
}

TurretSpawn::TurretSpawn(DWORD netID, const char *szName) : TurretSpawn() {
    header.netId = netID;
    tID = netID;
    strncpy(reinterpret_cast<char *>(name), szName, 29);
}

TurretSpawn::TurretSpawn() {
    header.cmd = PKT_S2C_TurretSpawn;
    tID = 0;
    unk = 0;
    memset(&name, 0, 29 + 42); //Set name + type to zero
}

TurretSpawn2::TurretSpawn2(DWORD netID) {
    ZeroMemory(this, sizeof(TurretSpawn2));
    header.cmd = PacketCmd(0xAE);
    header.netId = netID;
    f1 = 1.0;
}

SetupSpell2::SetupSpell2(DWORD netID, DWORD spellID) {
    ZeroMemory(this, sizeof(SetupSpell2));
    header.cmd = PacketCmd(0xFE);
    header.netId = netID;
    unk1 = 7;
    unk2 = 1;
    spellId = spellID;
    unk3 = 0;
    unk4 = -1;
    unk5 = 0;
    unk6 = 0;
}

SetupSpell1::SetupSpell1(DWORD netID, DWORD spellID) {
    ZeroMemory(this, sizeof(SetupSpell1));
    header.cmd = PacketCmd(0x85);
    header.netId = netID;
    spellId = spellID;
    unk = 0;
    f1 = -1.0;
}

SpawnHealthPoints::SpawnHealthPoints(DWORD netID, float health, float maxHealth) {
    ZeroMemory(this, sizeof(SpawnHealthPoints));
    header.cmd = PacketCmd(0xAD);
    header.netId = netID;
    unk = 0;
    this->health = health;
    this->maxHealth = maxHealth;
}

HeroSpawn4::HeroSpawn4(DWORD netId, float fX, float fY, BYTE spawnIndex, float fAngle) {
    ZeroMemory(this, sizeof(HeroSpawn4));
    header.cmd = PKT_S2C_Waypoints;
    header.netId = netId;
    unk1 = 0x01;
    unk2 = 0x07;
    unk3 = 0x01;
    unk4 = 0x00;
    unk5 = 0xD1;
    unk6 = 0x07;
    fUnk2 = 1.0;
    bUnk1 = 3;
    dwUnk = spawnIndex;
    f1 = fX;// 145.902802;
    f2 = fY;// 163.551910;
    f3 = fAngle;// 0;// -0.707107;
    f4 = -fAngle;// 0;// 0.707107;
}

HeroSpawn3::HeroSpawn3(DWORD netId, float fX, float fY, BYTE spawnIndex, float fAngle) {
    ZeroMemory(this, sizeof(HeroSpawn3));
    header.cmd = PKT_S2C_Waypoints;
    header.netId = netId;
    fUnk1 = 1.0;
    bUnk1 = 3;
    dwUnk = spawnIndex;
    f1 = fX;// 145.902802;
    f2 = fY;// 163.551910;
    f3 = -fAngle;//-0.707107;
    f4 = fAngle;// 0.707107;
}

InitSpawn::InitSpawn(DWORD netID) {
    ZeroMemory(this, sizeof(InitSpawn));
    header.cmd = PKT_S2C_Masteries;
    header.netId = netID;
    /*for(int i = 0; i < 9; i++) { int1[i] = 5245; }
    for(int i = 0; i < 9; i++) { int2[i] = 5275; }
    for(int i = 0; i < 4; i++) { int3[i] = 5305; }
    for(int i = 0; i < 5; i++) { int4[i] = 5317; }
    for(int i = 0; i < 3; i++) { int5[i] = 5335; }
    for(int i = 0; i < 17; i++) { pts[i].SetPos(0, 0); }
    unk4 = 3;*/
    //0
    unk6 = 1310;
}

void InitPoint::SetPos(WORD x, WORD y) {
    this->x = x;
    this->y = y;
}

InitPoint::InitPoint(WORD x, WORD y) {
    this->x = x;
    this->y = y;
    _0 = 0;
}

InitPoint::InitPoint() {
    x = 0;
    y = 0;
    _0 = 0;
}

HeroSpawn::HeroSpawn(DWORD netId, DWORD playerNo, bool isBlue, bool isBot, BYTE location, DWORD skinId, char *szName, char *szType) {
    ZeroMemory(this, sizeof(HeroSpawn));
    header.cmd = PKT_S2C_HeroSpawn;
    this->netId = netId;
    this->playerNo = playerNo;
    this->unk1 = 0x40;
    this->unk2 = 0x2A;
    this->isBlue = isBlue;
    this->isBot = isBot;
    this->unk5 = 0xE0;
    this->location = location;
    this->skinId = skinId;
    strncpy((char *)name, szName, 128);
    strncpy((char *)type, szType, 64);
}

FogUpdate2::FogUpdate2() {
    header.cmd = PKT_S2C_FogUpdate2;
    header.netId = 0x40000019;
}

_StatePacket2::_StatePacket2(PacketCmd state) {
    header.cmd = state;
    nUnk = 0;
}

char *ChatMessage::getMessage() {
    return &msg;
}

_WorldSendGameNumber::_WorldSendGameNumber(DWORD gameId, char *szServer, char *szUsername) {
    ZeroMemory(this, sizeof(_WorldSendGameNumber));
    header.cmd = PKT_World_SendGameNumber;
    this->gameId = gameId;
    strncpy((char *)platformId, szServer, 32);
    strncpy((char *)userName, szUsername, 128);
}

_QueryStatus::_QueryStatus() {
    header.cmd = PKT_S2C_QueryStatusAns;
    ok = 1;
}

_ViewAns::_ViewAns() {
    cmd = PKT_S2C_ViewAns;
    unk1 = 0;
}

_AttentionPingAns::_AttentionPingAns(AttentionPing *ping) {
    attentionPing = AttentionPing(ping);
    attentionPing.cmd = PKT_S2C_AttentionPing;
}

_AttentionPing::_AttentionPing(_AttentionPing *ping) {
    cmd = ping->cmd;
    unk1 = ping->unk1;
    x = ping->x;
    y = ping->y;
    z = ping->z;
    unk2 = ping->unk2;
    type = ping->type;
}

_AttentionPing::_AttentionPing() {
}

_KeyCheck::_KeyCheck(DWORD playerNo, DWORD64 userId, DWORD dwKeyUnk) {
    ZeroMemory(this, sizeof(_KeyCheck));
    cmd = PKT_KeyCheck;
    _10000 = 10000;
    this->playerNo = playerNo;
    this->userId = userId;
    this->dwKeyUnk = dwKeyUnk;
}

DWORD _LoadScreenPlayer::getPacketLength() {
    return sizeof(_LoadScreenPlayer) + length;
}

BYTE *_LoadScreenPlayer::getDescription() {
    return &description;
}

void _LoadScreenPlayer::destroy(_LoadScreenPlayer *packet) {
    delete[](BYTE *)packet;
}

_LoadScreenPlayer *_LoadScreenPlayer::create(PacketCmd cmd, char *str, DWORD size) {
    //Builds packet buffer
    DWORD totalSize = sizeof(_LoadScreenPlayer) + size + 1;
    BYTE *buf = new BYTE[totalSize];
    memset(buf, 0, totalSize);
    //Set defaults
    _LoadScreenPlayer *packet = (_LoadScreenPlayer *)buf;
    packet->cmd = cmd;
    packet->length = size;
    packet->userId = 0;
    packet->skinId = 0;
    memcpy(packet->getDescription(), str, packet->length);
    return packet;
}

_LoadScreenInfo::_LoadScreenInfo() {
    //Zero this complete buffer
    memset(this, 0, sizeof(_LoadScreenInfo));
    cmd = PKT_S2C_LoadScreenInfo;
    blueMax = redMax = 6;
}

_SynchVersionAns::_SynchVersionAns(DWORD mapId, char *szVersion, char *szMapNode) {
    ZeroMemory(this, sizeof(_SynchVersionAns));
    header.cmd = PKT_S2C_SynchVersion;
    ok = 1; //not ok, some bitmask
    this->mapId = mapId;
    strncpy((char *)version, szVersion, 256);
    strncpy((char *)gameMode, szMapNode, 256);
    for(UINT i = 0; i < 12; i++) {
        players[i] = _SynchBlock();
    }
    dwOpt = 0x377192;
    for(UINT i = 0; i < 1; i++) {
        bUnk2[i] = 1;
    }
}

_SynchBlock::_SynchBlock() {
    ZeroMemory(this, sizeof(_SynchBlock));
    userId = 0xFFFFFFFFFFFFFFFF;
}

GameHeader::GameHeader() {
    netId = ticks = 0;
}

PacketHeader::PacketHeader() {
    cmd = (PacketCmd)0xFF;
    netId = 0;
}
