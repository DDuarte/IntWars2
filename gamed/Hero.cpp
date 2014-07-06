#include "stdafx.h"
#include "RafDB.h"
#include "Packets.h"
#include "Hero.h"
#include "Server.h"
#include "LuaVM.h"
#include "UpdateStatsPacket.h"
#include "SpellCmdPacket.h"
#include "MissileSpawnPacket.h"
//URF BUFF = 'InternalTestBuff'


#include "MovementAnsPacket.h"

#include <queue>
std::string CSpell::GetName() {
    if(owner) {
        return owner->GetSpellName(iIndex);
    }
    return std::string();
}
CIniBin CSpell::GetSpell() {
    if(owner) {
        return RafDB::GetSpell(owner->szModel, (char *)GetName().c_str());
    }
    return CIniBin();
}
CParticle *CSpellWork::AddTargetParticle(CHero *target, CHero *src, const char *szParticle, float fTimer) {
    CParticle *p = AddParticle(src, szParticle, fTimer);
    p->applyToId = target->GetNetworkID();
    return p;
}

#define HQUEUE_UPDATE_EX(mAttribute, mValue) UpdateStatsPacket::QueueUpdate(GetNetworkID(), Summoners_Rift::obj_AI_Hero::mAttribute, mValue);
#define HQUEUE_UPDATE(mAttribute) UpdateStatsPacket::QueueUpdate(GetNetworkID(), Summoners_Rift::obj_AI_Hero::mAttribute, mAttribute);

void CHero::SendWaypoints() {
    MovementAnsPacket answer(GetNetworkID(), GetWaypoints());
    server->listener->_handler->broadcastPacket(answer);
}

float CHero::GetAARange() {
    return mAttackRange + GetGameplayCollisionRadius();
}
float CHero::GetGameplayCollisionRadius() {
    float fVal = charData->GameplayCollisionRadius;
    return fVal ? fVal : 65;
}
float CHero::GetAttackDelayCastOffsetPercent() {
    return charData->BaseAttack.AttackDelayCastOffsetPercent;
}
float CHero::GetAttackDelayOffsetPercent() {
    return charData->BaseAttack.AttackDelayOffsetPercent;
}
float CHero::GetAttackSpeedPerLevel() {
    return charData->AttackSpeedPerLevel;//* 0.009999999776482582; (from LoL Client reverse)
}
DWORD CHero::GetClosestAATargetId() {
    if(aaTarget) {
        return aaTarget->GetNetworkID();
    }
    DWORD netId = 0;
    float fBestRange = 900;
    for(auto p : CObjectManager::GetPlayers(GetEnemyTeam())) {
        float fDistance = GetDistance(p);
        if(fDistance < fBestRange) {
            netId = p->GetNetworkID();;
            fBestRange = fDistance;
        }
    }
    return netId;
}
void CHero::SetAATarget(CHero *target) {
    if(!bCanAttack) {
        //send 33 for orbwalk and drop attack
        AbortAnimationPacket aborAnimationPacket(GetNetworkID());
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&aborAnimationPacket), sizeof(AbortAnimationPacket), CHL_S2C);
    }
    if(aaWork->IsValid()) {
        if(!bCanAttack) {
            UnlockAttack();
            //send 33 for orbwalk and drop attack
            AbortAnimationPacket aborAnimationPacket(GetNetworkID());
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&aborAnimationPacket), sizeof(AbortAnimationPacket), CHL_S2C);
        }
        aaWork->Drop();
    }
    aaTarget = target;
}
bool CHero::DoAutoAttack() {
    aaWork->Drop();
    if(aaTarget && GetCollisionDistance(aaTarget) <= mAttackRange) {
        if(!bCanAttack) {
            return true;
        }
        Stop();
        RotateTo(aaTarget->GetPos());
        float fWindupTime = 1.6f * (0.3f + GetAttackDelayCastOffsetPercent()) * (1 - GetAttackDelayOffsetPercent()); //correct
        float fASPDExtra = (1 + 0/*aspdbonuses*/ + GetAttackSpeedPerLevel() * (GetLevel() - 1) / 100);
        float fTotalCast = 1 / (1 / (1.6f * (1 + GetAttackDelayOffsetPercent())) * fASPDExtra); //fullFormula: 1/(1.6 * (1 + stats.attackspeedoffset)) * (1 + aspdbonuses + stats.aspdperlevel * (level - 1) / 100)
        LockAttack(fTotalCast);
        auto objAttack = CObjectManager::NewObject();
        //CSpellWork w(this, 0);
        AggroPacket aggroPacket(GetNetworkID(), aaTarget->GetNetworkID());//broadcast BF NETID targetId except to peer
        server->listener->_handler->sendToOthers(this, reinterpret_cast<uint8 *>(&aggroPacket), sizeof(AggroPacket), CHL_S2C);
        AttackDataStruct *attackAnim = charData->GetRandomAttackAnim();
        printf("Doing attack with anim: %02X\r\n", attackAnim->spellSlot);
        if(charData->IsMelee) {
            MeleeAttackPacket aaPacket(GetNetworkID(), aaTarget->GetNetworkID(), objAttack->GetNetworkID(), attackAnim->spellSlot, GetX(), GetY()); //broadcast 1A NETID targetID 0x80 projectileID spellID(0x41) fX fY
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&aaPacket), sizeof(MeleeAttackPacket), CHL_S2C);
        } else {
            AttackProjectilePacket aaPacket(GetNetworkID(), aaTarget->GetNetworkID(), objAttack->GetNetworkID(), attackAnim->spellSlot, GetX(), GetY()); //broadcast 1A NETID targetID 0x80 projectileID spellID(0x41) fX fY
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&aaPacket), sizeof(AttackProjectilePacket), CHL_S2C);
        }
        UpdateDrawPos updatePos(GetNetworkID(), GetX(), GetY(), 2, aaTarget->GetNetworkID()); //0xFE
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&updatePos), sizeof(UpdateDrawPos), CHL_S2C);
        CHero *owner = this;
        DWORD aaId = objAttack->GetNetworkID();
        //DISABLED TO AVOID CRASHES !??? VVVVVVV
        /*aaWork = server->ScheduleWork(fWindupTime, [owner, aaId]() {  //Release Missile after ~0.28 secs, use objAttack netId
            ReleaseMissilePacket rlsMissile(owner->GetNetworkID(), aaId); //6D
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&rlsMissile), sizeof(ReleaseMissilePacket), CHL_S2C);
            owner->aaWork->Drop();
        });*/
        return true;
    }
    return false;
}
bool CHero::IsValidTarget(CHero *target) {
    return target->GetNetworkID() != GetNetworkID() &&
           target->tTeam != tTeam;
}
void CHero::HandleMoveRequest(Vector2f waypoint, DWORD targetId) {
    if(!vSpells.empty()) {
        vSpells[0].Cancel();
        //send 33 to cancel animation
        AbortAnimationPacket aborAnimationPacket(GetNetworkID());
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&aborAnimationPacket), sizeof(AbortAnimationPacket), CHL_S2C);
    }
    //cancel spell work here
    if(targetId) {
        Stop();
        CHero *h = (CHero *)CObjectManager::GetUnit(targetId);
        if(h && IsValidTarget(h)) {   //check if valid target
            if(h != aaTarget) {
                SetAATarget(h); //...
            }
            if(!DoAutoAttack()) {
                moveTo(Vector2f(GetX(), GetY()), Vector2f(aaTarget->GetX(), aaTarget->GetY()), mAttackRange);  //if not in target range, move?
            }
        }
    } else {
        if(aaTarget) {
            SetAATarget(NULL);
        }
        moveTo(waypoint);
    }
}

