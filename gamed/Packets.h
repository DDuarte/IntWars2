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
#ifndef _PACKETS_H
#define _PACKETS_H

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif
#include "common.h"

struct PacketHeader {
    PacketHeader();

    PacketCmd cmd;
    DWORD netId;
};

struct GameHeader {
    GameHeader();

    GameCmd cmd;
    DWORD netId;
    DWORD ticks;
};
struct ClientReady {
    DWORD cmd;
    DWORD playerId;
    DWORD teamId;
};


typedef struct _SynchBlock {
    _SynchBlock();
    DWORD64 userId;
    WORD wUnk;
    DWORD skill1;
    DWORD skill2;
    BYTE bBot;
    DWORD teamId;
    BYTE name[64];
    BYTE type[64];
    BYTE rank[30];
} SynchBlock;
typedef struct _SynchVersionAns {
    _SynchVersionAns(DWORD mapId, char *szVersion, char *szMapNode);

    PacketHeader header;
    BYTE ok;
    DWORD mapId;
    SynchBlock players[12];
    BYTE version[256];
    BYTE gameMode[128];
    BYTE unk1[512];
    BYTE unk2[245];

    BYTE ekg1[256]; //ekg.riotgames.net
    BYTE msg1[256]; //"/messages"

    WORD wUnk1; //0x50?
    BYTE ekg2[256]; //ekg.riotgames.net
    BYTE msg2[256]; //"/messages"

    WORD wUnk2; //0x50?
    BYTE ekg3[256]; //ekg.riotgames.net
    BYTE msg3[256]; //"/messages"

    WORD wUnk3; //0x50?
    DWORD dwUnk1;
    DWORD dwOpt; //0x377192
    BYTE bUnk1[0x100];
    BYTE bUnk2[11];
} SynchVersionAns;

typedef struct _PingLoadInfo {
    PacketHeader header;
    DWORD unk1;
    DWORD64 userId;
    float loaded;
    float ping;
    float f3;
} PingLoadInfo;

BYTE *createDynamicPacket(BYTE *str, DWORD size);

typedef struct _LoadScreenInfo {
    _LoadScreenInfo();

    BYTE cmd;
    DWORD blueMax;
    DWORD redMax;
    DWORD64 bluePlayerIds[6]; //Team 1, 6 players max
    BYTE blueData[144];
    DWORD64 redPlayersIds[6]; //Team 2, 6 players max
    BYTE redData[144];
    DWORD bluePlayerNo;
    DWORD redPlayerNo;
} LoadScreenInfo;

typedef struct _LoadScreenPlayer {
    static _LoadScreenPlayer *create(PacketCmd cmd, char *str, DWORD size);
    static void destroy(_LoadScreenPlayer *packet);
    BYTE cmd;
    DWORD64 userId;
    DWORD skinId;
    DWORD length;
    BYTE description;
    BYTE *getDescription();

    DWORD getPacketLength();
} LoadScreenPlayer;


typedef struct _KeyCheck {
    _KeyCheck(DWORD playerNo, DWORD64 userId, DWORD dwKeyUnk);

    BYTE cmd;
    BYTE partialKey[3];   //Bytes 1 to 3 from the blowfish key for that client
    DWORD playerNo;
    DWORD64 userId;         //BYTE testVar[8];   //User id
    DWORD dwKeyUnk;
    DWORD dwKeyCheckBegin;
    DWORD _10000;
    DWORD unk1;
} KeyCheck;

typedef struct _AttentionPing {
    _AttentionPing();
    _AttentionPing(_AttentionPing *ping);

    BYTE cmd;
    DWORD unk1;
    float x;
    float y;
    float z;
    DWORD unk2;
    BYTE type;
} AttentionPing;

typedef struct _AttentionPingAns {
    _AttentionPingAns(AttentionPing *ping);

    AttentionPing attentionPing;
    DWORD response;
} AttentionPingAns;

struct CameraLock {
    PacketHeader header;
    BYTE isLock;
    DWORD padding;
};

typedef struct _ViewReq {
    BYTE cmd;
    DWORD unk1;
    float x;
    float zoom;
    float y;
    float y2;		//Unk
    DWORD width;	//Unk
    DWORD height;	//Unk
    DWORD unk2;	//Unk
    BYTE requestNo;
} ViewReq;

typedef struct _ViewAns {
    _ViewAns();

    BYTE cmd;
    DWORD unk1;
    BYTE requestNo;
} ViewAns;


typedef struct _QueryStatus {
    _QueryStatus();
    PacketHeader header;
    BYTE ok;
} QueryStatus;

typedef struct _SynchVersion {
    PacketHeader header;
    DWORD unk1;
    BYTE version[50]; //Dunno how big and when usefull data begins
} SynchVersion;

