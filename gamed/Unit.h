#pragma once
#include "IniBin.h"
#define NWID_BASE 0x40000000
#define VALID_NWID(x) ((NWID_BASE & x) == NWID_BASE)

enum Team {
    BLUE = 100,
    RED = 200,
    NEUTRAL = 300,
    TEAM_COUNT = 4 //MAX_TEAM_ARRAY
};

class CObject {
    public:
        WORD index;
        float x;
        float y;
        float z;
        CObject();
        bool operator==(const CObject &cmp);
        CObject(WORD index, float x, float y);
        DWORD GetNetworkID();
        virtual void SetPos(float fX, float fY, float fZ = 0);
};

class CMovable : public CObject {
        typedef std::chrono::high_resolution_clock Time;
    public:
        virtual ~CMovable();
        CMovable();
        Vector2f GetPos();
        float GetRotation();
        void SetWaypoints(std::vector<Vector2f> waypoints);
        std::vector<Vector2f> GetWaypoints();
        void SetPos(float x, float y, float z) override;
        void RotateTo(Vector2f fTarget);
        float GetSpeed();
        void SetSpeed(float speed);
        void Pause();
        virtual void Stop();
        void Resume();
        float GetRotationSpeed();
        void SetRotationSpeed(float rotationSpeed);
        float GetDistanceOf(std::vector<Vector2f> points);
        std::vector<Vector2f> GetAllWaypoints();
    private:
        double GetTimeDelta();
        double GetRotationTimeDelta();
        Time::time_point lastUpdateTime;
        Time::time_point lastRotationUpdateTime;
        std::vector<Vector2f> waypoints;
        float mMoveSpeed = 330;
        float rotationSpeed = 220;
        float initialRotation = 0;
        float targetRotation = 270;
        bool bPause = false;
};

class CUnit : public CMovable {
    public:
        float GetX();
        float GetY();
        char szName[32];
        char szType[32];
        char szModel[32];
        Team tTeam;
        DWORD mSkinId;

        float mGold;
        float mGoldTotal;

        DWORD mCanCastBits1;
        DWORD mCanCastBits2;

        DWORD mEvolvePoints;
        DWORD mEvolveFlag;


        float ManaCost_0;
        float ManaCost_1;
        float ManaCost_2;
        float ManaCost_3;
        float ManaCost_Ex0;
        float ManaCost_Ex1;
        float ManaCost_Ex2;
        float ManaCost_Ex3;
        float ManaCost_Ex4;
        float ManaCost_Ex5;
        float ManaCost_Ex6;
        float ManaCost_Ex7;
        float ManaCost_Ex8;
        float ManaCost_Ex9;
        float ManaCost_Ex10;
        float ManaCost_Ex11;
        float ManaCost_Ex12;
        float ManaCost_Ex13;
        float ManaCost_Ex14;
        float ManaCost_Ex15;

        DWORD ActionState;

        DWORD MagicImmune;
        DWORD IsInvulnerable;
        DWORD IsPhysicalImmune;
        DWORD IsLifestealImmune;

        float mBaseAttackDamage;
        float mBaseAbilityDamage;

        float mDodge;
        float mCrit;
        float mCrit2;

        float mArmor;
        float mSpellBlock;

        float mHPRegenRate;
        float mPARRegenRate;

        float mAttackRange;

        float mFlatPhysicalDamageMod;
        float mPercentPhysicalDamageMod;

        float mFlatMagicDamageMod;
        float mPercentMagicDamageMod;

        float mFlatMagicReduction;
        float mPercentMagicReduction;

        float mAttackSpeedMod;

        float mFlatCastRangeMod;

        float mPercentCooldownMod;
        float mPassiveCooldownEndTime;
        float mPassiveCooldownTotalTime;

        float mFlatArmorPenetration;
        float mPercentArmorPenetration;

        float mFlatMagicPenetration;
        float mPercentMagicPenetration;

        float mPercentLifeStealMod;
        float mPercentSpellVampMod;

        float mPercentCCReduction;

        float mHP;
        float mMaxHP;
        float mMP;
        float mMaxMP;

        float mExp;

        float mLifetime;
        float mMaxLifetime;
        float mLifetimeTicks;

        float mFlatBubbleRadiusMod;
        float mPercentBubbleRadiusMod;

        float mPathfindingRadiusMod;

        DWORD mLevelRef;

        DWORD mNumNeutralMinionsKilled;

        DWORD mIsTargetable;
        DWORD mIsTargetableToTeamFlags;

        CUnit *actor;
        CUnit();
        CUnit(WORD index, char *szName, char *szType = NULL, Team tTeam = NEUTRAL, float x = 0, float y = 0);
        CUnit *GetActor(WORD wIndex = 0);
        void SetName(const char *szName);
        bool SetType(const char *szType);
        bool SetModel(const char *szModel);

        const char *GetName();
        const char *GetType();
        const char *GetModel();


        CIniBin GetIniBin();
        std::string GetVar(const char *szSection, const char *szVar);

        void moveTo(Vector2f pos);
        void moveTo(Vector2f pos, Vector2f target, float range);

};