void CBuffSlot::SetBuff(BYTE type, BYTE bVisible, DWORD dwHash, float fTime, UINT nStacks) {
    this->bType = type;
    this->bVisible = bVisible;
    this->dwHash = dwHash;
    this->Renew(fTime);
    this->nStacks = nStacks;
}
bool CBuffSlot::IsExpired() {
    return fEnd < server->GetTime();
}
void CBuffSlot::Renew(float fTime) {
    fStart = server->GetTime();
    fEnd = fStart + fTime;
}

void CBuffSlot::SetFree() {
    dwHash = 0;
    nStacks = 1;
}

bool CBuffSlot::IsFree() {
    return dwHash == 0;// || fEnd >= server->GetTime();
}

CBuffSlot::CBuffSlot(BYTE slot) : CBuffSlot() {
    iSlot = slot;
}

CBuffSlot::CBuffSlot() {
    ZeroMemory(this, sizeof(CBuffSlot) - 1);
    nStacks = 1;
}

CBuffSlot *CBuffManager::GetFreeSlot(const char *szBuff) {
    DWORD dwHash = szBuff ? getHash(szBuff) : 0;
    CBuffSlot *freeSlot = NULL;
    for(UINT i = 0; i < BUFF_SLOTS; i++) {
        if(vSlots[i].IsFree()) {
            if(freeSlot == NULL) { freeSlot = &vSlots[i]; }
        } else if(dwHash && vSlots[i].dwHash == dwHash) {
            return &vSlots[i];
        }
    }
    return freeSlot;
}
void CBuffManager::ExpireBuff(CBuffSlot *slot) {
    if(!slot->IsFree() && slot->IsExpired()) {
        slot->SetFree();
        RemoveBuffPacket drawPos(p->GetNetworkID(), slot->iSlot, slot->dwHash); //4F
        server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&drawPos), sizeof(RemoveBuffPacket), CHL_S2C);
    } else {
    }
}

CBuffSlot *CHero::ApplyBuff(UINT iType, float fVisible, const char *szBuff, float fTime, UINT nStacks) {
    CBuffSlot *bS = buffManager.GetFreeSlot(szBuff);
    if(bS) {
        //if(bS->nStacks == 1) {
        bS->nStacks = nStacks;
        bS->SetBuff(iType, fVisible, getHash(szBuff), fTime);
        ApplyBuffPacket drawPos(GetNetworkID(), bS->iSlot, bS->bType, bS->nStacks, !bS->bVisible, bS->dwHash, GetHash(), fTime); //4F
        server->listener->_handler->sendPacket(GetPeer(), reinterpret_cast<uint8 *>(&drawPos), sizeof(ApplyBuffPacket), CHL_S2C);
        server->ScheduleWork(fTime, [this, bS]() { this->buffManager.ExpireBuff(bS); });
        /*} else {
            bS->nStacks += nStacks;
            bS->Renew(fTime);
            server->ScheduleWork(fTime, [this, bS]() { this->buffManager.ExpireBuff(bS); });
            //update...
        }*/
        return bS;
    }
    return NULL;
}

void CHero::RegenTick() {
    AddHP(mHPRegenRate / 10);
    AddMP(mPARRegenRate / 10);
}

void CHero::AddHP(float fAdd) {
    if(mHP == mMaxHP) {
        return;
    }
    float fNew = mHP + fAdd;
    if(fNew > mMaxHP) {
        fNew = mMaxHP;
    }
    SetHP(fNew);
}
void CHero::AddMP(float fAdd) {
    if(mMP == mMaxMP) {
        return;
    }
    float fNew = mMP + fAdd;
    if(fNew > mMaxMP) {
        fNew = mMaxMP;
    }
    SetMP(fNew);
}

UINT CHero::GetHash() {
    return getCharacterHash(szType, mSkinId);
}
void CHero::LockAttack(float fTimer) {
    bCanAttack = false;
    if(fTimer) {
        attackLockWork = server->ScheduleWork(fTimer, [this]() { this->UnlockAttack(); });
    }
}
void CHero::UnlockAttack() {
    bCanAttack = true;
    attackLockWork->Drop();
    DoAutoAttack();
}
void CHero::LockCast(float fTimer) {
    bCanCast = false;
    if(fTimer) {
        castLockWork = server->ScheduleWork(fTimer, [this]() { this->UnlockCast(); });
    }
}
void CHero::UnlockCast() {
    bCanCast = true;
    castLockWork->Drop();
    Resume();
}
void CHero::LockMove(float fTimer) {
    bCanMove = false;
    if(fTimer) {
        moveLockWork = server->ScheduleWork(fTimer, [this]() { this->UnlockMove(); });
    }
    Pause();
}
void CHero::UnlockMove() {
    //33 NETID 00 00 00 00 00, here to release animation and walk
    bCanMove = true;
    moveLockWork->Drop();
    //setWaypoints(_getWaypoints()); //need to restart waypoints and updateTimer
    //SendWaypoints();
}
UINT CHero::GetActionState() {
    DWORD dwState = 0x00400000;
    if(bCanAttack) { dwState |= 0x01; }
    if(bCanCast) { dwState |= 0x02; }
    if(bCanMove) { dwState |= 0x04; }
    return dwState;
}
UINT CHero::GetSpellBits() {
    DWORD castBits = inv.GetTrinket()->ID > 0 ? 0x0C00 : 0x0800; //0xC00 if trinket
    if(spells[0].iLevel) { castBits |= 1; }
    if(spells[1].iLevel) { castBits |= 2; }
    if(spells[2].iLevel) { castBits |= 4; }
    if(spells[3].iLevel) { castBits |= 8; }
    return castBits;
}
bool CHero::SetType(const char *szType) {
    if(CUnit::SetType(szType)) {
        SetModel(szType);
        return true;
    }
    return false;
}
bool CHero::Morph(const char *szType) {
    if(SetType(szType)) {
        charData = CharacterDataStruct::GetCharacterStruct(szType);
        CalculateStats();
        SendHeroStats();
        //..
        return true;
    }
    return false;
}
bool CHero::SetModel(const char *szModel) {
    if(CUnit::SetModel(szModel)) {
        UpdateModel modelPacket(GetNetworkID(), this->szModel); //96
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&modelPacket), sizeof(UpdateModel), CHL_S2C);
        //Logging->writeLine("set champ model to %s\n", this->szModel);
        return true;
    }
    return false;
}
void CHero::SetupSpell(BYTE b1) {
    _UpdateDrawPos updatePos(GetNetworkID(), GetX(), GetY(), b1, 0); //0xFE
    server->listener->_handler->sendPacket(GetPeer(), reinterpret_cast<uint8 *>(&updatePos), sizeof(_UpdateDrawPos), CHL_S2C);
}