typedef struct _WorldSendGameNumber {
    _WorldSendGameNumber(DWORD gameId, char *szServer, char *szUsername);
    PacketHeader header;
    DWORD64 gameId;
    BYTE platformId[32];
    BYTE userName[128];
} WorldSendGameNumber;

struct ChatMessage {
    BYTE cmd;
    DWORD playerNo;
    BYTE unk2[5];
    ChatType type;
    DWORD unk3;
    DWORD lenght;
    BYTE unk4[32];
    char msg;

    char *getMessage();
};

typedef struct _StatePacket {
    _StatePacket(PacketCmd state) {
        header.cmd = state;
    }
    PacketHeader header;
} StatePacket;

typedef struct _StatePacket2 {
    _StatePacket2(PacketCmd state);
    PacketHeader header;
    WORD nUnk;
} StatePacket2;

struct FogUpdate2 {
    FogUpdate2();
    PacketHeader header;
    float x;
    float y;
    DWORD radius;
    BYTE unk1;
};

struct HeroSpawn {
    HeroSpawn(DWORD netId, DWORD playerNo, bool isBlue, bool isBot, BYTE location, DWORD skinId, char *szName, char *szType);
    PacketHeader header;
    DWORD netId;
    DWORD playerNo; //1-number of players
    BYTE unk1;
    BYTE unk2;
    BYTE isBlue;
    BYTE isBot;
    BYTE unk5;
    BYTE location;
    DWORD skinId;
    BYTE name[128];
    BYTE type[64];
} ;
struct InitPoint {
    WORD x;
    WORD y;
    BYTE _0;
    InitPoint();
    InitPoint(WORD x, WORD y);
    void SetPos(WORD x, WORD y);
};
struct InitSpawn {
    InitSpawn(DWORD netID);
    PacketHeader header;
    DWORD int1[9];
    DWORD int2[9];
    DWORD int3[4];
    DWORD int4[5];
    DWORD int5[3];
    DWORD unk1;
    DWORD unk2;
    InitPoint pts[17];
    BYTE unk5[315];
    WORD unk6;
};

struct HeroSpawn3 { //PKT_S2C_SpawnPos
    HeroSpawn3(DWORD netId, float fX, float fY, BYTE spawnIndex, float fAngle);
    PacketHeader header;
    WORD wFlag;
    BYTE unk1;
    BYTE unk2;
    BYTE unk3;
    BYTE unk4;
    BYTE unk5;
    BYTE unk6;
    BYTE unk7[5];
    float fUnk1;
    BYTE unk8[3];
    float fUnk2;
    BYTE unk9[6];
    BYTE bUnk1;
    DWORD dwUnk;
    float f1;
    float f2;
    float f3;
    float f4;
};
struct HeroSpawn4 { //PKT_S2C_SpawnPos
    HeroSpawn4(DWORD netId, float fX, float fY, BYTE spawnIndex, float fAngle);
    PacketHeader header;
    WORD wFlag;
    BYTE unk1;
    BYTE unk2;
    BYTE unk3;
    BYTE unk4;
    BYTE unk5;
    BYTE unk6;
    BYTE unk7[5];
    float fUnk1;
    BYTE unk8[3];
    float fUnk2;
    BYTE unk9[13];
    BYTE bUnk1;
    DWORD dwUnk;
    float f1;
    float f2;
    float f3;
    float f4;
};
struct SpawnHealthPoints { //PKT_S2C_SpawnHP
    SpawnHealthPoints(DWORD netID, float health, float maxHealth);

    PacketHeader header;
    WORD unk;
    float maxHealth;
    float health;
};
struct SetupSpell1 {
    SetupSpell1(DWORD netID, DWORD spellID);
    PacketHeader header;
    BYTE spellId;
    float unk;
    float f1;
};
struct SetupSpell2 {
    SetupSpell2(DWORD netID, DWORD spellID);
    PacketHeader header;
    BYTE unk1;
    WORD unk2;
    DWORD spellId;
    DWORD unk3;
    DWORD unk4; //-1
    DWORD unk5;
    DWORD unk6;
};


struct TurretSpawn2 {
    TurretSpawn2(DWORD netID);
    PacketHeader header;
    BYTE unk[13];
    float f1;
    BYTE unk2[13];
};

struct TurretSpawn {
    TurretSpawn();
    TurretSpawn(DWORD netID, const char *szName);

