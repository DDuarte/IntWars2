#pragma once
/************************************************************************/
/* function log2(n)
  return math.log(n) / math.log(2)
  end

  function printTable(t)
  for map, mapinfo in pairs(t) do
  print("namespace " .. map:gsub(" ", "_"):gsub("'", "") .. " {")
  for objectType, attributeBlock in pairs(mapinfo) do
  print("\tnamespace " .. objectType .. " {")
  for mask, attributes in pairs(attributeBlock) do
  mask = log2(attributes[1])
  attributes = attributes[2]
  for index, attribute in pairs(attributes) do
  local valueType = attribute[2] == 0
  and "ObjectAttributeType::INTEGER_VALUE"
  or "ObjectAttributeType::FLOAT_VALUE"


  print("\t\tconst ObjectAttributeIndex " .. attribute[1] .. " = { " .. mask .. ", " .. (index - 1) .. ", " .. valueType .. "};")
  end
  end
  print("\t}")
  end
  print("}")
  end
  end                                                                     */
/************************************************************************/

enum ObjectAttributeType {
    FLOAT_VALUE,
    INTEGER_VALUE,
};

struct ObjectAttributeIndex {
    unsigned char major; //0 - 8
    unsigned char minor; //0 - 32
    ObjectAttributeType type;
    unsigned char getMajorMask();
    unsigned int getMinorMask();
    bool operator==(const ObjectAttributeIndex &other) const;
};

namespace std {
    template <> struct hash < ObjectAttributeIndex > {
        std::size_t operator()(const ObjectAttributeIndex &k) const {
            return ((std::hash<unsigned char>()(k.major)
                     ^ (std::hash<unsigned char>()(k.minor) << 1)) >> 1);
        }
    };
}