CSpellWork::CSpellWork(CHero *owner, DWORD iSpell) : CSpellWork() {
    this->owner = owner;
    this->iSpell = iSpell;
    fCooldown = 0;
    fManaCost = 0;
    if(owner) {
        //get mana, cooldown, etc
        CSpell *spell = owner->GetSpell(iSpell);
        fCastFrame = spell->GetCastTime();
        fDelay = 0;
        fVisible = 1.0;
        fCooldown = spell->GetCooldown();
        fManaCost = spell->GetManaCost();
        spellName = spell->GetName();
        bCanCancel = spell->GetCantCancelWhileWindingUp() == false;
    }
}

CSpellWork::CSpellWork(CHero *owner, DWORD iSpell, float fX, float fY) : CSpellWork(owner, iSpell) {
    SetPosition(fX, fY);
}

/*CSpellWork(DWORD iSpell, const char *szSpell, float fX, float fY) : CSpellWork() {
this->iSpell = iSpell;
spellName = std::string(szSpell);
this->fX = fX;
this->fY = fY;
animation = std::string();
}*/

CSpellWork::CSpellWork(CHero *hero, const char *szSpell, DWORD iSpell) : CSpellWork() {
    owner = hero;
    spellName = szSpell;
    this->iSpell = iSpell;
}

CSpellWork::CSpellWork() {
    ZeroMemory(this, sizeof(this));
    iUpdatePos = 3;
    wFlag = 0x66;
    tp = { 0, 0, 0 };
    castWork = nullptr;
}

void CSpellWork::Cancel() {
    if(bCanCancel) {
        if(castWork) {
            castWork->Drop();
        }
        owner->UnlockMove();
        owner->PopSpell();
        owner->DoSpellCast();
    }
}

CSpell *CSpellWork::GetSpell() {
    return owner ? owner->GetSpell(iSpell) : 0;
}
bool CSpellWork::DoCast() { //Pre Cast / Winding Up
    if(iUpdatePos == 0) {
        owner->SetupSpell(0);
    } else if(iUpdatePos == 1) {
        owner->SetupSpell(1);
    } else if(iUpdatePos == 2) {
        //owner->SetupSpell(2); //TODO: MORE DATA
    }
    SpellCmdPacket spellCmd = SpellCmdPacket::BuildSpell(owner, iSpell, getHash((char *)spellName.c_str()), wFlag, fX, fY, fCastFrame, fDelay, fVisible, fCooldown, fManaCost);
    spellCmd.Broadcast();
    this->netId = spellCmd.netID;
    if(!bCanCancel) {
        owner->LockMove(fCastFrame);
    }
    if(castWork) {
        castWork->Drop();
    }
    CHero *hero = owner;
    castWork = server->ScheduleWork(fCastFrame, [hero]() {
        hero->DoSpellWork();
    });
    return true;
}
bool CSpellWork::DoWork() { //After Cast / Release missiles
    if(fManaCost) {
        owner->SetMP(owner->mMP - fManaCost);
    }
    if(bStop) {
        owner->Stop();
    }
    if(bSetFacing) {
        UpdateDash drawPos(owner->GetNetworkID(), owner->GetX(), 0, owner->GetY(), 0.083333); //4F , doesnt seem to change anything
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&drawPos), sizeof(UpdateDash), CHL_S2C);
    }
    /*if((int)bin.GetVar("SpellData", "CantCancelWhileWindingUp")) {
        owner->LockMove(fCastFrame); //QUEUE CAST DELAY HERE / this->LockAttack(fCastFrame);
        //sendStop()
    }
    if((int)bin.GetVar("SpellData", "AILifetime")) {
        //Schedule remove missile based on AILifetime?
    }*/
    if(fReleaseCast) {
        DWORD dwSrcID = owner->GetNetworkID();
        DWORD dwNetID = this->netId;
        server->ScheduleWork(fReleaseCast, [dwSrcID, dwNetID]() {
            ReleaseMissilePacket rlsMissile(dwSrcID, dwNetID); //6D release missile
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&rlsMissile), sizeof(ReleaseMissilePacket), CHL_S2C);
        });
    }
    if(bAnimation) {
        AnimateSpellPacket nidaPounce2(owner->GetNetworkID(), 0, (char *)GetSpell()->GetAnimationName().c_str()); //AF
        nidaPounce2.f1 = 0;
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&nidaPounce2), sizeof(AnimateSpellPacket), CHL_S2C);
    }
    if(tp.fTime > 0) {
        CHero *hero = owner;
        float fX = tp.fX;
        float fY = tp.fY;
        server->ScheduleWork(tp.fTime, [hero, fX, fY]() {
            hero->SetPos(Vector2f(fX, fY));
            hero->SendWaypoints(); //ez teleport
        });
    }
    //apply cast spells..
    for(UINT i = 0; i < spells.size(); i++) {
        DWORD dwSrcID = owner->GetNetworkID();
        SpellCmdPacket *spellCmd2 = SpellCmdPacket::_BuildSpell(owner, spells[i].iSpellSlot, getHash((char *)spells[i].sSpell.c_str()), wFlag, fX, fY, fCastFrame, fDelay, fVisible, fCooldown, fManaCost);
        server->ScheduleWork(spells[i].fTime, [spellCmd2, dwSrcID]() {
            spellCmd2->Broadcast();
            ReleaseMissilePacket rlsMissile(dwSrcID, spellCmd2->netID); //6D creates obj
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&rlsMissile), sizeof(ReleaseMissilePacket), CHL_S2C);
            //release
            delete spellCmd2;
        });
    }
    //apply buffs..
    for(UINT i = 0; i < buffs.size(); i++) {
        owner->ApplyBuff(buffs[i].bType, 1, buffs[i].sBuff.c_str(), buffs[i].fTime, buffs[i].nStacks);
    }
    //apply set spell name
    for(UINT i = 0; i < spellSets.size(); i++) {
        //...spellSets[i].sName
    }
    //apply particles
    for(UINT i = 0; i < particles.size(); i++) {
        CParticle *p = particles[i];
        server->ScheduleWork(p->fTimer, [p]() { p->Broadcast(); delete p; });
    }
    //Send Missiles
    for(UINT i = 0; i < missiles.size(); i++) {
        CParticle *m = missiles[i];
        CHero *hero = owner;
        server->ScheduleWork(m->fTimer, [m, hero]() {
            MissileSpawnPacket p(hero, m->dwHash, m->fX, m->fY);
            p.Broadcast();
        });
    }
    //check for colision on HaveHitBone?
    //bRet = spellCmd.netID;
    //}
    owner->PopSpell();
    owner->DoSpellCast();
    return true;// particles.size() == 0;
}

