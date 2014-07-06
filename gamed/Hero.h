#pragma once
#include "Particle.h"
#include "CharacterDataStruct.h"
#include "Unit.h"
#include <enet/enet.h>
class CHero;
class WorkItem;

class CItemSlot {
    public:
        DWORD ID;
        DWORD nStacks;
        float fCooldown;
        BYTE iSlot;
        CItemSlot();
        CItemSlot(DWORD itemId);
        operator int();
        void Swap(CItemSlot *s2);
};

class CInventory { //CInventoryManager
    public:
        CItemSlot vSlots[7];
        CInventory();
        CItemSlot *GetTrinket();
        CItemSlot *GetSlot(DWORD iIndex);
        CItemSlot *GetItemById(DWORD itemId);
        CItemSlot *GetFreeSlot();

        int GetItemCount();
};

class CBuffSlot {
    public:
        char szBuff[64];
        DWORD dwHash;
        BYTE bType;
        DWORD nStacks;
        BYTE bVisible;
        float fStart;
        float fEnd;
        BYTE iSlot;
        CBuffSlot();
        CBuffSlot(BYTE slot);
        bool IsFree();
        void SetFree();
        void Renew(float fTime);
        bool IsExpired();
        void SetBuff(BYTE type, BYTE fVisible, DWORD dwHash, float fTime, UINT nStacks = 1);
};

#define BUFF_SLOTS 32
class CBuffManager {
    public:
        CHero *p;
        CBuffSlot vSlots[BUFF_SLOTS];
        CBuffManager() {
            ZeroMemory(this, sizeof(CBuffManager));
        }
        CBuffManager(CHero *p) : CBuffManager() {
            this->p = p;
            for(UINT i = 0; i < BUFF_SLOTS; i++) {
                vSlots[i] = CBuffSlot(i);
            }
        }
        //void ApplyBuff(const char *szBuff, int tType, float fTime) {
        //find buff..
        //}
        CBuffSlot *GetFreeSlot(const char *szBuff = NULL);
        void ExpireBuff(CBuffSlot *slot);
};

#define SPELL_VAR(X,Y) X Get##Y() { return GetSpell().GetVar("SpellData", #Y); }
#define SPELL_VAR2(X,Y,Z) X Z() { return GetSpell().GetVar("SpellData", #Y); }
#define SPELL_VAR_L(X,Y) X Get##Y() { char szVar[64]; sprintf_s(szVar, 64, #Y"%i", iLevel); return GetSpell().GetVar("SpellData", szVar); }
#define SPELL_VAR_L2(X,Y,Z) X Z() { char szVar[64]; sprintf_s(szVar, 64, #Y"%i", iLevel); return GetSpell().GetVar("SpellData", szVar); }
class CSpell {
    public:
        CSpell() {
            owner = nullptr;
            iIndex = 0;
            iLevel = 0;
        }
        CSpell(CHero *hero, UINT index, UINT level) : CSpell() {
            owner = hero;
            iIndex = index;
            iLevel = level;
        }
        CHero *owner;
        UINT iIndex;
        UINT iLevel;
        std::string GetName();
        CIniBin GetSpell();

