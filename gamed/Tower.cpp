#include "stdafx.h"
#include "Tower.h"
#include "ObjectAttributes.h"
#include "UpdateStatsPacket.h"

#define TQUEUE_UPDATE(mAttribute) UpdateStatsPacket::QueueUpdate(GetNetworkID(), Summoners_Rift::obj_AI_Turret::mAttribute, mAttribute);
#define TQUEUE_UPDATE_EX(mAttribute, mValue) UpdateStatsPacket::QueueUpdate(GetNetworkID(), Summoners_Rift::obj_AI_Turret::mAttribute, mValue);

void CTower::SetMaxMP(float fMaxMP) {
    this->mMaxMP = fMaxMP;
    TQUEUE_UPDATE(mMaxMP)
}
void CTower::SetMP(float fMP) {
    this->mMP = fMP;
    TQUEUE_UPDATE(mMP)
}
void CTower::SetActionState(DWORD dwActionState) {
    this->ActionState = dwActionState;
    TQUEUE_UPDATE(ActionState)
}
void CTower::SetMagicImmune(DWORD bMagicImmune) {
    this->MagicImmune = bMagicImmune;
    TQUEUE_UPDATE(MagicImmune)
}
void CTower::SetIsInvulnerable(DWORD bIsInvulnerable) {
    this->IsInvulnerable = bIsInvulnerable;
    TQUEUE_UPDATE(IsInvulnerable)
}
void CTower::SetIsPhysicalImmune(DWORD bIsPhysicalImmune) {
    this->IsPhysicalImmune = bIsPhysicalImmune;
    TQUEUE_UPDATE(IsPhysicalImmune)
}
void CTower::SetIsLifestealImmune(DWORD bIsLifeStealImmune) {
    this->IsLifestealImmune = bIsLifeStealImmune;
    TQUEUE_UPDATE(IsLifestealImmune)
}
void CTower::SetBaseAttackDamage(float fBaseAttackDamage) {
    this->mBaseAttackDamage = fBaseAttackDamage;
    TQUEUE_UPDATE(mBaseAttackDamage)
}
void CTower::SetArmor(float fArmor) {
    this->mArmor = fArmor;
    TQUEUE_UPDATE(mArmor)
}
void CTower::SetSpellBlock(float fSpellBlock) {
    this->mSpellBlock = fSpellBlock;
    TQUEUE_UPDATE(mSpellBlock)
}
void CTower::SetAttackSpeedMod(float fAttackSpeedMod) {
    this->mAttackSpeedMod = fAttackSpeedMod;
    TQUEUE_UPDATE(mAttackSpeedMod)
}
void CTower::SetFlatPhysicalDamageMod(float fFlatPhysicalDamageMod) {
    this->mFlatPhysicalDamageMod = fFlatPhysicalDamageMod;
    TQUEUE_UPDATE(mFlatPhysicalDamageMod)
}
void CTower::SetPercentPhysicalDamageMod(float fPercentPhysicalDamageMod) {
    this->mPercentPhysicalDamageMod = fPercentPhysicalDamageMod;
    TQUEUE_UPDATE(mPercentPhysicalDamageMod)
}
void CTower::SetFlatMagicDamageMod(float fFlatMagicDamageMod) {
    this->mFlatMagicDamageMod = fFlatMagicDamageMod;
    TQUEUE_UPDATE(mFlatMagicDamageMod)
}
void CTower::SetHPRegenRate(float fHPRegenRate) {
    this->mHPRegenRate = fHPRegenRate;
    TQUEUE_UPDATE(mHPRegenRate)
}
void CTower::SetMaxHP(float fMaxHP) {
    this->mMaxHP = fMaxHP;
    TQUEUE_UPDATE(mMaxHP)
}
void CTower::SetHP(float fHP) {
    this->mHP = fHP;
    TQUEUE_UPDATE(mHP)
}
void CTower::SetFlatBubbleRadiusMod(float fFlatBubbleRadiusMod) {
    this->mFlatBubbleRadiusMod = fFlatBubbleRadiusMod;
    TQUEUE_UPDATE(mFlatBubbleRadiusMod)
}
void CTower::SetPercentBubbleRadiusMod(float fPercentBubbleRadiusMod) {
    this->mPercentBubbleRadiusMod = fPercentBubbleRadiusMod;
    TQUEUE_UPDATE(mPercentBubbleRadiusMod)
}
void CTower::SetMoveSpeed(float fMoveSpeed) {
    this->SetSpeed(fMoveSpeed);
    TQUEUE_UPDATE_EX(mMoveSpeed, fMoveSpeed);
}
void CTower::SetCrit(float fCrit) {
    this->mCrit = fCrit;
    TQUEUE_UPDATE(mCrit)
}
void CTower::SetIsTargetable(bool bIsTargetable) {
    this->mIsTargetable = bIsTargetable;
    TQUEUE_UPDATE(mIsTargetable)
}
void CTower::SetIsTargetableToTeamFlags(DWORD dwIsTargetableToTeamFlags) {
    this->mIsTargetableToTeamFlags = dwIsTargetableToTeamFlags;
    TQUEUE_UPDATE(mIsTargetableToTeamFlags)
}

float CTower::GetRange() {
    return fRange;
}

float CTower::GetY() {
    return y;
}

float CTower::GetX() {
    return x;
}

CTower::CTower(WORD index, char *szName, Team tTeam) : CUnit(index, szName, NULL, tTeam) {
    fRange = 0;
}
