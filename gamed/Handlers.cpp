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
#include "PacketHandler.h"
#include "NetworkListener.h"
#include "UpdateStatsPacket.h"
#include "ObjectAttributes.h"
#include "MovementReqPacket.h"
#include "MovementAnsPacket.h"
#include "Server.h"
#include "Client.h"
#include "ChatBox.h"
#include "Packets.h"

bool PacketHandler::handleNull(HANDLE_ARGS) {
    return true;
}

void SendKeys(PacketHandler *_handler, ENetPeer *peer) {
    //Send current peers
    for(auto p : CObjectManager::GetPlayers()) {
        if(p->GetPeer()) {
            KeyCheck response(p->playerIndex, p->GetUserID(), p->dwKeyUnk);
            _handler->sendPacket(peer, reinterpret_cast<uint8 *>(&response), sizeof(KeyCheck), CHL_HANDSHAKE);
        }
    }
}

bool PacketHandler::handleKeyCheck(ENetPeer *peer, ENetPacket *packet) {
    //MessageBoxA(0, "Got 00", "Got 00", 0);
    KeyCheck *keyCheck = reinterpret_cast<KeyCheck *>(packet->data);
    uint64 userId = _blowfish->Decrypt(*(uint64 *)&keyCheck->dwKeyCheckBegin);
    /*
    uint64 enc = _blowfish->Encrypt(keyCheck->userId);
    char buffer[255];
    unsigned char *p = (unsigned char*)&enc;
    for(int i = 0; i < 8; i++)
    {
    sprintf(&buffer[i*3], "%02X ", p[i]);
    }
    PDEBUG_LOG_LINE(Logging," Enc id: %s\n", buffer);*/
    if(userId == 0 || userId > CObjectManager::GetPlayers().size()) {
        return false;
    }
    CHero *hero = CObjectManager::GetPlayer(userId - 1);
    if(hero->GetPeer() != NULL) {  // maybe give him next free player?
        Logging->errorLine(" PLAYER ALREADY CONNECTED, GTFO!!!\n");
        return false;
    }
    if(hero && userId == keyCheck->userId) {
        PDEBUG_LOG_LINE(Logging, "[%08X] User got the same key as i do, go on!\n", userId);
        peerInfo(peer)->keyChecked = true;
        peerInfo(peer)->userId = userId;
        SendKeys(this, peer);
        //try sending twice
        peerInfo(peer)->setPlayer(hero);
        peerUnit(peer)->userId = userId;
        peerUnit(peer)->dwKeyUnk = keyCheck->dwKeyUnk;
    } else {
        Logging->errorLine(" WRONG KEY, GTFO!!!\n");
        return false;
    }
    //Send response as this is correct (OFC DO SOME ID CHECKS HERE!!!)
    KeyCheck response(hero->playerIndex, hero->GetUserID(), hero->dwKeyUnk);
    bool bRet = broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(KeyCheck), CHL_HANDSHAKE);
    handleGameNumber(peer, nullptr);//Send 0x91 Packet?
    return bRet;
}

bool PacketHandler::handleGameNumber(ENetPeer *peer, ENetPacket *packet) {
    WorldSendGameNumber world(1, "EUW1", peerUnit(peer)->szName);
    return sendPacket(peer, reinterpret_cast<uint8 *>(&world), sizeof(WorldSendGameNumber), CHL_S2C);
}

bool PacketHandler::handleSynch(ENetPeer *peer, ENetPacket *packet) {
    SynchVersion *version = reinterpret_cast<SynchVersion *>(packet->data);
    Logging->writeLine("Client version: %s\n", version->version);
    SynchVersionAns answer(1, "Version 4.6.0.188 [PUBLIC]", "CLASSIC");
    int iIndex[2] = { 0, 6 };
    for(auto h : CObjectManager::GetPlayers()) {
        int i = h->tTeam == BLUE ? iIndex[0]++ : iIndex[1]++;
        answer.players[i].userId = h->GetUserID();
        answer.players[i].teamId = h->tTeam;
        answer.players[i].skill1 = SPL_Ignite;
        answer.players[i].skill2 = SPL_Flash;
        if(h->GetPeer()) {  //peer
        } else { //bot
            //answer.players[i].bBot = true;
            //answer.players[i].userId = -1;
            //answer.players[i].wUnk = 1;
            strcpy((char *)answer.players[i].name, h->szModel);
            strcpy((char *)answer.players[i].type, h->szModel);
        }
    }
    return sendPacket(peer, reinterpret_cast<uint8 *>(&answer), sizeof(SynchVersionAns), 3);
}