        SPELL_VAR_L(float, FloatVarsDecimals);
        SPELL_VAR_L2(float, CastRange, GetCastRangeL);
        SPELL_VAR_L2(float, ManaCost, GetManaCostL);
        SPELL_VAR_L2(float, Cooldown, GetCooldownL);
        SPELL_VAR_L(float, ChannelDuration);
        SPELL_VAR_L(float, LocationTargettingWidth);
        SPELL_VAR_L(float, LocationTargettingLength);
        SPELL_VAR(int, CastType);
        SPELL_VAR(float, LineMissileTargetHeightAugment);
        SPELL_VAR(float, BounceRadius);
        SPELL_VAR(float, CastRadius);
        SPELL_VAR(int, HaveHitBone);
        SPELL_VAR(int, IsDisabledWhileDead);
        SPELL_VAR(float, AILifetime);
        SPELL_VAR(float, AIRange);
        SPELL_VAR(float, AISpeed);
        SPELL_VAR(float, AIRadius);
        SPELL_VAR(DWORD, AIBlockLevel);
        SPELL_VAR(float, MissileSpeed);
        SPELL_VAR2(float, CastRange, GetCastRangeA); //Absolute?
        SPELL_VAR2(float, ManaCost, GetManaCostA); //Absolute?
        SPELL_VAR2(float, Cooldown, GetCooldownA); //Absolute?
        SPELL_VAR(std::string, AnimationName);
        SPELL_VAR(float, DelayTotalTimePercent);
        SPELL_VAR(float, DelayCastOffsetPercent);
        SPELL_VAR(float, OverrideCastTime);
        SPELL_VAR(float, SpellRevealsChampion);
        SPELL_VAR(float, CastConeAngle); //Make hero turn to this min angle?
        SPELL_VAR(int, CantCancelWhileWindingUp);
        float GetCastTime() {
            float fCastTime = GetOverrideCastTime();
            if(fCastTime == 0) {
                fCastTime = (1 + GetDelayTotalTimePercent()) * (1 + GetDelayCastOffsetPercent());
            }
            return fCastTime;
        }
        float GetCastRange() {
            float fCastRange = GetCastRangeA();
            if(fCastRange == 0) {
                fCastRange = GetCastRangeL();
            }
            return fCastRange;
        }
        float GetCooldown() {
            float fCooldown = GetCooldownA();
            if(fCooldown == 0) {
                fCooldown = GetCooldownL();
            }
            return fCooldown;
        }
        float GetManaCost() {
            float fManaCost = GetManaCostA();
            if(fManaCost == 0) {
                fManaCost = GetManaCostL();
            }
            return fManaCost;
        }
};

struct CBuffWork {
    BYTE bType;
    std::string sBuff;
    float fTime;
    int nStacks;
};
struct SpellSet {
    BYTE iSlot;
    std::string spellName;
    float fTime;
};
struct ExtraSpell {
    BYTE iSpellSlot;
    std::string sSpell;
    float fTime;
};
struct Teleport {
    float fX;
    float fY;
    float fTime;
};

class CSpellWork {
    public:
        CHero *owner;
        DWORD iSpell;
        std::string spellName;
        WorkItem *castWork;

        DWORD netId;
        WORD wFlag;
        float fX;
        float fY;
        Teleport tp;

        int iUpdatePos; //0,1 or2
        bool bCanCancel;
        bool bSetFacing; //?
        bool bAnimation; //use animation or not
        bool bStop; //stop while casting?

        float fReleaseCast;
        float fCastFrame;
        float fDelay;
        float fVisible;
        float fCooldown;
        float fManaCost;

        std::vector<CParticle *> particles;
        std::vector<CParticle *> missiles;
        std::vector<CBuffWork> buffs;
        std::vector<ExtraSpell> spells;
        std::vector<SpellSet > spellSets;
        /*CSpellWork(DWORD iSpell, const char *szSpell, float fX, float fY) : CSpellWork() {
        this->iSpell = iSpell;
        spellName = std::string(szSpell);
        this->fX = fX;
        this->fY = fY;
        animation = std::string();
        }*/
        CSpellWork();
        CSpellWork(CHero *hero, const char *szSpell, DWORD iSpell);
        void Cancel();

