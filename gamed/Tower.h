#pragma once
#include "Unit.h"

class CTower : public CUnit { //obj_AI_Turret
    public:
        float fRange;
        CTower(WORD index, char *szName, Team tTeam);
        float GetX();
        float GetY();
        float GetRange();
        void SetMaxMP(float fMaxMP);
        void SetMP(float fMP);
        void SetActionState(DWORD dwActionState);
        void SetMagicImmune(DWORD bMagicImmune);
        void SetIsInvulnerable(DWORD bIsInvulnerable);
        void SetIsPhysicalImmune(DWORD bIsPhysicalImmune);
        void SetIsLifestealImmune(DWORD bIsLifeStealImmune);
        void SetBaseAttackDamage(float fBaseAttackDamage);
        void SetArmor(float fArmor);
        void SetSpellBlock(float fSpellBlock);
        void SetAttackSpeedMod(float fAttackSpeedMod);
        void SetFlatPhysicalDamageMod(float fFlatPhysicalDamageMod);
        void SetPercentPhysicalDamageMod(float fPercentPhysicalDamageMod);
        void SetFlatMagicDamageMod(float fFlatMagicDamageMod);
        void SetHPRegenRate(float fHPRegenRate);
        void SetMaxHP(float fMaxHP);
        void SetHP(float fHP);
        void SetFlatBubbleRadiusMod(float fFlatBubbleRadiusMod);
        void SetPercentBubbleRadiusMod(float fPercentBubbleRadiusMod);
        void SetMoveSpeed(float fMoveSpeed);
        void SetCrit(float fCrit);
        void SetIsTargetable(bool bIsTargetable);
        void SetIsTargetableToTeamFlags(DWORD dwIsTargetableToTeamFlags);
};