    PacketHeader header;
    BYTE tID;
    WORD unk;
    BYTE name[29];
    BYTE type[42];
};
struct GameTimer {
    GameTimer(float fTime);
    PacketHeader header;
    float fTime;
};
struct GameTimerUpdate {
    GameTimerUpdate(float fTime);
    PacketHeader header;
    float fTime;
};
struct VisionRange {
    VisionRange(DWORD netID, DWORD actorID, DWORD dwTeam, float x, float y, float fRange);
    PacketHeader header;
    DWORD dwTeam;
    DWORD dwUnk;
    DWORD dwUnk2; // -1
    DWORD netID;
    DWORD actorID;
    DWORD dwUnk3;
    float x;
    float y;
    float z;
    float xRange; //88
    float yRange; //130
    float zRange; // 1
    DWORD dwUnk4;
    DWORD dwUnk5; //7
    float fUnk; //800
};
class LevelPropSpawn {
    public:
        LevelPropSpawn();
        void SetProp(char *szName, char *szType, float fX, float fY, float fZ);
        PacketHeader header;
        DWORD netId;
        BYTE bFlag; //0x40
        DWORD dwUnk; //0
        float x;
        float y;
        float z;
        BYTE unk[24];
        float f1;
        float f2;
        float f3;
        DWORD dwUnk1;
        BYTE bPos;
        BYTE name[64];
        BYTE type[64];
};

struct SpellSetPacket {
    SpellSetPacket(DWORD netID, DWORD _spellID, DWORD _level);
    PacketHeader header;
    DWORD spellID;
    DWORD level;
};
struct SpellBook {
    explicit SpellBook(DWORD netID);
    PacketHeader header;
    BYTE bSpell[4];
    DWORD Sum1;
    DWORD Sum2;
    DWORD unk;
    float fZ; //1 or 25000
    DWORD netID;;
};

struct Announce {
    PacketHeader header;
    BYTE msg;
    DWORD64 unknown;
    DWORD mapNo;
};

typedef struct _SkillUpPacket {
    PacketHeader header;
    BYTE skill;
} SkillUpPacket;

typedef struct _SkillUpResponse {
    _SkillUpResponse(DWORD netID);
    PacketHeader header;
    BYTE skill;
    BYTE level; //?
    BYTE pointsLeft;

} SkillUpResponse;

typedef struct _BuyItemReq {
    PacketHeader header;
    DWORD id;
} BuyItemReq;

typedef struct _BuyItemAns {
    _BuyItemAns();
    _BuyItemAns(DWORD netID, int item, int slot, int stack);
    PacketHeader header;
    WORD itemId;
    WORD unk1;
    BYTE slotId;
    BYTE stack;
    WORD unk2;
} BuyItemAns;

typedef struct _SellItemReq {
    PacketHeader header;
    BYTE iSlot;
} SellItemReq;

typedef struct _SellItemAns {
    _SellItemAns(DWORD netID, int slot, int stack);
    PacketHeader header;
    BYTE slotId;
    BYTE stack;
} SellItemAns;

typedef struct _SwapItemReq {
    PacketHeader header;
    BYTE iSlot1;
    BYTE iSlot2;
} SwapItemReq;
typedef struct _SwapItemAns {
    _SwapItemAns(DWORD netID, int iSlot1, int iSlot2);
    PacketHeader header;
    BYTE iSlot1;
    BYTE iSlot2;
} SwapItemAns;

typedef struct _EmotionPacket {
    PacketHeader header;
    BYTE id;
} EmotionPacket;

typedef struct _EmotionResponse {
    _EmotionResponse();
    PacketHeader header;
    BYTE id;
} EmotionResponse;
typedef struct _PingCheckPacket {
    PacketHeader header;
    float fTime1;
    float fTime2;
} PingCheckPacket;

typedef struct _PingCheckResponse {
    _PingCheckResponse(float fTime1, float fTime2);
    PacketHeader header;
    float fTime1;
    float delta;
    float fTime2;
} PingCheckResponse;

typedef struct _UpdateModel {
    _UpdateModel(DWORD netID, const char *szModel);
    PacketHeader header;
    DWORD id;
    BYTE bOk;
    DWORD unk1;
    BYTE szName[32];
} UpdateModel;
typedef struct _RestoreModel {
    _RestoreModel(DWORD netID);
    PacketHeader header;
    DWORD id;
} RestoreModel;
typedef struct _EliseSpider3 {
    _EliseSpider3(DWORD netID, DWORD unk3);
    PacketHeader header;
    BYTE unk1;
    BYTE unk2;
    BYTE unk3;
} EliseSpider3;