        CSpellWork(CHero *owner, DWORD iSpell);
        CSpellWork::CSpellWork(CHero *owner, DWORD iSpell, float fX, float fY);
        void SetPosition(float fX, float fY);
        CParticle *AddParticle(CHero *src, const char *szParticle, float fTimer);
        CParticle *AddTargetParticle(CHero *target, CHero *src, const char *szParticle, float fTimer);
        CParticle *AddMissile(CHero *src, const char *szSpell, float fTimer);
        void AddSpell(BYTE iSlot, const char *szSpell, float fTime);
        void AddBuff(BYTE bType, const char *szBuff, float fTime, int nStacks);
        void AddSpellSet(BYTE iSlot, const char *szSpell, float fTime);
        void SetTeleport(float fX, float fY, float fTimer);
        bool DoCast();
        bool DoWork();
        CSpell *GetSpell();
};
class CHero : public CUnit { //obj_AI_Hero
    public:
        int _peer;
        CharacterDataStruct *charData;
        std::vector<CSpellWork> vSpells;
        UINT playerIndex;
        UINT dwKeyUnk;
        UINT userId;
        BYTE spawnIndex;
        float spawnX;
        float spawnY;
        //..
        WorkItem *aaWork;
        CHero *aaTarget;
        //Spells
        UINT iSpellPoints;
        CSpell spells[4];
        //Inventory
        CInventory inv;
        //Buffs
        CBuffManager buffManager;
        //State Flags
        BOOL bCanAttack;
        BOOL bCanCast;
        BOOL bCanMove;

        WorkItem *attackLockWork;
        WorkItem *castLockWork;
        WorkItem *moveLockWork;
        CHero(WORD index, char *szName, char *szHero, Team tTeam, DWORD skinId);
        void RegenTick();
        ENetPeer *GetPeer();
        DWORD GetLevel();
        void SetLevel(DWORD iLevel);
        Team GetTeam();
        Team GetEnemyTeam();
        bool IsValidTarget(CHero *target);
        DWORD GetClosestAATargetId();
        void SetAATarget(CHero *target);
        bool DoAutoAttack();
        void QueueSpell(CSpellWork work);
        void PopSpell();
        void DoSpellCast();
        void DoSpellWork();
        void SetPos(Vector2f fPos);
        void SetSpawn(BYTE spawnIndex, Vector3f fPos);
        float GetBaseHP();
        float GetBaseMP();
        float GetBaseHPRegen();
        float GetBaseMPRegen();
        float GetBaseArmor();
        float GetBaseSpellBlock();
        float GetBaseDamage();
        float GetBaseAbilityDamage();
        float GetBaseCrit();
        float GetBaseAttackRange();
        float GetBaseMoveSpeed();

        void LockAttack(float fTimer = 0);
        void UnlockAttack();
        void LockCast(float fTimer = 0);
        void UnlockCast();
        void LockMove(float fTimer = 0);
        void UnlockMove();

        CBuffSlot *ApplyBuff(UINT iType, float fVisible, const char *szBuff, float fTime, UINT nStacks = 1);

        void CalculateStats();
        void SendHeroStats();
        void SpellInit();
        bool SetType(const char *szType);
        bool Morph(const char *szType);
        bool SetModel(const char *szModel);

        bool IsMorphed();
        unsigned long long GetUserID();
        CSpell *GetSpell(UINT iIndex);
        std::string GetSpellName(UINT iIndex);
        CIniBin GetIniBin();

        float GetAARange();
        float GetGameplayCollisionRadius();
        float GetAttackDelayCastOffsetPercent();
        float GetAttackDelayOffsetPercent();
        float GetAttackSpeedPerLevel();
        UINT GetHash();

        void Stop() override;
        void SendWaypoints();
        void HandleMoveRequest(Vector2f  waypoint, DWORD targetId = 0);
        void SetupSpell(BYTE b1);
        float GetDistance(CHero *h);
        float GetCollisionDistance(CHero *h);
        //
        UINT GetActionState();
        UINT GetSpellBits();
        bool LevelSkill(UINT spellIndex);
        bool BuyItem(UINT itemId);
        bool SellItem(UINT iSlot);
        bool SwapItem(UINT iSlot1, UINT iSlot2);
        UINT CastSpell(UINT iIndex, float fX, float fY); //Sends spell display
        DWORD _CastSpell(DWORD iIndex, float fX, float fY);
        bool CallSpell(UINT iIndex, float fX, float fY); //Calls lua handler