void CSpellWork::SetTeleport(float fX, float fY, float fTimer) {
    tp = { fX, fY, fTimer };
}

void CSpellWork::AddSpellSet(BYTE iSlot, const char *szSpell, float fTime) {
    SpellSet s = { iSlot, szSpell, fTime };
    spellSets.push_back(s);
    //..
}

void CSpellWork::AddBuff(BYTE bType, const char *szBuff, float fTime, int nStacks) {
    CBuffWork b = { bType, szBuff, fTime, nStacks };
    buffs.push_back(b);
}

void CSpellWork::AddSpell(BYTE iSlot, const char *szSpell, float fTime) {
    ExtraSpell s = { iSlot, szSpell, fTime };
    spells.push_back(s);
}

CParticle *CSpellWork::AddMissile(CHero *src, const char *szSpell, float fTimer) {
    CParticle *p = new CParticle(src, (char *)szSpell);
    p->fX = fX;
    p->fY = fY;
    p->fTimer = fTimer;
    missiles.push_back(p);
    return p;
}

CParticle *CSpellWork::AddParticle(CHero *src, const char *szParticle, float fTimer) {
    CParticle *p = new CParticle(src, (char *)szParticle);
    p->fX = fX;
    p->fY = fY;
    p->fTimer = fTimer;
    particles.push_back(p);
    return p;
}

void CSpellWork::SetPosition(float fX, float fY) {
    this->fX = fX;
    this->fY = fY;
}