namespace The_Crystal_Scar {
    namespace obj_AI_Minion {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 1, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 1, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 7, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 8, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 9, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 10, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 5, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Hero {
        const ObjectAttributeIndex mGold = { 0, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mGoldTotal = { 0, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCanCastBits1 = { 0, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mCanCastBits2 = { 0, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolvePoints = { 0, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolveFlag = { 0, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex ManaCost_0 = { 0, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_1 = { 0, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_2 = { 0, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_3 = { 0, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex0 = { 0, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex1 = { 0, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex2 = { 0, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex3 = { 0, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex4 = { 0, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex5 = { 0, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex6 = { 0, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex7 = { 0, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex8 = { 0, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex9 = { 0, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex10 = { 0, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex11 = { 0, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex12 = { 0, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex13 = { 0, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex14 = { 0, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex15 = { 0, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mBaseAbilityDamage = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mDodge = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackRange = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatCastRangeMod = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCooldownMod = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownEndTime = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownTotalTime = { 1, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatArmorPenetration = { 1, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentArmorPenetration = { 1, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicPenetration = { 1, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicPenetration = { 1, 26, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentLifeStealMod = { 1, 27, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentSpellVampMod = { 1, 28, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCCReduction = { 1, 29, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHP = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mExp = { 3, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 3, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 3, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 3, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit2 = { 3, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPathfindingRadiusMod = { 3, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLevelRef = { 3, 13, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mNumNeutralMinionsKilled = { 3, 14, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 15, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 16, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Turret {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
}
namespace Howling_Abyss {
    namespace obj_BarracksDampener {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Hero {
        const ObjectAttributeIndex mGold = { 0, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mGoldTotal = { 0, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCanCastBits1 = { 0, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mCanCastBits2 = { 0, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolvePoints = { 0, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolveFlag = { 0, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex ManaCost_0 = { 0, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_1 = { 0, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_2 = { 0, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_3 = { 0, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex0 = { 0, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex1 = { 0, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex2 = { 0, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex3 = { 0, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex4 = { 0, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex5 = { 0, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex6 = { 0, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex7 = { 0, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex8 = { 0, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex9 = { 0, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex10 = { 0, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex11 = { 0, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex12 = { 0, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex13 = { 0, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex14 = { 0, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex15 = { 0, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mBaseAbilityDamage = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mDodge = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackRange = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatCastRangeMod = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCooldownMod = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownEndTime = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownTotalTime = { 1, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatArmorPenetration = { 1, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentArmorPenetration = { 1, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicPenetration = { 1, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicPenetration = { 1, 26, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentLifeStealMod = { 1, 27, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentSpellVampMod = { 1, 28, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCCReduction = { 1, 29, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHP = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mExp = { 3, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 3, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 3, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 3, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit2 = { 3, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPathfindingRadiusMod = { 3, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLevelRef = { 3, 13, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mNumNeutralMinionsKilled = { 3, 14, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 15, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 16, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Minion {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 1, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 1, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 7, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 8, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 9, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 10, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 5, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_Barracks {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_HQ {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Turret {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
}
namespace Summoners_Rift {
    namespace obj_AI_Turret {
        const ObjectAttributeIndex mMaxMP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsLifestealImmune = { 1, 6, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHP = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Hero {
        const ObjectAttributeIndex mGold = { 0, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mGoldTotal = { 0, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCanCastBits1 = { 0, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mCanCastBits2 = { 0, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolvePoints = { 0, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolveFlag = { 0, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex ManaCost_0 = { 0, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_1 = { 0, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_2 = { 0, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_3 = { 0, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex0 = { 0, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex1 = { 0, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex2 = { 0, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex3 = { 0, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex4 = { 0, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex5 = { 0, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex6 = { 0, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex7 = { 0, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex8 = { 0, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex9 = { 0, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex10 = { 0, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex11 = { 0, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex12 = { 0, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex13 = { 0, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex14 = { 0, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex15 = { 0, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsLifestealImmune = { 1, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mBaseAbilityDamage = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mDodge = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackRange = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatCastRangeMod = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCooldownMod = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownEndTime = { 1, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownTotalTime = { 1, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatArmorPenetration = { 1, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentArmorPenetration = { 1, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicPenetration = { 1, 26, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicPenetration = { 1, 27, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentLifeStealMod = { 1, 28, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentSpellVampMod = { 1, 29, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCCReduction = { 1, 30, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHP = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mExp = { 3, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 3, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 3, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 3, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit2 = { 3, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPathfindingRadiusMod = { 3, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLevelRef = { 3, 13, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mNumNeutralMinionsKilled = { 3, 14, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 15, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 16, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Minion {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 1, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 1, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 7, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 8, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 9, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 10, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsLifestealImmune = { 1, 11, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 5, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_HQ {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_BarracksDampener {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_Barracks {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
}
namespace The_Twisted_Treeline_Beta {
    namespace obj_BarracksDampener {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Hero {
        const ObjectAttributeIndex mGold = { 0, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mGoldTotal = { 0, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCanCastBits1 = { 0, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mCanCastBits2 = { 0, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolvePoints = { 0, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mEvolveFlag = { 0, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex ManaCost_0 = { 0, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_1 = { 0, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_2 = { 0, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_3 = { 0, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex0 = { 0, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex1 = { 0, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex2 = { 0, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex3 = { 0, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex4 = { 0, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex5 = { 0, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex6 = { 0, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex7 = { 0, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex8 = { 0, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex9 = { 0, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex10 = { 0, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex11 = { 0, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex12 = { 0, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex13 = { 0, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex14 = { 0, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ManaCost_Ex15 = { 0, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mBaseAbilityDamage = { 1, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mDodge = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackRange = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatCastRangeMod = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCooldownMod = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownEndTime = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPassiveCooldownTotalTime = { 1, 22, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatArmorPenetration = { 1, 23, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentArmorPenetration = { 1, 24, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicPenetration = { 1, 25, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicPenetration = { 1, 26, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentLifeStealMod = { 1, 27, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentSpellVampMod = { 1, 28, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentCCReduction = { 1, 29, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHP = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mExp = { 3, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 3, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 3, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 3, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit2 = { 3, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPathfindingRadiusMod = { 3, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLevelRef = { 3, 13, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mNumNeutralMinionsKilled = { 3, 14, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 15, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 16, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_Barracks {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 1, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_Shop {
    }
    namespace obj_AI_Minion {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetime = { 1, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxLifetime = { 1, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mLifetimeTicks = { 1, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 6, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 7, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 8, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 14, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 15, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 16, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPARRegenRate = { 1, 17, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicReduction = { 1, 18, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentMagicReduction = { 1, 19, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP = { 1, 20, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP = { 1, 21, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxMP2 = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMP2 = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 4, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 5, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 3, 6, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 3, 7, ObjectAttributeType::INTEGER_VALUE };
    }
    namespace obj_AI_Turret {
        const ObjectAttributeIndex mHP = { 1, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMaxHP = { 1, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex ActionState = { 1, 2, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex MagicImmune = { 1, 3, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsInvulnerable = { 1, 4, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex IsPhysicalImmune = { 1, 5, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mBaseAttackDamage = { 1, 6, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mArmor = { 1, 7, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mSpellBlock = { 1, 8, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mAttackSpeedMod = { 1, 9, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatPhysicalDamageMod = { 1, 10, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentPhysicalDamageMod = { 1, 11, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatMagicDamageMod = { 1, 12, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mHPRegenRate = { 1, 13, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mFlatBubbleRadiusMod = { 3, 0, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mPercentBubbleRadiusMod = { 3, 1, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mMoveSpeed = { 3, 2, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mCrit = { 3, 3, ObjectAttributeType::FLOAT_VALUE };
        const ObjectAttributeIndex mIsTargetable = { 5, 0, ObjectAttributeType::INTEGER_VALUE };
        const ObjectAttributeIndex mIsTargetableToTeamFlags = { 5, 1, ObjectAttributeType::INTEGER_VALUE };
    }
}