bool PacketHandler::handleMap(ENetPeer *peer, ENetPacket *packet) {
    //Builds team info
    LoadScreenInfo screenInfo;
    screenInfo.bluePlayerNo = CObjectManager::GetPlayers(BLUE).size();
    screenInfo.redPlayerNo = CObjectManager::GetPlayers(RED).size();
    //For all players send this info
    int iBlue = 0;
    int iRed = 0;
    for(auto p : CObjectManager::GetPlayers()) {
        if(p->tTeam == BLUE) {
            screenInfo.bluePlayerIds[iBlue++] = p->GetUserID();
        } else {
            screenInfo.redPlayersIds[iRed++] = p->GetUserID();
        }
    }
    bool pInfo = sendPacket(peer, reinterpret_cast<uint8 *>(&screenInfo), sizeof(LoadScreenInfo), CHL_LOADING_SCREEN);
    for(auto p : CObjectManager::GetPlayers()) {
        if(p->GetNetworkID() != peerInfo(peer)->unit->GetNetworkID()) {
            //continue;
        }
        //setup
        LoadScreenPlayer *playerName = LoadScreenPlayer::create(PKT_S2C_LoadName, (int8 *)p->szName, strlen(p->szName));
        playerName->userId = p->GetUserID();
        LoadScreenPlayer *playerHero = LoadScreenPlayer::create(PKT_S2C_LoadHero, (int8 *)p->szModel, strlen(p->szModel));
        playerHero->userId = p->GetUserID();
        playerHero->skinId = p->mSkinId;
        //send
        sendPacket(peer, reinterpret_cast<uint8 *>(playerName), playerName->getPacketLength(), CHL_LOADING_SCREEN);
        sendPacket(peer, reinterpret_cast<uint8 *>(playerHero), playerHero->getPacketLength(), CHL_LOADING_SCREEN);
        //cleanup
        LoadScreenPlayer::destroy(playerName);
        LoadScreenPlayer::destroy(playerHero);
    }
    /*server->listener->_handler->Flush();
    //flush
    int iIndex = 0;
    for(auto p : CObjectManager::GetPlayers()) {
        if(p->GetNetworkID() != peerInfo(peer)->unit->GetNetworkID()) {
            KeyCheck response;
            response.playerNo = iIndex;
            response.userId = p->GetUserID();
            sendPacket(peer, reinterpret_cast<uint8 *>(&response), sizeof(KeyCheck), CHL_LOADING_SCREEN);
        }
        iIndex++;
    }*/
    return pInfo;
}
#include "ObjectManager.h"
void PacketHandler::ReleaseTowers() {
    bool bFirst = true;
    for(int i = 0; i < 3; i++) {
        bFirst = true;
        for(auto t : server->blueTeam->lanes[i].vUnits) {
            t->SetIsTargetable(TRUE);
            if(bFirst) {
                t->SetIsInvulnerable(TRUE);
                t->SetIsTargetableToTeamFlags(0x02000000);
            } else {
                t->SetIsTargetableToTeamFlags(0x01000000);
            }
            bFirst = false;
        }
    }
    for(int i = 0; i < 3; i++) {
        bFirst = true;
        for(auto t : server->redTeam->lanes[i].vUnits) {
            t->SetIsTargetable(TRUE);
            t->SetIsTargetableToTeamFlags(bFirst ? 0x02000000 : 0x01000000);
            bFirst = false;
        }
    }
    UpdateStatsPacket::SendQueue();
}
void PacketHandler::InitTowers(ENetPeer *peer) {
    for(auto t : CObjectManager::GetTowers(peerUnit(peer)->tTeam)) {  //vision range packet // Only for custom mode?
        if(t->GetRange()) {
            VisionRange range(t->GetNetworkID(), t->GetActor()->GetNetworkID(), t->tTeam, t->GetX(), t->GetY(), t->GetRange());
            sendPacket(peer, reinterpret_cast<uint8 *>(&range), sizeof(range), CHL_S2C);
        }
    }
    //...
    //apply buff to tower 40000011/12
    //apply particle for towers
    //SetStats for #1 - FFF97DB5 to #14 - FFB77171 and run B9 / AD (SpawnPos/SpawnHp)
    //vision spawn for tower 40000011/12
    //...
    for(auto t : CObjectManager::GetTowers(BLUE)) {
        BuyItemAns item1(t->GetNetworkID(), 1500, 0, 1);
        item1.unk2 = 0x4000;
        BuyItemAns item2(t->GetNetworkID(), 1502, 1, 1);
        item2.unk2 = 0x4000;
        BuyItemAns item3(t->GetNetworkID(), 1503, 2, 1);
        item3.unk2 = 0x4000;
        sendPacket(peer, reinterpret_cast<uint8 *>(&item1), sizeof(BuyItemAns), CHL_S2C);
        sendPacket(peer, reinterpret_cast<uint8 *>(&item2), sizeof(BuyItemAns), CHL_S2C);
        sendPacket(peer, reinterpret_cast<uint8 *>(&item3), sizeof(BuyItemAns), CHL_S2C);
    }
    std::unordered_map<uint32, std::unordered_map<ObjectAttributeIndex, double>> updates;
    std::unordered_map<ObjectAttributeIndex, double> attr1;
    attr1[Summoners_Rift::obj_HQ::mHP] = 5500;
    attr1[Summoners_Rift::obj_HQ::IsInvulnerable] = 1;
    attr1[Summoners_Rift::obj_HQ::mIsTargetable] = 0;
    attr1[Summoners_Rift::obj_HQ::mIsTargetableToTeamFlags] = 0x01800000;
    for(auto id : { 0xF97DB5, 0xF02C0F }) {
        updates[0xFF000000 | id] = attr1;
    }
    //
    std::unordered_map<ObjectAttributeIndex, double> attr2;
    attr2[Summoners_Rift::obj_BarracksDampener::mHP] = 4000;
    attr2[Summoners_Rift::obj_BarracksDampener::IsInvulnerable] = 1;
    attr2[Summoners_Rift::obj_BarracksDampener::mIsTargetable] = 0;
    attr2[Summoners_Rift::obj_BarracksDampener::mIsTargetableToTeamFlags] = 0x01800000;
    for(auto id : { 0x9303E1, 0x4A20F1, 0xD23C3E, 0xFF8F1F, 0x26AC0F, 0x6793D0 }) {
        updates[0xFF000000 | id] = attr2;
    }
    //
    std::unordered_map<ObjectAttributeIndex, double> attr3;
    attr3[Summoners_Rift::obj_Barracks::mHP] = 100000000;
    attr3[Summoners_Rift::obj_Barracks::IsInvulnerable] = 1;
    attr3[Summoners_Rift::obj_Barracks::mIsTargetable] = 0;
    attr3[Summoners_Rift::obj_Barracks::mIsTargetableToTeamFlags] = 0x01800000;
    for(auto id : { 0x5EC9AF0, 0xBA00E8, 0x53B836, 0xEB364C, 0xB77171, 0xE647D5 }) {
        updates[0xFF000000 | id] = attr3;
    }
    UpdateStatsPacket updatePacket(updates);
    sendPacket(peer, updatePacket);
    for(auto t : CObjectManager::GetTowers()) {
        t->SetMaxMP(0);
        t->SetMP(0);
        t->SetActionState(0x00420007);
        t->SetMagicImmune(false);
        t->SetIsInvulnerable(true);
        t->SetIsPhysicalImmune(false);
        t->SetIsLifestealImmune(true);
        t->SetBaseAttackDamage(25.0);
        t->SetArmor(0);
        t->SetSpellBlock(0);
        t->SetAttackSpeedMod(1.0);
        t->SetFlatPhysicalDamageMod(0);
        t->SetPercentPhysicalDamageMod(0);
        t->SetFlatMagicDamageMod(0);
        t->SetHPRegenRate(0);
        t->SetHP(1300.0);
        t->SetMaxHP(1300.0);
        t->SetFlatBubbleRadiusMod(0);
        t->SetPercentBubbleRadiusMod(0);
        t->SetMoveSpeed(0);
        t->SetCrit(1.0);
        t->SetIsTargetable(0);
        t->SetIsTargetableToTeamFlags(0x01800000);
    }
    UpdateStatsPacket::SendQueue();
}
//building the map
bool PacketHandler::handleSpawn(ENetPeer *peer, ENetPacket *packet) {
    CHero *p = peerInfo(peer)->unit;
    //save peer as loaded and wait for others to finish?
    // or game is already started?
    if(server->fGameStart == 0) {
        server->fGameStart = GetTickCount(); //start game timer
    }
    StatePacket2 start(PKT_S2C_StartSpawn);
    bool p1 = sendPacket(peer, reinterpret_cast<uint8 *>(&start), sizeof(StatePacket2), CHL_S2C);
    printf("Spawning map\r\n");
    //..
    for(auto h : CObjectManager::GetPlayers()) {
        HeroSpawn spawn(h->GetNetworkID(), h->playerIndex, h->tTeam == BLUE, false, h->spawnIndex, h->mSkinId, h->szName, h->szType);
        //spawn.unk2 = 0;
        //spawn.unk5 = 0x71;
        sendPacket(peer, reinterpret_cast<uint8 *>(&spawn), sizeof(HeroSpawn), CHL_S2C);
        if(h == p) {
            for(int i = 0; i < 4; i++) {           //spells
                SetupSpell1 spell1(h->GetNetworkID(), i);
                SetupSpell2 spell2(h->GetNetworkID(), i);
                sendPacket(peer, reinterpret_cast<uint8 *>(&spell1), sizeof(SetupSpell1), CHL_S2C);
                sendPacket(peer, reinterpret_cast<uint8 *>(&spell2), sizeof(SetupSpell2), CHL_S2C);
            }
        }
        //0x29 here -- Masteries
        InitSpawn initSpawn(h->GetNetworkID());
        sendPacket(peer, reinterpret_cast<uint8 *>(&initSpawn), sizeof(InitSpawn), CHL_S2C);
    }
    //..
    bool p2 = true;
    HeroSpawn3 h2(p->GetNetworkID(), p->spawnX, p->spawnY, 0, 0);
    sendPacket(peer, reinterpret_cast<uint8 *>(&h2), sizeof(HeroSpawn3), CHL_S2C);
    SpawnHealthPoints h3(p->GetNetworkID(), 666, 1337);
    sendPacket(peer, reinterpret_cast<uint8 *>(&h3), sizeof(SpawnHealthPoints), CHL_S2C);
    for(auto t : CObjectManager::GetTowers()) {
        TurretSpawn turretSpawn(t->GetNetworkID(), t->szName);
        sendPacket(peer, reinterpret_cast<uint8 *>(&turretSpawn), sizeof(TurretSpawn), CHL_S2C);
    }
    //Spawn Props
    for(auto prop : CObjectManager::GetProps()) {
        if(prop == NULL) { continue; }
        LevelPropSpawn lpSpawn;
        lpSpawn.SetProp(const_cast<char *>(prop->szName), const_cast<char *>(prop->szModel), prop->x, prop->y, prop->z);
        lpSpawn.header.netId = 0;
        lpSpawn.netId = prop->GetNetworkID();
        //lpSpawn.x = 12465; lpSpawn.y = 101;
        sendPacket(peer, reinterpret_cast<uint8 *>(&lpSpawn), sizeof(LevelPropSpawn), CHL_S2C);
    }
    //end spawn packet
    StatePacket end(PKT_S2C_EndSpawn);
    bool p3 = sendPacket(peer, reinterpret_cast<uint8 *>(&end), sizeof(StatePacket), CHL_S2C);
    //recall item slot
    BuyItemAns recall(p->GetNetworkID(), 2001, 7, 1);
    bool p4 = sendPacket(peer, reinterpret_cast<uint8 *>(&recall), sizeof(BuyItemAns), CHL_S2C); //activate recall slot
    char _0x5B[6] = { 0x5B, 0, 0, 0, 0, 0 }; //0x5B now
    sendPacket(peer, reinterpret_cast<uint8 *>(_0x5B), sizeof(_0x5B), CHL_S2C);
    //Only for my team
    bool bFirst = true;
    for(auto t : CObjectManager::GetTowers(BLUE)) {
        TurretSpawn2 turretSpawn(t->GetNetworkID());
        SpawnHealthPoints h3(t->GetNetworkID(), 1300, 1300);
        sendPacket(peer, reinterpret_cast<uint8 *>(&turretSpawn), sizeof(TurretSpawn2), CHL_S2C);
        sendPacket(peer, reinterpret_cast<uint8 *>(&h3), sizeof(SpawnHealthPoints), CHL_S2C);
        if(bFirst) {
            char _0xEF[6] = { 0xEF, 0, 0, 0, 0, 1 }; //0xEF now
            *reinterpret_cast<DWORD *>(&_0xEF[1]) = p->GetNetworkID();
            sendPacket(peer, reinterpret_cast<uint8 *>(_0xEF), sizeof(_0xEF), CHL_S2C);
            UINT iIndex = 1;
            for(auto h : CObjectManager::GetPlayers()) {
                if(h->GetNetworkID() != p->GetNetworkID()) {
                    HeroSpawn4 h2(h->GetNetworkID(), h->spawnX, h->spawnY, 0, 0); //this is for spawning vision of the player
                    //printf("%s - %f\r\n", h->szName, h->getRotation() / 360.0f);
                    sendPacket(peer, reinterpret_cast<uint8 *>(&h2), sizeof(HeroSpawn4), CHL_S2C);
                    SpawnHealthPoints h3(h->GetNetworkID(), h->mHP, h->mMaxHP);
                    sendPacket(peer, reinterpret_cast<uint8 *>(&h3), sizeof(SpawnHealthPoints), CHL_S2C);
                    iIndex++;
                }
            }
            bFirst = false;
        }
    }
    GameTimer timer(0); //0xC0
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer), sizeof(GameTimer), CHL_S2C);
    InitTowers(peer);
    //update attributes of player
    p->SendHeroStats(); //show attribute of other players too
    GameTimer timer2(server->GetGameTime()); //0xC0
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer2), sizeof(GameTimer), CHL_S2C);
    GameTimerUpdate timer3(server->GetGameTime()); //0xC1
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer3), sizeof(GameTimerUpdate), CHL_S2C);
    p->SpellInit();
    //chapion spell book - BUFF NOT SPELLBOOK U DUMBASS
    /*SpellBook sBook(p->GetNetworkID());
    sBook.bSpell[0] = 1;
    sBook.bSpell[1] = 1;
    sBook.bSpell[2] = 1;
    sBook.Sum1 = 0x0049A905;
    sBook.Sum2 = 0xF293BF45;
    sendPacket(peer, reinterpret_cast<uint8 *>(&sBook), sizeof(SpellBook), CHL_S2C);
    //maybe only for elise spider form?
    SpellBook sBook2(p->GetNetworkID());
    sBook2.bSpell[0] = 2;
    sBook2.bSpell[1] = 1;
    sBook2.bSpell[2] = 2;
    sBook2.Sum1 = 0x04FD5139;
    sBook2.Sum2 = 0xF293BF45;
    sendPacket(peer, reinterpret_cast<uint8 *>(&sBook2), sizeof(SpellBook), CHL_S2C);*/
    //86 here?
    return true;// p1 & p2 & p3;
}
bool PacketHandler::handleStartGame(HANDLE_ARGS) {
    StatePacket start(PKT_S2C_StartGame);
    sendPacket(peer, reinterpret_cast<uint8 *>(&start), sizeof(StatePacket), CHL_S2C);
    FogUpdate2 test;
    test.x = 0;
    test.y = 0;
    test.radius = 1;
    test.unk1 = 2;
    //uint8 p[] = {0xC5, 0x19, 0x00, 0x00, 0x40, 0x00, 0x00, 0x50};
    //sendPacket(peer, reinterpret_cast<uint8*>(p), sizeof(p), 3);
    sendPacket(peer, reinterpret_cast<uint8 *>(&test), sizeof(FogUpdate2), 3);
    //playing around 8-)
    /*
    CharacterStats movement;
    movement.netId = peerInfo(peer)->netId;
    movement.statType = STI_Movement;
    movement.statValue = 800;
    sendPacket(peer,reinterpret_cast<uint8*>(&movement),sizeof(movement), 4);*/
    return true;
}
bool PacketHandler::handleAttentionPing(ENetPeer *peer, ENetPacket *packet) {
    AttentionPing *ping = reinterpret_cast<AttentionPing *>(packet->data);
    AttentionPingAns response(ping);
    Logging->writeLine("Plong x: %f, y: %f, z: %f, type: %i\n", ping->x, ping->y, ping->z, ping->type);
    return broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(AttentionPing), 3);
}
bool PacketHandler::handleView(ENetPeer *peer, ENetPacket *packet) {
    ViewReq *request = reinterpret_cast<ViewReq *>(packet->data);
    //Logging->writeLine("View (%i), x:%f, y:%f, zoom: %f\n", request->requestNo, request->x, request->y, request->zoom);
    ViewAns answer;
    answer.requestNo = request->requestNo;
    sendPacket(peer, reinterpret_cast<uint8 *>(&answer), sizeof(ViewAns), CHL_S2C, UNRELIABLE);
    enet_host_flush(peer->host);
    if(request->requestNo == 0xFE) {
        answer.requestNo = 0xFF;
        sendPacket(peer, reinterpret_cast<uint8 *>(&answer), sizeof(ViewAns), CHL_S2C, UNRELIABLE);
    }
    return true;
}
bool PacketHandler::handleMove(ENetPeer *peer, ENetPacket *packet) {
    MovementReqPacket request(packet->data, packet->dataLength);
    /*int i = 0;
    for(auto waypoint : request.getVertices()) {
        printf("     Vector %i, x: %i, y: %i\n", i++, waypoint.x, waypoint.y);
    }*/
    DWORD targetId = request.getTargetID();
    switch(request.getMovementType()) {
        //TODO, Implement stop commands
        case STOP:
            Logging->writeLine("Move stop\n");
            break;//return true;
        case EMOTE: //Never seen it, emotes don't go here?
            Logging->writeLine("Emotion\n");
            break; //return true;
        case ATTACK_MOVE:
            //Logging->writeLine("Attack Move\n");
            targetId = peerUnit(peer)->GetClosestAATargetId();//set targetId to closest valid target - //lock target on closes target
            break;
        case MOVE:
            break;
        default:
            break;
    }
    Vector2f pos(request.getX(), request.getZ());
    peerUnit(peer)->HandleMoveRequest(pos, targetId);
    return true;
}
bool PacketHandler::handleLoadPing(ENetPeer *peer, ENetPacket *packet) {
    PingLoadInfo *loadInfo = reinterpret_cast<PingLoadInfo *>(packet->data);
    PingLoadInfo response;
    memcpy(&response, packet->data, sizeof(PingLoadInfo));
    response.header.cmd = PKT_S2C_Ping_Load_Info;
    response.userId = peerInfo(peer)->userId;
    Logging->writeLine("loaded: %f, ping: %f, %f\n", loadInfo->loaded, loadInfo->ping, loadInfo->f3);
    bool bRet = broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(PingLoadInfo), CHL_LOW_PRIORITY, UNRELIABLE);
    if(!peerInfo(peer)->bLoadScreen) {
        handleMap(peer, nullptr);
        peerInfo(peer)->bLoadScreen = true;
    }
    return bRet;
}
bool PacketHandler::handleQueryStatus(HANDLE_ARGS) {
    QueryStatus response;
    return sendPacket(peer, reinterpret_cast<uint8 *>(&response), sizeof(QueryStatus), CHL_S2C);
}
bool PacketHandler::handleChatBoxMessage(HANDLE_ARGS) {
    CHero *hero = (CHero *)peerInfo(peer)->unit;
    ChatMessage *message = reinterpret_cast<ChatMessage *>(packet->data);
    bool handled = false;
    //Lets do commands
    if(message->msg == '.') {
        const char *cmd[] = { ".set", ".gold", ".speed", ".health", ".xp", ".ap", ".ad", ".mana", ".model", ".level", ".restore", ".help" };
        //Set field
        if(strncmp(message->getMessage(), cmd[0], strlen(cmd[0])) == 0) {
            uint32 blockNo = atoi(&message->getMessage()[strlen(cmd[0]) + 1]);
            uint32 fieldNo = atoi(&message->getMessage()[strlen(cmd[0]) + 3]);
            float value = static_cast<double>(atoi(&message->getMessage()[strlen(cmd[0]) + 5]));
            UpdateStatsPacket::QueueUpdate(peerInfo(peer)->netId, { blockNo, fieldNo, FLOAT_VALUE }, value);
            Logging->writeLine("Setting to %f in block: %i, field: %i\n", value, blockNo, fieldNo);
            handled = true;
        }
        // Set Gold
        if(strncmp(message->getMessage(), cmd[1], strlen(cmd[1])) == 0) {
            float gold = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[1]) + 1]));
            hero->SetGold(gold);
            Logging->writeLine("Set gold to %f\n", gold);
            handled = true;
        }
        //movement
        if(strncmp(message->getMessage(), cmd[2], strlen(cmd[2])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[2]) + 1]));
            hero->SetMoveSpeed(value);
            Logging->writeLine("set champ speed to %f\n", value);
            handled = true;
        }
        //health
        if(strncmp(message->getMessage(), cmd[3], strlen(cmd[3])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[3]) + 1]));
            hero->SetHP(value);
            Logging->writeLine("set champ health to %f\n", value);
            handled = true;
        }
        //experience
        if(strncmp(message->getMessage(), cmd[4], strlen(cmd[4])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[4]) + 1]));
            hero->SetExp(value);
            Logging->writeLine("set champ exp to %f\n", value);
            handled = true;
        }
        //AbilityPower
        if(strncmp(message->getMessage(), cmd[5], strlen(cmd[5])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[5]) + 1]));
            hero->SetBaseAbilityDamage(value);
            Logging->writeLine("set champ abilityPower to %f\n", value);
            handled = true;
        }
        //Attack damage
        if(strncmp(message->getMessage(), cmd[6], strlen(cmd[6])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[6]) + 1]));
            hero->SetBaseAttackDamage(value);
            Logging->writeLine("set champ attack damage to %f\n", value);
            handled = true;
        }
        //Mana
        if(strncmp(message->getMessage(), cmd[7], strlen(cmd[7])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[7]) + 1]));
            hero->SetMP(value);
            Logging->writeLine("set champ mana to %f\n", value);
            handled = true;
        }
        //Model
        if(strncmp(message->getMessage(), cmd[8], strlen(cmd[8])) == 0) {
            std::string sModel = (char *)&message->getMessage()[strlen(cmd[8]) + 1];
            hero->Morph((char *)sModel.c_str());
            handled = true;
        }
        //level
        if(strncmp(message->getMessage(), cmd[9], strlen(cmd[9])) == 0) {
            float value = static_cast<float>(atoi(&message->getMessage()[strlen(cmd[9]) + 1]));
            hero->SetLevelRef(value);
            Logging->writeLine("set champ level to %f\n", value);
            handled = true;
        }
        //restore
        if(strncmp(message->getMessage(), cmd[10], strlen(cmd[10])) == 0) {
            hero->SetHP(hero->mMaxHP);
            hero->SetMP(hero->mMaxMP);
            Logging->writeLine("restored champ hp and mp %f\n");
            handled = true;
        }
        if(handled) {
            return true;
        }
    }
    switch(message->type) {
        case CMT_ALL:
            return broadcastPacket(packet->data, packet->dataLength, CHL_COMMUNICATION);
            break;
        case CMT_TEAM:
            return broadcastPacket(packet->data, packet->dataLength, CHL_COMMUNICATION);//sendToTeam !TODO make a team class and foreach player in the team send the message
            break;
        default:
            Logging->errorLine("Unknown ChatMessageType\n");
            return sendPacket(peer, packet->data, packet->dataLength, CHL_COMMUNICATION);
            break;
    }
    return false;
}
bool PacketHandler::handleSkillUp(HANDLE_ARGS) {
    SkillUpPacket *skillUpPacket = reinterpret_cast<SkillUpPacket *>(packet->data);
    UINT iSpell = skillUpPacket->skill;
    CHero *h = (CHero *)peerInfo(peer)->unit;
    bool p1 = h->LevelSkill(iSpell);
    //set manacost_0 and mCanCastBits1
    /*std::unordered_map<ObjectAttributeIndex, double> attributes;
    attributes[Summoners_Rift::obj_AI_Hero::ManaCost_0] = 28;
    attributes[Summoners_Rift::obj_AI_Hero::mCanCastBits1] = 0x801;
    std::unordered_map<uint32, std::unordered_map<ObjectAttributeIndex, double>> updates;
    updates[peerInfo(peer)->netId] = attributes;
    UpdateStatsPacket updatePacket(updates);
    sendPacket(peer, updatePacket);*/
    return p1;
}
bool PacketHandler::handleBuyItem(HANDLE_ARGS) {
    BuyItemReq *request = reinterpret_cast<BuyItemReq *>(packet->data);
    CHero *h = (CHero *)peerInfo(peer)->unit;
    printf("Buy Item: %i\r\n", request->id);
    return h->BuyItem(request->id);
}
bool PacketHandler::handleSellItem(HANDLE_ARGS) {
    _SellItemReq *request = reinterpret_cast<_SellItemReq *>(packet->data);
    CHero *h = (CHero *)peerInfo(peer)->unit;
    //printf("Sell Item: %i\r\n", request->iSlot & 0x7F);
    return h->SellItem(request->iSlot & 0x7F);
}
bool PacketHandler::handleSwapItem(HANDLE_ARGS) {
    _SwapItemReq *request = reinterpret_cast<_SwapItemReq *>(packet->data);
    CHero *h = (CHero *)peerInfo(peer)->unit;
    //printf("Swap Items: %i <-> %i\r\n", request->iSlot1, request->iSlot2);
    return h->SwapItem(request->iSlot1, request->iSlot2);
}
bool PacketHandler::handleEmotion(HANDLE_ARGS) {
    EmotionPacket *emotion = reinterpret_cast<EmotionPacket *>(packet->data);
    //for later use -> tracking, etc.
    switch(emotion->id) {
        case 0:
            //dance
            //Logging->writeLine("dance");
            break;
        case 1:
            //taunt
            //Logging->writeLine("taunt");
            break;
        case 2:
            //laugh
            //Logging->writeLine("laugh");
            break;
        case 3:
            //joke
            //Logging->writeLine("joke\r\n");
            break;
    }
    CHero *h = peerUnit(peer);
    h->HandleMoveRequest(Vector2f(h->GetX(), h->GetY()), 0);
    EmotionResponse response;
    response.header.netId = emotion->header.netId;
    response.id = emotion->id;
    return broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(response), CHL_S2C);
}
bool PacketHandler::handleCastSpell(HANDLE_ARGS) {
    CHero *p = peerInfo(peer)->unit;
    CastSpellPacket *castData = reinterpret_cast<CastSpellPacket *>(packet->data);
    //Run lua for Model and spellId
    return p->CallSpell(castData->spellId & 0x7F, castData->xTo, castData->yTo);
    /*if(castData->spellId == 0x0B) { //Recall
        //B4
        //60 MoveAns
        //9E -recall cd
        //D7
        //86
        //60 Move Ans
    	}*/
    /*GameTimerUpdate timer3(server->GetGameTime()); //0xC1
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer3), sizeof(GameTimerUpdate), CHL_S2C);*/
}
bool PacketHandler::handleTimerResponse(HANDLE_ARGS) {
    PingCheckPacket *timerData = reinterpret_cast<PingCheckPacket *>(packet->data);
    PingCheckResponse response(timerData->fTime2, server->GetGameTime());
    return broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(PingCheckResponse), CHL_GAMEPLAY);
    //return true; //CHL_GAMEPLAY response?
}