UINT CHero::CastSpell(UINT iIndex, float fX, float fY) {
    DWORD bRet = 0;
    if(iIndex > 4) { return bRet; }
    std::string spellName = GetSpellName(iIndex);
    CIniBin bin = RafDB::GetSpell(szType, (char *)spellName.c_str());
    if(bin) {
        float fCastFrame = (float)bin.GetVar("SpellData", "OverrideCastTime");
        if(fCastFrame == 0) {
            fCastFrame = (1 + (float)bin.GetVar("SpellData", "DelayTotalTimePercent")) * (1 + (float)bin.GetVar("SpellData", "DelayCastOffsetPercent"));
        }
        //DelayCastOffsetPercent, DelayTotalTimePercent
        SpellCmdPacket spellCmd = SpellCmdPacket::BuildSpell(this, iIndex, 0x66 , fX, fY, fCastFrame, 0, 1.0, 3.0, 25.0);
        spellCmd.Send(_peer);
        if((int)bin.GetVar("SpellData", "CantCancelWhileWindingUp")) {
            //this->LockMove(fCastFrame); //QUEUE CAST DELAY HERE / this->LockAttack(fCastFrame);
        }
        if((int)bin.GetVar("SpellData", "AILifetime")) {
            //Schedule remove missile based on AILifetime?
        }
        //check for colision on HaveHitBone?
        bRet = spellCmd.netID;
    }
    return bRet;
}
bool CHero::CallSpell(UINT iIndex, float fX, float fY) {
    if(iIndex == 0x0A) {    //Trinket
    } else if(iIndex == 0x0B) { //Recall
        CSpellWork w(this, "Recall", 0x0B);
        w.SetPosition(fX, fY);
        w.wFlag = 0x77;
        w.fVisible = 8.5;
        w.fCastFrame = 0.5;
        w.fCooldown = 0;
        w.fManaCost = 0;
        //9E NETID 07 01 00
        //D7 00000 134 Bytes
        //FE NETID 21 01 CE E2 A0 06
        w.AddTargetParticle(this, this, "TeleportHome.troy", w.fCastFrame);
        w.AddBuff(1, "Recall", 7.900002, 1); //0,6 delay for apply? from 8.5, looks good
        QueueSpell(w);
        //8.5 seconds later
        //72 NETID x,y,z teleports?
        //37 NETID ... delete particle (TeleportHome.troy)
        //D7 00000 134 bytes
        //7A NETID Lose buff
        //[     0] pe:AddParticle(self,'teleportarrive.troy') --Applied to (null)
        //0.5 secs later (maybe executed at another handler or status check)
        //[63] MoveAns(self, ...)--[1] STOP
        //FE NETID 21 01 8E A2 D2 05
    } else {
        /*if(iIndex > 4) { return false; }
        std::string spellName = GetSpell(iIndex);
        CIniBin bin = RafDB::GetSpell(szType, (char *)spellName.c_str());
        if(bin) {
            float fCastFrame = (float)bin.GetVar("SpellData", "OverrideCastTime");
            if(fCastFrame == 0) {
                fCastFrame = (float)bin.GetVar("SpellData", "CastFrame") / 30;
            }
            //DelayCastOffsetPercent, DelayTotalTimePercent
            stop();
            SpellCmdPacket spellCmd = SpellCmdPacket::BuildSpell(this, iIndex, 0x66, fX, fY, fCastFrame, 0, 1.0, 3.0, 25.0);
            spellCmd.Send((int)_peer);
        }*/
        char szLuaPath[124];
        sprintf(szLuaPath, "Scripts\\Heroes\\%s.lua", szType);
        luaVM->RunScript(szLuaPath, "CastSpell", this, iIndex, fX, fY);
        //CLuaVM::DoScript(szLuaPath, "CastSpell", this, iIndex);
        //BOOL bBuiltStruct = CLuaVM::DoScript((QString().sprintf("%s\\templates\\%s\\%02X.lua", gApp.szDllPath, p->bOut ? "out" : "in", p->GetHeader())).toLatin1().data());
        //luabind::object lScript = GetLuaScript();
        //lScript["CastSpell"](this, iIndex);
        return true;
    }
    return false;
}
bool CHero::LevelSkill(UINT spellIndex) {
    if(iSpellPoints < 0 || spellIndex > 3 || spells[spellIndex].iLevel >= 5 || (spellIndex == 3 && spells[spellIndex].iLevel >= 3)) { return false; }
    //!TODO Check if can up skill? :)
    CSpell *spell = GetSpell(spellIndex);
    SkillUpResponse skillUpResponse(GetNetworkID());
    skillUpResponse.skill = spellIndex;
    skillUpResponse.level = ++spell->iLevel;
    skillUpResponse.pointsLeft = --iSpellPoints;
    bool p1 = server->listener->_handler->sendPacket(GetPeer(), reinterpret_cast<uint8 *>(&skillUpResponse), sizeof(SkillUpResponse), CHL_S2C);
    //if first time leveling spell
    if(spells[spellIndex].iLevel == 1) {
        SetCanCastBits1(GetSpellBits());
    }
    //set mana cost
    if(spellIndex == 0) {
        SetManaCost0(spell->GetManaCost());
    } else if(spellIndex == 1) {
        SetManaCost1(spell->GetManaCost());
    } else if(spellIndex == 2) {
        SetManaCost2(spell->GetManaCost());
    } else if(spellIndex == 3) {
        SetManaCost3(spell->GetManaCost());
    }
    return true;
}
bool CHero::BuyItem(UINT itemId) {
    CIniBin bin = RafDB::GetItem(itemId);
    int nPrice = (int)bin.GetVar("Data", "Price");
    if(nPrice == 0) {     //Trinket, not ward
        CItemSlot *iSlot = inv.GetTrinket();
        if(iSlot->ID == 0) {
            iSlot->ID = itemId;
            //SetupSpell1
            BuyItemAns response(GetNetworkID(), iSlot->ID, iSlot->iSlot, iSlot->nStacks);
            server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(response), CHL_S2C);
            //SetupSpell2
            SetCanCastBits1(GetSpellBits());//CharStats 0xC00 for trinket
            //7E
            //SetupSpell1
            //7E
            return true;
        }
    } else { //real item
        if(inv.GetItemCount() >= 6) { return false; }
        CItemSlot *iSlot = inv.GetItemById(itemId);
        if(!iSlot) {
            int iPos = 0;
            while(inv.vSlots[iPos]) {
                iPos++;
            }
            iSlot = inv.GetSlot(iPos);
            *iSlot = CItemSlot(itemId);
            iSlot->iSlot = iPos;
        } else {
            iSlot->nStacks++;
        }
        AddGold(-nPrice);
        BuyItemAns response(GetNetworkID(), iSlot->ID, iSlot->iSlot, iSlot->nStacks);
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(response), CHL_S2C);
        return true;
    }
    return false;
    //get item ref by itemId
    //if () //if is trinket
}
bool CHero::SellItem(UINT iSlot) {
    CItemSlot *s = inv.GetSlot(iSlot);
    if(s) {
        CIniBin bin = RafDB::GetItem(s->ID);
        int nPrice = bin.GetVar("Data", "Price");
        float nSellBack = bin.GetVar("Data", "SellBackModifier");
        if(nPrice && nSellBack) {
            AddGold(nPrice * nSellBack);
        }
        if(s->nStacks == 1) {  //remove
            s->ID = 0;
        }
        s->nStacks--;
        SellItemAns response(GetNetworkID(), s->iSlot, s->nStacks);
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(SellItemAns), CHL_S2C);
        return true;
    }
    //...
    return false;
}
bool CHero::SwapItem(UINT iSlot1, UINT iSlot2) {
    CItemSlot *s1 = inv.GetSlot(iSlot1);
    CItemSlot *s2 = inv.GetSlot(iSlot2);
    if(s1 && s2) {
        s1->Swap(s2);
        SwapItemAns response(GetNetworkID(), iSlot1, iSlot2);
        server->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(SwapItemAns), CHL_S2C);
        return true;
    }
    return false;
}
void CHero::SpellInit() {
    for(UINT i = 0; i < 4; i++) {
        UINT iLevel = spells[i].iLevel;
        if(iLevel) {
            SpellSetPacket spell(GetNetworkID(), i, iLevel);
            server->listener->_handler->sendPacket(GetPeer(), reinterpret_cast<uint8 *>(&spell), sizeof(SpellSetPacket), CHL_S2C);
        }
    }
}
void CHero::SendHeroStats() { //maybe dont broadcast and send to locals?
    SetGold(mGold); //get from map
    SetCanCastBits1(GetSpellBits());
    SetCanCastBits2(0x00000003);
    SetEvolvePoints(0);
    SetEvolveFlag(0);
    HQUEUE_UPDATE(ManaCost_0);
    HQUEUE_UPDATE(ManaCost_1);
    HQUEUE_UPDATE(ManaCost_2);
    HQUEUE_UPDATE(ManaCost_3);
    HQUEUE_UPDATE(ManaCost_Ex0);
    HQUEUE_UPDATE(ManaCost_Ex1);
    HQUEUE_UPDATE(ManaCost_Ex2);
    HQUEUE_UPDATE(ManaCost_Ex3);
    HQUEUE_UPDATE(ManaCost_Ex4);
    HQUEUE_UPDATE(ManaCost_Ex5);
    HQUEUE_UPDATE(ManaCost_Ex6);
    HQUEUE_UPDATE(ManaCost_Ex7);
    HQUEUE_UPDATE(ManaCost_Ex8);
    HQUEUE_UPDATE(ManaCost_Ex9);
    HQUEUE_UPDATE(ManaCost_Ex10);
    HQUEUE_UPDATE(ManaCost_Ex11);
    HQUEUE_UPDATE(ManaCost_Ex12);
    HQUEUE_UPDATE(ManaCost_Ex13);
    HQUEUE_UPDATE(ManaCost_Ex14);
    HQUEUE_UPDATE(ManaCost_Ex15);
    SetActionState(GetActionState());
    SetMagicImmune(FALSE); //always false
    SetIsInvulnerable(FALSE); //always false
    SetIsPhysicalImmune(FALSE); //always false
    SetIsLifestealImmune(FALSE); //always false
    SetBaseAttackDamage(mBaseAttackDamage);
    SetBaseAbilityDamage(mBaseAbilityDamage);
    SetDodge(0); //always 0, no dodge
    SetCrit(mCrit);
    SetArmor(mArmor);
    SetSpellBlock(mSpellBlock);
    SetHPRegenRate(mHPRegenRate);
    SetPARRegenRate(mPARRegenRate);
    SetAttackRange(mAttackRange);
    SetFlatPhysicalDamageMod(23.34); //? what is this
    SetPercentPhysicalDamageMod(mPercentPhysicalDamageMod); //? what is this
    SetFlatMagicDamageMod(mFlatMagicDamageMod); //? what is this
    SetFlatMagicReduction(mFlatMagicReduction);
    SetPercentMagicReduction(mPercentMagicReduction);
    SetAttackSpeedMod(mAttackSpeedMod);
    SetFlatCastRangeMod(mFlatCastRangeMod);
    SetPercentCooldownMod(mPercentCooldownMod);
    SetPassiveCooldownEndTime(mPassiveCooldownEndTime);
    SetPassiveCooldownTotalTime(mPassiveCooldownTotalTime);
    SetFlatArmorPenetration(mFlatArmorPenetration);
    SetPercentArmorPenetration(1);
    SetFlatMagicPenetration(mFlatMagicPenetration);
    SetPercentMagicPenetration(1);
    SetPercentLifeStealMod(mPercentLifeStealMod);
    SetPercentSpellVampMod(mPercentSpellVampMod);
    SetPercentCCReduction(mPercentCCReduction);
    SetHP(mHP);
    SetMP(mMP);
    SetMaxHP(mMaxHP);
    SetMaxMP(mMaxMP);
    SetExp(mExp);
    SetLifetime(mLifetime);
    SetMaxLifetime(mMaxLifetime);
    SetLifetimeTicks(mLifetimeTicks);
    SetFlatBubbleRadiusMod(mFlatBubbleRadiusMod);
    SetPercentBubbleRadiusMod(mPercentBubbleRadiusMod);
    SetMoveSpeed(GetSpeed());
    SetCrit2(1.0);//(repeat ? 1.0 here ? crit2
    SetPathfindingRadiusMod(mPathfindingRadiusMod);
    SetLevelRef(mLevelRef);
    SetNumNeutralMinionsKilled(mNumNeutralMinionsKilled);
    SetIsTargetable(1);
    SetIsTargetableToTeamFlags(0x02000000);
}
void CHero::CalculateStats() {
    mMaxHP = mHP = charData->hp.GetStat(GetLevel());
    mMaxMP = mMP = charData->mp.GetStat(GetLevel());
    mHPRegenRate = charData->hpRegen.GetStat(GetLevel());
    mPARRegenRate = charData->mpRegen.GetStat(GetLevel());
    mArmor = charData->armor.GetStat(GetLevel());
    mSpellBlock = charData->spellBlock.GetStat(GetLevel());
    mBaseAttackDamage = charData->damage.GetStat(GetLevel());
    mBaseAbilityDamage = charData->abilityDamage.GetStat(GetLevel());
    mAttackSpeedMod = 1.0f;
    mCrit = charData->critChance.GetStat(GetLevel());
    mAttackRange = charData->AttackRange;
    SetSpeed(charData->MoveSpeed);
    //exp = 0;
    //printf("%s - %f / %f\r\n", szType, mHPRegenRate, mPARRegenRate);
    //Calc Stats here..
    for(UINT i = 0; i < 6; i++) {
        if(inv.vSlots[i].ID == 2023) {  //ret pot? cool down = 0, mana cost = 0
            mPercentCooldownMod = 1.0; //1.0 cdr mod? 0.4 should be max, 0.8 for urf
        }
    }
    //check extraattacks here
    //float fBaseAAProb = GetIniBin().GetVar("Data", "BaseAttack_Probability");
}
CIniBin CHero::GetIniBin() {
    return RafDB::GetCharacter(szModel);
}
//Hero Stats
void CHero::SetGold(float fGold) {
    this->mGold = fGold;
    this->mGoldTotal = fGold;
    HQUEUE_UPDATE(mGold);
    HQUEUE_UPDATE(mGoldTotal);
}
void CHero::SetCanCastBits1(DWORD dwCanCastBits1) {
    this->mCanCastBits1 = dwCanCastBits1;
    HQUEUE_UPDATE(mCanCastBits1)
}
void CHero::SetCanCastBits2(DWORD dwCanCastBits2) {
    this->mCanCastBits2 = dwCanCastBits2;
    HQUEUE_UPDATE(mCanCastBits2)
}
void CHero::SetEvolvePoints(DWORD dwEvolvePoints) {
    this->mEvolvePoints = dwEvolvePoints;
    HQUEUE_UPDATE(mEvolvePoints)
}
void CHero::SetEvolveFlag(DWORD dwEvolveFlag) {
    this->mEvolveFlag = dwEvolveFlag;
    HQUEUE_UPDATE(mEvolveFlag)
}
void CHero::SetManaCost0(float fCost) {
    this->ManaCost_0 = fCost;
    HQUEUE_UPDATE(ManaCost_0)
}
void CHero::SetManaCost1(float fCost) {
    this->ManaCost_1 = fCost;
    HQUEUE_UPDATE(ManaCost_1)
}
void CHero::SetManaCost2(float fCost) {
    this->ManaCost_2 = fCost;
    HQUEUE_UPDATE(ManaCost_2)
}
void CHero::SetManaCost3(float fCost) {
    this->ManaCost_3 = fCost;
    HQUEUE_UPDATE(ManaCost_3)
}
//ManaCost_0 to ManaCost_Ex15
void CHero::SetActionState(DWORD dwActionState) {
    this->ActionState = dwActionState;
    HQUEUE_UPDATE(ActionState)
}
void CHero::SetMagicImmune(DWORD bMagicImmune) {
    this->MagicImmune = bMagicImmune;
    HQUEUE_UPDATE(MagicImmune)
}
void CHero::SetIsInvulnerable(DWORD bIsInvulnerable) {
    this->IsInvulnerable = bIsInvulnerable;
    HQUEUE_UPDATE(IsInvulnerable)
}
void CHero::SetIsPhysicalImmune(DWORD bIsPhysicalImmune) {
    this->IsPhysicalImmune = bIsPhysicalImmune;
    HQUEUE_UPDATE(IsPhysicalImmune)
}
void CHero::SetIsLifestealImmune(DWORD bIsLifeStealImmune) {
    this->IsLifestealImmune = bIsLifeStealImmune;
    HQUEUE_UPDATE(IsLifestealImmune)
}
void CHero::SetBaseAttackDamage(float fBaseAttackDamage) {
    this->mBaseAttackDamage = fBaseAttackDamage;
    HQUEUE_UPDATE(mBaseAttackDamage)
}
void CHero::SetBaseAbilityDamage(float fBaseAbilityDamage) {
    this->mBaseAbilityDamage = fBaseAbilityDamage;
    HQUEUE_UPDATE(mBaseAbilityDamage)
}
void CHero::SetDodge(float fDodge) {
    this->mDodge = fDodge;
    HQUEUE_UPDATE(mDodge)
}
void CHero::SetCrit(float fCrit) {
    this->mCrit = fCrit;
    HQUEUE_UPDATE(mCrit)
}
void CHero::SetCrit2(float fCrit2) {
    this->mCrit2 = fCrit2;
    HQUEUE_UPDATE(mCrit2)
}
void CHero::SetArmor(float fArmor) {
    this->mArmor = fArmor;
    HQUEUE_UPDATE(mArmor)
}
void CHero::SetSpellBlock(float fSpellBlock) {
    this->mSpellBlock = fSpellBlock;
    HQUEUE_UPDATE(mSpellBlock)
}
void CHero::SetHPRegenRate(float fHPRegenRate) {
    this->mHPRegenRate = fHPRegenRate;
    HQUEUE_UPDATE(mHPRegenRate)
}
void CHero::SetPARRegenRate(float fPARRegenRate) {
    this->mPARRegenRate = fPARRegenRate;
    HQUEUE_UPDATE(mPARRegenRate)
}
void CHero::SetAttackRange(float fAttackRange) {
    this->mAttackRange = fAttackRange;
    HQUEUE_UPDATE(mAttackRange)
}
void CHero::SetAttackSpeedMod(float fAttackSpeedMod) {
    this->mAttackSpeedMod = fAttackSpeedMod;
    HQUEUE_UPDATE(mAttackSpeedMod)
}
void CHero::SetFlatPhysicalDamageMod(float fFlatPhysicalDamageMod) {
    this->mFlatPhysicalDamageMod = fFlatPhysicalDamageMod;
    HQUEUE_UPDATE(mFlatPhysicalDamageMod)
}
void CHero::SetPercentPhysicalDamageMod(float fPercentPhysicalDamageMod) {
    this->mPercentPhysicalDamageMod = fPercentPhysicalDamageMod;
    HQUEUE_UPDATE(mPercentPhysicalDamageMod)
}
void CHero::SetFlatMagicDamageMod(float fFlatMagicDamageMod) {
    this->mFlatMagicDamageMod = fFlatMagicDamageMod;
    HQUEUE_UPDATE(mFlatMagicDamageMod)
}
void CHero::SetFlatMagicReduction(float fFlatMagicReduction) {
    this->mFlatMagicReduction = fFlatMagicReduction;
    HQUEUE_UPDATE(mFlatMagicReduction)
}
void CHero::SetPercentMagicReduction(float fPercentMagicReduction) {
    this->mPercentMagicReduction = fPercentMagicReduction;
    HQUEUE_UPDATE(mPercentMagicReduction)
}
void CHero::SetFlatCastRangeMod(float fFlatCastRangeMod) {
    this->mFlatCastRangeMod = fFlatCastRangeMod;
    HQUEUE_UPDATE(mFlatCastRangeMod)
}
void CHero::SetPercentCooldownMod(float fPercentCooldownMod) {
    this->mPercentCooldownMod = fPercentCooldownMod;
    HQUEUE_UPDATE(mPercentCooldownMod)
}
void CHero::SetPassiveCooldownEndTime(float fPassiveCooldownEndTime) {
    this->mPassiveCooldownEndTime = fPassiveCooldownEndTime;
    HQUEUE_UPDATE(mPassiveCooldownEndTime)
}
void CHero::SetPassiveCooldownTotalTime(float fPassiveCooldownTotalTime) {
    this->mPassiveCooldownTotalTime = fPassiveCooldownTotalTime;
    HQUEUE_UPDATE(mPassiveCooldownTotalTime)
}
void CHero::SetFlatArmorPenetration(float fFlatArmorPenetration) {
    this->mFlatArmorPenetration = fFlatArmorPenetration;
    HQUEUE_UPDATE(mFlatArmorPenetration)
}
void CHero::SetPercentArmorPenetration(float fPercentArmorPenetration) {
    this->mPercentArmorPenetration = fPercentArmorPenetration;
    HQUEUE_UPDATE(mPercentArmorPenetration)
}
void CHero::SetFlatMagicPenetration(float fFlatMagicPenetration) {
    this->mFlatMagicPenetration = fFlatMagicPenetration;
    HQUEUE_UPDATE(mFlatMagicPenetration)
}
void CHero::SetPercentMagicPenetration(float fPercentMagicPenetration) {
    this->mPercentMagicPenetration = fPercentMagicPenetration;
    HQUEUE_UPDATE(mPercentMagicPenetration)
}
void CHero::SetPercentLifeStealMod(float fPercentLifeStealMod) {
    this->mPercentLifeStealMod = fPercentLifeStealMod;
    HQUEUE_UPDATE(mPercentLifeStealMod)
}
void CHero::SetPercentSpellVampMod(float fPercentSpellVampMod) {
    this->mPercentSpellVampMod = fPercentSpellVampMod;
    HQUEUE_UPDATE(mPercentSpellVampMod)
}
void CHero::SetPercentCCReduction(float fPercentCCReduction) {
    this->mPercentCCReduction = fPercentCCReduction;
    HQUEUE_UPDATE(mPercentCCReduction)
}
void CHero::SetHP(float fHP) {
    this->mHP = fHP;
    HQUEUE_UPDATE(mHP)
}
void CHero::SetMP(float fMP) {
    this->mMP = fMP;
    HQUEUE_UPDATE(mMP)
}
void CHero::SetMaxHP(float fMaxHP) {
    this->mMaxHP = fMaxHP;
    HQUEUE_UPDATE(mMaxHP)
}
void CHero::SetMaxMP(float fMaxMP) {
    this->mMaxMP = fMaxMP;
    HQUEUE_UPDATE(mMaxMP)
}
void CHero::SetExp(float fExp) {
    this->mExp = fExp;
    HQUEUE_UPDATE(mExp)
}
void CHero::SetLifetime(float fLifetime) {
    this->mLifetime = fLifetime;
    HQUEUE_UPDATE(mLifetime)
}
void CHero::SetMaxLifetime(float fMaxLifetime) {
    this->mMaxLifetime = fMaxLifetime;
    HQUEUE_UPDATE(mMaxLifetime)
}
void CHero::SetLifetimeTicks(float fLifetimeTicks) {
    this->mLifetimeTicks = fLifetimeTicks;
    HQUEUE_UPDATE(mLifetimeTicks)
}
void CHero::SetPathfindingRadiusMod(float mPathfindingRadiusMod) {
    this->mPathfindingRadiusMod = mPathfindingRadiusMod;
    HQUEUE_UPDATE(mPathfindingRadiusMod)
}
void CHero::SetLevelRef(DWORD dwLevelRef) {
    this->mLevelRef = dwLevelRef;
    HQUEUE_UPDATE(mLevelRef)
}
void CHero::SetNumNeutralMinionsKilled(DWORD dwNumNeutralMinionsKilled) {
    this->mNumNeutralMinionsKilled = dwNumNeutralMinionsKilled;
    HQUEUE_UPDATE(mNumNeutralMinionsKilled)
}
void CHero::SetFlatBubbleRadiusMod(float fFlatBubbleRadiusMod) {
    this->mFlatBubbleRadiusMod = fFlatBubbleRadiusMod;
    HQUEUE_UPDATE(mFlatBubbleRadiusMod)
}
void CHero::SetPercentBubbleRadiusMod(float fPercentBubbleRadiusMod) {
    this->mPercentBubbleRadiusMod = fPercentBubbleRadiusMod;
    HQUEUE_UPDATE(mPercentBubbleRadiusMod)
}
void CHero::SetMoveSpeed(float fMoveSpeed) {
    this->SetSpeed(fMoveSpeed);
    HQUEUE_UPDATE_EX(mMoveSpeed, fMoveSpeed);
}
void CHero::SetIsTargetable(DWORD dwIsTargetable) {
    this->mIsTargetable = dwIsTargetable;
    HQUEUE_UPDATE(mIsTargetable)
}
void CHero::SetIsTargetableToTeamFlags(DWORD dwIsTargetableToTeamFlags) {
    this->mIsTargetableToTeamFlags = dwIsTargetableToTeamFlags;
    HQUEUE_UPDATE(mIsTargetableToTeamFlags)
}