typedef struct _SpawnParticle {
    _SpawnParticle();
    _SpawnParticle(DWORD dwHash, DWORD netID1, DWORD netID2, DWORD srcID);
    _SpawnParticle(const char *szParticle, DWORD netID1, DWORD netID2, DWORD srcID);
    void SetPos(float fX, float fY);
    WORD flags; //0x66
    DWORD objHash; //0
    DWORD netId1; //4
    byte unk6; //8
    float unk7; //9
    DWORD srcId1; //13
    DWORD srcId2; //17
    //..
    DWORD unk8; //some hash related to unit model maybe? //21
    DWORD netId2;//another object? maybe actor? //25
    //..
    float toX;
    float toY;
    float toZ;
    //..
    float toX2;
    float toY2;
    float toZ2;
} SpawnParticle;

typedef struct _ReleaseMissilePacket {
    _ReleaseMissilePacket(DWORD netID, DWORD objID);
    PacketHeader header;
    DWORD objId;
} ReleaseMissilePacket;


typedef struct _DisplaySpellPacket {
    _DisplaySpellPacket(DWORD dwHash, DWORD srcID, DWORD objId1, DWORD objId2);
    PacketHeader header;
    DWORD unk1;
    byte unk2;
    SpawnParticle spawn;
    BYTE unk11;
    float fCastTime; //0.25
    float fDelay; //0
    float fVisible; //1.0
    float fCooldown; //3.5
    DWORD unk15;
    WORD wFlags;
    //.. bounding box?
    float fMana;
    float fFromX;
    float fFromY;
    float fFromZ;
    //..
    DWORD unk17;
    DWORD unk18;
} DisplaySpellPacket;
typedef struct _AnimateSpellPacket {
    _AnimateSpellPacket(DWORD netID, DWORD spellID, char *szSpell);
    PacketHeader header;
    DWORD spellId;
    float f1;
    DWORD unk2;
    char szSpell[32];
} AnimateSpellPacket;
typedef struct _SetSpellPacket {
    _SetSpellPacket(DWORD netID, DWORD slotId, char *szSpell);
    PacketHeader header;
    BYTE slotId;
    BYTE bUnk;
    char szSpell[32];
} SetSpellPacket;

typedef struct _AbortAnimationPacket {
    _AbortAnimationPacket(DWORD srcId);
    PacketHeader header;
    DWORD unk1;
    BYTE unk2;
} AbortAnimationPacket;
typedef struct _AggroPacket {
    _AggroPacket(DWORD srcId, DWORD targetId);
    PacketHeader header;
    DWORD targetId;
} AggroPacket;
typedef struct _MeleeAttackPacket {
    _MeleeAttackPacket(DWORD srcId, DWORD targetId, DWORD projectileId, BYTE bAnimation, float fX, float fY);
    PacketHeader header;
    DWORD targetId;
    uint8 bUnk;
    DWORD projectileId;
    uint8 spellId;
    float fX;
    float fY;
} MeleeAttackPacket;
typedef struct _AttackProjectilePacket {
    _AttackProjectilePacket(DWORD srcId, DWORD targetId, DWORD projectileId, BYTE bAnimation, float fX, float fY);
    PacketHeader header;
    DWORD targetId;
    uint8 bUnk;
    DWORD projectileId;
    uint8 spellId;
    float fX;
    float fY;
} AttackProjectilePacket;

typedef struct _UpdateDrawPos {
    _UpdateDrawPos(DWORD netID, float fX, float fY, BYTE bUnk, DWORD netId2);
    PacketHeader header;
    BYTE unk1; //bHeader (0x10F)
    BYTE unk2; //not used?
    BYTE unk3; //01 for skill, 01 for champion?
    float fX;
    float fY;
    float fZ;
    DWORD netId;
} UpdateDrawPos;
typedef struct _UpdateDash {
    _UpdateDash(DWORD netID, float fX, float fY, float fZ, float fRotation);
    PacketHeader header;
    float f1;
    float f2;
    float f3;
    BYTE unk;
    float f4;
} UpdateDash;
typedef struct _ApplyBuffPacket {
    _ApplyBuffPacket(DWORD netID, BYTE bSlot, BYTE bType, BYTE nStacks, BYTE bVisible, DWORD dwHash1, DWORD dwHash2, float fTime);
    PacketHeader header;
    BYTE bSlot;
    BYTE bType;
    BYTE nStacks;
    BYTE bVisible;
    DWORD buffID;
    DWORD targetID;
    DWORD unk;
    float fTime;
    DWORD srcID;
} ApplyBuffPacket;

typedef struct _RemoveBuffPacket {
    _RemoveBuffPacket(DWORD netID, BYTE bSlot, DWORD buffHash);
    PacketHeader header;
    BYTE bSlot;
    DWORD buffHash;
    float fUnk;
} RemoveBuffPacket;

typedef struct _CastSpellPacket {
    PacketHeader header;
    BYTE spellId;
    float xFrom;
    float yFrom;
    float xTo;
    float yTo;
    DWORD dwTargetId;
} CastSpellPacket;

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif