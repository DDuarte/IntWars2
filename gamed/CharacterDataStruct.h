#pragma once
#include "stdafx.h"
#include "IniBin.h"

struct ScaleStat {
    float fBase;
    float fPerLevel;
    float GetStat(int iLevel) {
        return fBase + (iLevel * fPerLevel);
    }
};
struct AttackDataStruct {
    BYTE spellSlot;
    std::string name;
    float Probability;
    float AttackDelayCastOffsetPercentAttackSpeedRatio;
    float AttackDelayCastOffsetPercent;
    float AttackDelayOffsetPercent;
    float AttackTotalTime;
    float AttackCastTime;
};
class CharacterDataStruct {
    public:
        std::string sCharacter;
        bool IsMelee;

        ScaleStat hp;
        ScaleStat mp;
        ScaleStat hpRegen;
        ScaleStat mpRegen;

        //..
        float BaseStaticHPRegen; //default 1.0
        float BaseFactorHPRegen; //default 0.0
        float BaseStaticMPRegen; //default 1.0
        float BaseFactorMPRegen; //default 0.0
        //..
        ScaleStat damage;
        ScaleStat abilityDamage;
        ScaleStat armor;
        ScaleStat spellBlock;
        //dodge
        //misschance
        ScaleStat critChance;
        //...
        float CritDamageBonus;
        float MoveSpeed;
        float AttackRange;
        float AttackAutoInterruptPercent;
        float AcquisitionRange;
        float TowerTargetingPriorityBoost;
        float DeathTime;
        //..
        AttackDataStruct BaseAttack;
        AttackDataStruct ExtraAttack1;
        AttackDataStruct ExtraAttack2;
        AttackDataStruct ExtraAttack3;
        AttackDataStruct ExtraAttack4;
        AttackDataStruct ExtraAttack5;
        AttackDataStruct ExtraAttack6;
        AttackDataStruct ExtraAttack7;
        AttackDataStruct ExtraAttack8;
        AttackDataStruct CritAttack;
        AttackDataStruct ExtraCritAttack1;
        AttackDataStruct ExtraCritAttack2;
        AttackDataStruct ExtraCritAttack3;
        AttackDataStruct ExtraCritAttack4;
        AttackDataStruct ExtraCritAttack5;
        AttackDataStruct ExtraCritAttack6;
        AttackDataStruct ExtraCritAttack7;
        AttackDataStruct ExtraCritAttack8;
        //..
        float AttackSpeedPerLevel; //* 0.009999999776482582; from LoL Client reverse
        float ExpGivenOnDeath; //Default 48.0
        float GoldGivenOnDeath; //Default 25.0
        float GoldRadius;
        float ExperienceRadius;
        float DeathEventListeningRadius; //Default 1000.0
        BYTE LocalGoldSplitWithLastHitter;
        float LocalGoldGivenOnDeath;
        float LocalExpGivenOnDeath;
        float GlobalGoldGivenOnDeath;
        float GlobalExpGivenOnDeath;
        float Significance;
        float PerceptionBubbleRadius; //1350.0 default

        //..
        std::string Spell1;
        std::string Spell2;
        std::string Spell3;
        std::string Spell4;
        std::string ExtraSpell1;
        std::string ExtraSpell2;
        std::string ExtraSpell3;
        std::string ExtraSpell4;
        std::string ExtraSpell5;
        std::string ExtraSpell6;
        std::string ExtraSpell7;
        std::string ExtraSpell8;
        std::string ExtraSpell9;
        std::string ExtraSpell10;
        std::string ExtraSpell11;
        std::string ExtraSpell12;
        std::string ExtraSpell13;
        std::string ExtraSpell14;
        std::string ExtraSpell15;
        std::string ExtraSpell16;
        std::string CriticalAttack;
        //..
        std::string Passive1Name;
        std::string Passive1LuaName;
        std::string Passive1Desc;
        std::string PassLev1Desc1;
        std::string PassiveSpell;
        std::string Passive1Icon;
        //..
        float GameplayCollisionRadius;
        AttackDataStruct *GetRandomAttackAnim();
        AttackDataStruct *GetRandomCritAnim();
        static CharacterDataStruct *GetCharacterStruct(std::string sName);
    private:
        CharacterDataStruct(std::string sName, CIniBin *bin);
};