float CHero::GetCriticalChance() {
    //mCrit * CritDamageBonus
    return .0f;
}

void CHero::AddGold(float fGold) {
    SetGold(mGold + fGold);
}

DWORD CHero::_CastSpell(DWORD iIndex, float fX, float fY) {
    return CastSpell(iIndex, fX, fY);
}

float CHero::GetCollisionDistance(CHero *h) {
    if(h == NULL) {
        return 0;
    }
    return sqrt(pow(GetX() - h->GetX(), 2) + pow(GetY() - h->GetY(), 2)) - GetGameplayCollisionRadius() - h->GetGameplayCollisionRadius();
}

float CHero::GetDistance(CHero *h) {
    if(h == NULL) {
        return 0;
    }
    return sqrt(pow(GetX() - h->GetX(), 2) + pow(GetY() - h->GetY(), 2));
}

void CHero::Stop() {
    CMovable::Stop();
    MovementAnsPacket p(GetNetworkID(), { GetPos() });
    p.Broadcast();
}

std::string CHero::GetSpellName(UINT iIndex) {
    if(iIndex > 4) { return ""; }
    if(iIndex == 3) { return GetIniBin().GetVar("Data", "Spell4"); }
    if(iIndex == 2) { return GetIniBin().GetVar("Data", "Spell3"); }
    if(iIndex == 1) { return GetIniBin().GetVar("Data", "Spell2"); }
    return GetIniBin().GetVar("Data", "Spell1");
}

