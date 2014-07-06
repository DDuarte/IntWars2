#include "stdafx.h"
#include "CharacterDataStruct.h"
#include "RafDB.h"
std::vector<CharacterDataStruct *> vCharacters;


//#define GET_BIN(x,y) ((float)bin->GetVar("Data",x) ? (float)bin->GetVar("Data",x) : y)
#define LOAD_SCALE(x,y,z) x = { bin->GetVar("Data", y), bin->GetVar("Data", z) }
#define LOAD_FLOAT(x) x = bin->GetVar("Data", #x);
#define LOAD_FLOAT2(x,y) x = bin->GetVar("Data", #x); x = x ? x : y;
#define LOAD_STRING(x) x = bin->GetVar("Data", #x);


float GET_BIN(CIniBin *bin, char *szVariable, float defaultValue) {
    float fValue = bin->GetVar("Data", szVariable);
    return fValue ? fValue : defaultValue;
}

#define READ_ATTACK(iSlot, x) x = { iSlot, bin->GetVar("Data", #x), \
                                    GET_BIN(bin, #x"_Probability", 2.0), \
                                    GET_BIN(bin, #x"_AttackDelayCastOffsetPercentAttackSpeedRatio", BaseAttack.AttackDelayCastOffsetPercentAttackSpeedRatio), \
                                    GET_BIN(bin, #x"_AttackDelayCastOffsetPercent", BaseAttack.AttackDelayCastOffsetPercent), \
                                    GET_BIN(bin, #x"_AttackDelayOffsetPercent", BaseAttack.AttackDelayOffsetPercent), \
                                    GET_BIN(bin, #x"_AttackTotalTime", 0.0), \
                                    GET_BIN(bin, #x"_AttackCastTime", 0.0)};



CharacterDataStruct::CharacterDataStruct(std::string sName, CIniBin *bin) {
    char szBaseAttack[64];
    sprintf_s(szBaseAttack, 64, "%sBasicAttack", sName.c_str());
    sCharacter = sName;
    LOAD_SCALE(hp, "BaseHP", "HPPerLevel");
    LOAD_SCALE(mp, "BaseMP", "MPPerLevel");
    LOAD_SCALE(hpRegen, "BaseStaticHPRegen", "HPRegenPerLevel");
    LOAD_SCALE(mpRegen, "BaseStaticMPRegen", "MPRegenPerLevel");
    LOAD_SCALE(armor, "Armor", "ArmorPerLevel");
    LOAD_SCALE(spellBlock, "SpellBlock", "SpellBlockPerLevel");
    LOAD_SCALE(damage, "BaseDamage", "DamagePerLevel");
    LOAD_SCALE(abilityDamage, "BaseAbilityPower", "AbilityPowerIncPerLevel");
    LOAD_SCALE(critChance, "BaseCritChance", "CritPerLevel");
    //.. load attacks here
    BaseAttack = { 0x40, szBaseAttack,
                   GET_BIN(bin, "BaseAttack_Probability", 1.0),
                   GET_BIN(bin, "AttackDelayCastOffsetPercentAttackSpeedRatio", 1.0),
                   GET_BIN(bin, "AttackDelayCastOffsetPercent", 0.0),
                   GET_BIN(bin, "AttackDelayOffsetPercent", 0.0),
                   GET_BIN(bin, "AttackTotalTime", 0.0),
                   GET_BIN(bin, "AttackCastTime", 0.0),
                 };
    //check if castTime 0, do some LoL Client RE
    //load other attacks here...
    READ_ATTACK(0x41, ExtraAttack1);
    READ_ATTACK(0x42, ExtraAttack2);
    READ_ATTACK(0x43, ExtraAttack3);
    READ_ATTACK(0x44, ExtraAttack4);
    READ_ATTACK(0x45, ExtraAttack5);
    READ_ATTACK(0x46, ExtraAttack6);
    READ_ATTACK(0x47, ExtraAttack7);
    READ_ATTACK(0x48, ExtraAttack8);
    READ_ATTACK(0x49, CritAttack);
    READ_ATTACK(0x50, ExtraCritAttack1);
    READ_ATTACK(0x51, ExtraCritAttack2);
    READ_ATTACK(0x52, ExtraCritAttack3);
    READ_ATTACK(0x53, ExtraCritAttack4);
    READ_ATTACK(0x54, ExtraCritAttack5);
    READ_ATTACK(0x55, ExtraCritAttack6);
    READ_ATTACK(0x56, ExtraCritAttack7);
    READ_ATTACK(0x57, ExtraCritAttack8);
    //check if castTime 0, do some LoL Client RE
    //load other attacks here...
    //
    LOAD_FLOAT(BaseStaticHPRegen);
    LOAD_FLOAT(BaseFactorHPRegen);
    LOAD_FLOAT(BaseStaticMPRegen);
    LOAD_FLOAT(BaseFactorMPRegen);
    //
    LOAD_FLOAT(CritDamageBonus);
    LOAD_FLOAT(MoveSpeed);
    LOAD_FLOAT(AttackRange);
    LOAD_FLOAT(AttackAutoInterruptPercent);
    LOAD_FLOAT(AcquisitionRange);
    LOAD_FLOAT(TowerTargetingPriorityBoost);
    LOAD_FLOAT(DeathTime);
    //..
    LOAD_FLOAT(AttackSpeedPerLevel); //* 0.009999999776482582; from LoL Client reverse
    LOAD_FLOAT2(ExpGivenOnDeath, 48.0);
    LOAD_FLOAT2(GoldGivenOnDeath, 25.0);
    LOAD_FLOAT(GoldRadius);
    LOAD_FLOAT(ExperienceRadius);
    LOAD_FLOAT2(DeathEventListeningRadius, 1000.0);
    //LOAD_BOOL(LocalGoldSplitWithLastHitter);
    LOAD_FLOAT(LocalGoldGivenOnDeath);
    LOAD_FLOAT(LocalExpGivenOnDeath);
    LOAD_FLOAT(GlobalGoldGivenOnDeath);
    LOAD_FLOAT(GlobalExpGivenOnDeath);
    LOAD_FLOAT(Significance);
    LOAD_FLOAT2(PerceptionBubbleRadius, 1350.0);
    //..
    LOAD_STRING(Spell1);
    LOAD_STRING(Spell2);
    LOAD_STRING(Spell3);
    LOAD_STRING(Spell4);
    LOAD_STRING(ExtraSpell1);
    LOAD_STRING(ExtraSpell2);
    LOAD_STRING(ExtraSpell3);
    LOAD_STRING(ExtraSpell4);
    LOAD_STRING(ExtraSpell5);
    LOAD_STRING(ExtraSpell6);
    LOAD_STRING(ExtraSpell7);
    LOAD_STRING(ExtraSpell8);
    LOAD_STRING(ExtraSpell9);
    LOAD_STRING(ExtraSpell10);
    LOAD_STRING(ExtraSpell11);
    LOAD_STRING(ExtraSpell12);
    LOAD_STRING(ExtraSpell13);
    LOAD_STRING(ExtraSpell14);
    LOAD_STRING(ExtraSpell15);
    LOAD_STRING(ExtraSpell16);
    LOAD_STRING(CriticalAttack);
    //..
    LOAD_STRING(Passive1Name);
    LOAD_STRING(Passive1LuaName);
    LOAD_STRING(Passive1Desc);
    LOAD_STRING(PassLev1Desc1);
    LOAD_STRING(PassiveSpell);
    LOAD_STRING(Passive1Icon);
    //
    LOAD_FLOAT(GameplayCollisionRadius);
    IsMelee = bin->GetVar("Data", "IsMelee").ToString() == "Yes";
    vCharacters.push_back(this);
}

AttackDataStruct *CharacterDataStruct::GetRandomAttackAnim() {
    AttackDataStruct *rAttack = &BaseAttack;
    AttackDataStruct *vAttacks[8] = {
        &ExtraAttack1,
        &ExtraAttack2,
        &ExtraAttack3,
        &ExtraAttack4,
        &ExtraAttack5,
        &ExtraAttack6,
        &ExtraAttack7,
        &ExtraAttack8,
    };
    float fRand = (float)(rand() % 1001) / 1000;
    float fLastProb = rAttack->Probability;
    for(UINT i = 0; i < sizeof(vAttacks) / 4; i++) {
        if(vAttacks[i]->Probability == 2.0f) {
            continue;
        }
        float fNewProb = fLastProb + vAttacks[i]->Probability;
        if(fLastProb < fRand && fRand <= fNewProb) {
            rAttack = vAttacks[i];
        }
        fLastProb = fNewProb;
    }
    //printf("#r: %f / %02X\r\n#1: %f\r\n#2: %f\r\n#3: %f\r\n", fRand, (BYTE)rAttack->spellSlot, BaseAttack.Probability, ExtraAttack1.Probability, ExtraAttack2.Probability);
    return rAttack;
}
AttackDataStruct *CharacterDataStruct::GetRandomCritAnim() {
    AttackDataStruct *rAttack = &CritAttack;
    AttackDataStruct *vAttacks[8] = {
        &ExtraCritAttack1,
        &ExtraCritAttack2,
        &ExtraCritAttack3,
        &ExtraCritAttack4,
        &ExtraCritAttack5,
        &ExtraCritAttack6,
        &ExtraCritAttack7,
        &ExtraCritAttack8,
    };
    float fRand = (float)(rand() % 1001) / 1000;
    float fLastProb = rAttack->Probability;
    for(UINT i = 0; i < sizeof(vAttacks) / 4; i++) {
        if(vAttacks[i]->Probability == 2.0f) {
            continue;
        }
        float fNewProb = fLastProb + vAttacks[i]->Probability;
        if(fLastProb < fRand && fRand <= fNewProb) {
            rAttack = vAttacks[i];
        }
        fLastProb = fNewProb;
    }
    return rAttack;
}


CharacterDataStruct *CharacterDataStruct::GetCharacterStruct(std::string szCharacter) {
    for(UINT i = 0; i < vCharacters.size(); i++) {
        if(vCharacters[i]->sCharacter == szCharacter) {
            return vCharacters[i];
        }
    }
    CIniBin bin = RafDB::GetCharacter((char *)szCharacter.c_str());
    if(bin) {
        return new CharacterDataStruct(szCharacter, &bin);
    }
    return NULL;
}