        //AddStats
        void AddGold(float fGold);
        void AddHP(float fAdd);
        void AddMP(float fAdd);
        float GetCriticalChance();
        //SetStats
        void SetGold(float fGold);
        void SetCanCastBits1(DWORD dwCanCastBits1);
        void SetCanCastBits2(DWORD dwCanCastBits2);
        void SetEvolvePoints(DWORD dwEvolvePoints);
        void SetEvolveFlag(DWORD dwEvolveFlag);
        void SetManaCost0(float fCost);
        void SetManaCost1(float fCost);
        void SetManaCost2(float fCost);
        void SetManaCost3(float fCost);
        //ManaCost_0 to ManaCost_Ex15
        void SetActionState(DWORD dwActionState);
        void SetMagicImmune(DWORD bMagicImmune);
        void SetIsInvulnerable(DWORD bIsInvulnerable);
        void SetIsPhysicalImmune(DWORD bIsPhysicalImmune);
        void SetIsLifestealImmune(DWORD bIsLifeStealImmune);
        void SetBaseAttackDamage(float fBaseAttackDamage);
        void SetBaseAbilityDamage(float fBaseAbilityDamage);
        void SetDodge(float fDodge);
        void SetCrit(float fCrit);
        void SetArmor(float fArmor);
        void SetSpellBlock(float fSpellBlock);
        void SetHPRegenRate(float fHPRegenRate);
        void SetPARRegenRate(float fPARRegenRate);
        void SetAttackRange(float fAttackRange);
        void SetAttackSpeedMod(float fAttackSpeedMod);
        void SetFlatPhysicalDamageMod(float fFlatPhysicalDamageMod);
        void SetPercentPhysicalDamageMod(float fPercentPhysicalDamageMod);
        void SetFlatMagicDamageMod(float fFlatMagicDamageMod);
        void SetFlatMagicReduction(float fFlatMagicReduction);
        void SetPercentMagicReduction(float fPercentMagicReduction);
        void SetFlatCastRangeMod(float fFlatCastRangeMod);
        void SetPercentCooldownMod(float fPercentCooldownMod);
        void SetPassiveCooldownEndTime(float fPassiveCooldownEndTime);
        void SetPassiveCooldownTotalTime(float fPassiveCooldownTotalTime);
        void SetFlatArmorPenetration(float fFlatArmorPenetration);
        void SetPercentArmorPenetration(float fPercentArmorPenetration);
        void SetFlatMagicPenetration(float fFlatMagicPenetration);
        void SetPercentMagicPenetration(float fPercentMagicPenetration);
        void SetPercentLifeStealMod(float fPercentLifeStealMod);
        void SetPercentSpellVampMod(float fPercentSpellVampMod);
        void SetPercentCCReduction(float fPercentCCReduction);
        void SetHP(float fHP);
        void SetMP(float fMP);
        void SetMaxHP(float fMaxHP);
        void SetMaxMP(float fMaxMP);
        void SetCrit2(float fCrit2);
        void SetExp(float fExp);
        void SetLifetime(float fLifetime);
        void SetMaxLifetime(float fMaxLifetime);
        void SetLifetimeTicks(float fLifetimeTicks);
        void SetPathfindingRadiusMod(float mPathfindingRadiusMod);
        void SetLevelRef(DWORD fLevelRef);
        void SetNumNeutralMinionsKilled(DWORD fNumNeutralMinionsKilled);
        void SetFlatBubbleRadiusMod(float fFlatBubbleRadiusMod);
        void SetPercentBubbleRadiusMod(float fPercentBubbleRadiusMod);
        void SetMoveSpeed(float fMoveSpeed);
        void SetIsTargetable(DWORD bIsTargetable);
        void SetIsTargetableToTeamFlags(DWORD dwIsTargetableToTeamFlags);
};