CSpell *CHero::GetSpell(UINT iIndex) {
    return iIndex < 4 ? &spells[iIndex] : 0;
}

unsigned long long CHero::GetUserID() {
    return userId;
}

bool CHero::IsMorphed() {
    return strcmp(szType, szModel) != 0;
}

void CHero::SetSpawn(BYTE spawnIndex, Vector3f fPos) {
    this->spawnIndex = spawnIndex;
    this->spawnX = fPos.x;
    this->spawnY = fPos.z;
    SetPos(Vector2f(fPos.x, fPos.z));
}

void CHero::SetPos(Vector2f fPos) {
    std::vector<Vector2f> vPos;
    vPos.push_back(fPos);
    SetWaypoints(vPos);
}

void CHero::DoSpellWork() {
    if(!vSpells.empty()) {
        vSpells[0].DoWork();
    }
}

void CHero::DoSpellCast() {
    if(!vSpells.empty()) {
        vSpells[0].DoCast();
    }
}

void CHero::PopSpell() {
    if(!vSpells.empty()) {
        vSpells.erase(vSpells.begin());
    }
}

void CHero::QueueSpell(CSpellWork work) {
    if(work.fManaCost > mMP) {  //not enough mana for spell
        return;
    }
    work.owner = this;
    vSpells.push_back(work);
    if(vSpells.size() == 1) {
        DoSpellCast();
    }
}

void CHero::SetLevel(DWORD iLevel) {
    SetLevelRef(iLevel);
}

DWORD CHero::GetLevel() {
    return mLevelRef;
}

ENetPeer *CHero::GetPeer() {
    return (ENetPeer *)_peer;
}

CHero::CHero(WORD index, char *szName, char *szHero, Team tTeam, DWORD skinId) : CUnit(index, szName, szHero, tTeam) {
    ZeroMemory(&_peer, sizeof(CHero) - sizeof(CUnit));
    this->mSkinId = skinId;
    inv = CInventory();
    buffManager = CBuffManager(this);
    SetLevel(1);
    iSpellPoints = 5;
    for(UINT i = 0; i < 4; i++) {
        spells[i] = CSpell(this, i, 0);
    }
    spells[3].iLevel = 1;
    //..
    aaWork = nullptr;
    aaTarget = nullptr;
    //BitMaks
    bCanAttack = TRUE;
    bCanCast = TRUE;
    bCanMove = TRUE;
    mGold = 475.0f;
    Morph(szHero);
}

Team CHero::GetTeam() {
    return tTeam;
}

Team CHero::GetEnemyTeam() {
    return tTeam == BLUE ? RED : BLUE;
}

void CItemSlot::Swap(CItemSlot *s2) {
    DWORD ID = this->ID;
    DWORD nStacks = this->nStacks;
    float fCooldown = this->fCooldown;
    //copy s2 to this
    this->ID = s2->ID;
    this->nStacks = s2->nStacks;
    this->fCooldown = s2->fCooldown;
    //copy this to to
    s2->ID = ID;
    s2->nStacks = nStacks;
    s2->fCooldown = fCooldown;
}

CItemSlot::operator int() {
    return ID;
}

CItemSlot::CItemSlot(DWORD itemId) : CItemSlot() {
    ID = itemId;
}

CItemSlot::CItemSlot() {
    ZeroMemory(this, sizeof(CItemSlot) - 1);
    nStacks = 1;
}

int CInventory::GetItemCount() {
    int nItems = 0;
    for(UINT i = 0; i < 6; i++) {
        if(vSlots[i] > 0) {
            nItems++;
        }
    }
    return nItems;
}

CItemSlot *CInventory::GetFreeSlot() {
    for(UINT i = 0; i < 6; i++) {
        if(vSlots[i] == 0) {
            return &vSlots[i];
        }
    }
    return NULL;
}

CItemSlot *CInventory::GetItemById(DWORD itemId) {
    for(UINT i = 0; i < 7; i++) {
        if(vSlots[i] == itemId) {
            return &vSlots[i];
        }
    }
    return NULL;
}

CItemSlot *CInventory::GetSlot(DWORD iIndex) {
    if(iIndex > 5) {
        return NULL;
    }
    return &vSlots[iIndex];
}

CItemSlot *CInventory::GetTrinket() {
    return &vSlots[6];
}

CInventory::CInventory() {
    for(UINT i = 0; i < 7; i++) {
        vSlots[i] = CItemSlot();
        vSlots[i].iSlot = i;
    }
}
