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

bool PacketHandler::handleNull(HANDLE_ARGS)
{
	return true;
}

bool PacketHandler::handleKeyCheck(ENetPeer *peer, ENetPacket *packet)
{
	KeyCheck *keyCheck = (KeyCheck*)packet->data;
	uint64 userId = _blowfish->Decrypt(keyCheck->checkId);

	/*
	uint64 enc = _blowfish->Encrypt(keyCheck->userId);
	char buffer[255];
	unsigned char *p = (unsigned char*)&enc;
	for(int i = 0; i < 8; i++)
	{
	sprintf(&buffer[i*3], "%02X ", p[i]);
	}
	PDEBUG_LOG_LINE(Logging," Enc id: %s\n", buffer);*/

	if (userId == keyCheck->userId)
	{
		PDEBUG_LOG_LINE(Logging, " User got the same key as i do, go on!\n");
		peerInfo(peer)->keyChecked = true;
		peerInfo(peer)->userId = userId;
	}
	else
	{
		Logging->errorLine(" WRONG KEY, GTFO!!!\n");
		return false;
	}

	//Send response as this is correct (OFC DO SOME ID CHECKS HERE!!!)
	KeyCheck response;
	response.userId = keyCheck->userId;

	bool bRet = sendPacket(peer, reinterpret_cast<uint8*>(&response), sizeof(KeyCheck), CHL_HANDSHAKE);
	handleGameNumber(peer, NULL);//Send 0x91 Packet?
	return bRet;
}

bool PacketHandler::handleGameNumber(ENetPeer *peer, ENetPacket *packet)
{
	WorldSendGameNumber world;
	world.gameId = 1;
	strcpy((char*)world.data1, "EUW1");
	memcpy(world.data, peerInfo(peer)->name, peerInfo(peer)->nameLen);

	return sendPacket(peer, reinterpret_cast<uint8*>(&world), sizeof(WorldSendGameNumber), CHL_S2C);
}

bool PacketHandler::handleSynch(ENetPeer *peer, ENetPacket *packet)
{
	SynchVersion *version = reinterpret_cast<SynchVersion*>(packet->data);
	Logging->writeLine("Client version: %s\n", version->version);

	SynchVersionAns answer;
	answer.mapId = 1;
	answer.players[0].userId = peerInfo(peer)->userId;
	answer.players[0].skill1 = SPL_Ignite;
	answer.players[0].skill2 = SPL_Flash;

	return sendPacket(peer, reinterpret_cast<uint8*>(&answer), sizeof(SynchVersionAns), 3);
}

bool PacketHandler::handleMap(ENetPeer *peer, ENetPacket *packet)
{
	LoadScreenPlayer *playerName = LoadScreenPlayer::create(PKT_S2C_LoadName, peerInfo(peer)->name, peerInfo(peer)->nameLen);
	playerName->userId = peerInfo(peer)->userId;

	LoadScreenPlayer *playerHero = LoadScreenPlayer::create(PKT_S2C_LoadHero, peerInfo(peer)->type, peerInfo(peer)->typeLen);
	playerHero->userId = peerInfo(peer)->userId;
	playerHero->skinId = peerInfo(peer)->skinNo;

	//Builds team info
	LoadScreenInfo screenInfo;
	screenInfo.bluePlayerNo = 1;
	screenInfo.redPlayerNo = 0;

	screenInfo.bluePlayerIds[0] = peerInfo(peer)->userId;

	bool pInfo = sendPacket(peer, reinterpret_cast<uint8*>(&screenInfo), sizeof(LoadScreenInfo), CHL_LOADING_SCREEN);

	//For all players send this info
	bool pName = sendPacket(peer, reinterpret_cast<uint8*>(playerName), playerName->getPacketLength(), CHL_LOADING_SCREEN);
	bool pHero = sendPacket(peer, reinterpret_cast<uint8*>(playerHero), playerHero->getPacketLength(), CHL_LOADING_SCREEN);

	//cleanup
	LoadScreenPlayer::destroy(playerName);
	LoadScreenPlayer::destroy(playerHero);

	return (pInfo && pName && pHero);

}

//building the map
bool PacketHandler::handleSpawn(ENetPeer *peer, ENetPacket *packet)
{
	StatePacket2 start(PKT_S2C_StartSpawn);
	bool p1 = sendPacket(peer, reinterpret_cast<uint8*>(&start), sizeof(StatePacket2), CHL_S2C);

	printf("Spawning map\r\n");

	HeroSpawn spawn;
	spawn.netId = peerInfo(peer)->netId;
	spawn.gameId = 0;
	memcpy(spawn.name, peerInfo(peer)->name, peerInfo(peer)->nameLen);
	memcpy(spawn.type, peerInfo(peer)->type, peerInfo(peer)->typeLen);
	bool p2 = sendPacket(peer, reinterpret_cast<uint8*>(&spawn), sizeof(HeroSpawn), CHL_S2C);

	HeroSpawn2 h2;
	h2.header.netId = peerInfo(peer)->netId;
	sendPacket(peer, reinterpret_cast<uint8*>(&h2), sizeof(HeroSpawn2), CHL_S2C);
	HeroSpawn3 h3;
	h3.header.netId = peerInfo(peer)->netId;
	sendPacket(peer, reinterpret_cast<uint8*>(&h3), sizeof(HeroSpawn3), CHL_S2C);

	//Spawn Turrets
	char* szTurrets[24] = {
		"@@Turret_T1_R_03_A",
		"@@Turret_T1_R_02_A",
		"@@Turret_T1_C_07_A",
		"@@Turret_T2_R_03_A",
		"@@Turret_T2_R_02_A",
		"@@Turret_T2_R_01_A",
		"@@Turret_T1_C_05_A",
		"@@Turret_T1_C_04_A",
		"@@Turret_T1_C_03_A",
		"@@Turret_T1_C_01_A",
		"@@Turret_T1_C_02_A",
		"@@Turret_T2_C_05_A",
		"@@Turret_T2_C_04_A",
		"@@Turret_T2_C_03_A",
		"@@Turret_T2_C_01_A",
		"@@Turret_T2_C_02_A",
		"@@Turret_OrderTurretShrine_A",
		"@@Turret_ChaosTurretShrine_A",
		"@@Turret_T1_L_03_A",
		"@@Turret_T1_L_02_A",
		"@@Turret_T1_C_06_A",
		"@@Turret_T2_L_03_A",
		"@@Turret_T2_L_02_A",
		"@@Turret_T2_L_01_A"
	};
	for (UINT i = 0; i < 24; i++) {
		TurretSpawn turretSpawn;
		turretSpawn.tID = i + 1;
		strcpy((char*)turretSpawn.name, szTurrets[i]);

		sendPacket(peer, reinterpret_cast<uint8*>(&turretSpawn), sizeof(TurretSpawn), CHL_S2C);
	}

	//Spawn Props
	LevelPropSpawn lpSpawn;

	lpSpawn.SetProp("LevelProp_Yonkey", "Yonkey");
	lpSpawn.header.netId = 0;
	lpSpawn.netId = GetNewNetID();
	lpSpawn.x = 12465; lpSpawn.y = 101;
	sendPacket(peer, reinterpret_cast<uint8*>(&lpSpawn), sizeof(LevelPropSpawn), CHL_S2C);

	lpSpawn.SetProp("LevelProp_Yonkey1", "Yonkey");
	lpSpawn.header.netId = 0;
	lpSpawn.netId = GetNewNetID();
	lpSpawn.x = -76; lpSpawn.y = 94;
	sendPacket(peer, reinterpret_cast<uint8*>(&lpSpawn), sizeof(LevelPropSpawn), CHL_S2C);

	lpSpawn.SetProp("LevelProp_ShopMale", "ShopMale");
	lpSpawn.header.netId = 0;
	lpSpawn.netId = GetNewNetID();
	lpSpawn.x = 13374; lpSpawn.y = 194;
	sendPacket(peer, reinterpret_cast<uint8*>(&lpSpawn), sizeof(LevelPropSpawn), CHL_S2C);

	lpSpawn.SetProp("LevelProp_ShopMale1", "ShopMale");
	lpSpawn.header.netId = 0;
	lpSpawn.netId = GetNewNetID();
	lpSpawn.x = -99; lpSpawn.y = 191;
	sendPacket(peer, reinterpret_cast<uint8*>(&lpSpawn), sizeof(LevelPropSpawn), CHL_S2C);


	StatePacket end(PKT_S2C_EndSpawn);
	bool p3 = sendPacket(peer, reinterpret_cast<uint8*>(&end), sizeof(StatePacket), CHL_S2C);

	BuyItemAns recall;
	recall.header.netId = peerInfo(peer)->netId;
	recall.itemId = 2001;
	recall.slotId = 7;
	recall.stack = 1;
	bool p4 = sendPacket(peer, reinterpret_cast<uint8*>(&recall), sizeof(BuyItemAns), CHL_S2C); //activate recall slot


	GameTimer timer(0); //0xC0
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer), sizeof(GameTimer), CHL_S2C);
	GameTimer timer2(0.4); //0xC0
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer2), sizeof(GameTimer), CHL_S2C);
    GameTimerUpdate timer3(0.4); //0xC1
    sendPacket(peer, reinterpret_cast<uint8 *>(&timer3), sizeof(GameTimerUpdate), CHL_S2C);
    //lvl 1 R for elise
    for(int i = 0; i < 4; i++) {
        SpellSet spell(peerInfo(peer)->netId, i, 1);
        sendPacket(peer, reinterpret_cast<uint8 *>(&spell), sizeof(SpellSet), CHL_S2C);
    }
	return p1 & p2 & p3;
}

bool PacketHandler::handleStartGame(HANDLE_ARGS)
{
	StatePacket start(PKT_S2C_StartGame);
	sendPacket(peer, reinterpret_cast<uint8*>(&start), sizeof(StatePacket), CHL_S2C);
	FogUpdate2 test;
	test.x = 0;
	test.y = 0;
	test.radius = 1;
	test.unk1 = 2;
	//uint8 p[] = {0xC5, 0x19, 0x00, 0x00, 0x40, 0x00, 0x00, 0x50};
	//sendPacket(peer, reinterpret_cast<uint8*>(p), sizeof(p), 3);
	sendPacket(peer, reinterpret_cast<uint8*>(&test), sizeof(FogUpdate2), 3);

	//playing around 8-)
	/*
	CharacterStats movement;
	movement.netId = peerInfo(peer)->netId;
	movement.statType = STI_Movement;
	movement.statValue = 800;
	sendPacket(peer,reinterpret_cast<uint8*>(&movement),sizeof(movement), 4);*/

	return true;
}

bool PacketHandler::handleAttentionPing(ENetPeer *peer, ENetPacket *packet)
{
	AttentionPing *ping = reinterpret_cast<AttentionPing*>(packet->data);
	AttentionPingAns response(ping);

	Logging->writeLine("Plong x: %f, y: %f, z: %f, type: %i\n", ping->x, ping->y, ping->z, ping->type);

	return broadcastPacket(reinterpret_cast<uint8*>(&response), sizeof(AttentionPing), 3);
}

bool PacketHandler::handleView(ENetPeer *peer, ENetPacket *packet)
{
	ViewReq *request = reinterpret_cast<ViewReq*>(packet->data);

	//Logging->writeLine("View (%i), x:%f, y:%f, zoom: %f\n", request->requestNo, request->x, request->y, request->zoom);

	ViewAns answer;
	answer.requestNo = request->requestNo;

	sendPacket(peer, reinterpret_cast<uint8*>(&answer), sizeof(ViewAns), CHL_S2C, UNRELIABLE);
	enet_host_flush(peer->host);

	if (request->requestNo == 0xFE)
	{
		answer.requestNo = 0xFF;
		sendPacket(peer, reinterpret_cast<uint8*>(&answer), sizeof(ViewAns), CHL_S2C, UNRELIABLE);
	}

	return true;

}

inline void SetBitmaskValue(byte mask[], int pos, bool val) {
	if (pos < 0)
		return;
	if (val)
		mask[pos / 8] |= 1 << (pos % 8);
	else
		mask[pos / 8] &= ~(1 << (pos % 8));
}

inline bool GetBitmaskValue(byte mask[], int pos) {
	return pos >= 0 && ((1 << (pos % 8)) & mask[pos / 8]) != 0;
}

#include <vector>

std::vector<MovementVector> readWaypoints(byte* buffer, int coordCount) {
	UINT nPos = (coordCount + 5) / 8;
	if (coordCount % 2)
		nPos++;
	int vectorCount = coordCount / 2;
	std::vector<MovementVector> vMoves;
	MovementVector lastCoord;
	for (int i = 0; i < vectorCount; i++) {
		if (GetBitmaskValue(buffer, (i - 1) * 2)) {
			lastCoord.x += *(char*)&buffer[nPos++];
		}
		else {
			lastCoord.x = *(short*)&buffer[nPos]; nPos += 2;
		}
		if (GetBitmaskValue(buffer, (i - 1) * 2 + 1)) {
			lastCoord.y += *(char*)&buffer[nPos++];
		}
		else {
			lastCoord.y = *(short*)&buffer[nPos]; nPos += 2;
		}
		vMoves.push_back(lastCoord);
	}
	return vMoves;
}

#define MAP_WIDTH (13982 / 2)
#define MAP_HEIGHT (14446 / 2)

bool PacketHandler::handleMove(ENetPeer *peer, ENetPacket *packet)
{
	MovementReq *request = reinterpret_cast<MovementReq*>(packet->data);

	switch (request->type)
	{
		//TODO, Implement stop commands
	case STOP:
		Logging->writeLine("Move stop\n");
		return true;
	case EMOTE:
		Logging->writeLine("Emotion\n");
		return true;
	}

	std::vector<MovementVector> vMoves = readWaypoints(&request->moveData, request->vectorNo);

	Logging->writeLine("Move to(normal): x:%f, y:%f, type: %i, vectorNo: %i\n", request->x, request->y, request->type, vMoves.size());
	for (int i = 0; i < vMoves.size(); i++)
		printf("     Vector %i, x: %f, y: %f\n", i, 2.0 * vMoves[i].x + MAP_WIDTH, 2.0 * vMoves[i].y + MAP_HEIGHT);

	MovementAns *answer = MovementAns::create(request->vectorNo);
	answer->ok = 1;
	answer->vectorNo = request->vectorNo;
	answer->netId = peerInfo(peer)->netId;
	for (int i = 0; i < vMoves.size(); i++)
	{
		answer->getVector(i)->x = vMoves[i].x;
		answer->getVector(i)->y = vMoves[i].y;
	}

	bool bRet = broadcastPacket(reinterpret_cast<uint8*>(answer), answer->size(), 4);
	MovementAns::destroy(answer);
	return bRet;
}

bool PacketHandler::handleLoadPing(ENetPeer *peer, ENetPacket *packet)
{
	PingLoadInfo *loadInfo = reinterpret_cast<PingLoadInfo*>(packet->data);

	PingLoadInfo response;
	memcpy(&response, packet->data, sizeof(PingLoadInfo));
	response.header.cmd = PKT_S2C_Ping_Load_Info;
	response.userId = peerInfo(peer)->userId;


	Logging->writeLine("loaded: %f, ping: %f, %f\n", loadInfo->loaded, loadInfo->ping, loadInfo->f3);
	bool bRet = broadcastPacket(reinterpret_cast<uint8*>(&response), sizeof(PingLoadInfo), CHL_LOW_PRIORITY, UNRELIABLE);
	static bool bLoad = false;
	if (!bLoad) {
		handleMap(peer, NULL);
		bLoad = true;
	}
	return bRet;
}

bool PacketHandler::handleQueryStatus(HANDLE_ARGS)
{
	QueryStatus response;
	return sendPacket(peer, reinterpret_cast<uint8*>(&response), sizeof(QueryStatus), CHL_S2C);
}

bool PacketHandler::handleChatBoxMessage(HANDLE_ARGS)
{
	ChatMessage* message = reinterpret_cast<ChatMessage*>(packet->data);

	//Lets do commands
	if (message->msg == '.')
	{
		const char *cmd[] = { ".set", ".gold", ".speed", ".health", ".xp", ".ap", ".ad", ".mana", ".help" };

		//Set field
		if (strncmp(message->getMessage(), cmd[0], strlen(cmd[0])) == 0)
		{
			uint32 blockNo = atoi(&message->getMessage()[strlen(cmd[0]) + 1]);
			uint32 fieldNo = atoi(&message->getMessage()[strlen(cmd[0]) + 3]);
			float value = (float)atoi(&message->getMessage()[strlen(cmd[0]) + 5]);

			uint32 mask = 1 << abs(((int)fieldNo - 1));

			CharacterStats *stats = CharacterStats::create(blockNo, mask);
			stats->netId = peerInfo(peer)->netId;
			stats->setValue(blockNo, mask, value);

			Logging->writeLine("Setting to %f in block: %i, field: %i\n", value, blockNo, fieldNo);
			sendPacket(peer, reinterpret_cast<uint8*>(stats), stats->getSize(), CHL_LOW_PRIORITY, 2);
			stats->destroy();
			return true;
		}

		// Set Gold
		if (strncmp(message->getMessage(), cmd[1], strlen(cmd[1])) == 0)
		{
			float gold = (float)atoi(&message->getMessage()[strlen(cmd[1]) + 1]);
			CharacterStats *stats = CharacterStats::create(FM1_Gold, 0, 0, 0, 0);
			stats->netId = peerInfo(peer)->netId;
			stats->setValue(1, FM1_Gold, gold);

			Logging->writeLine("Set gold to %f\n", gold);
			sendPacket(peer, reinterpret_cast<uint8*>(stats), stats->getSize(), CHL_LOW_PRIORITY, 2);
			stats->destroy();
			return true;
		}
		/*

		//movement
		if(strncmp(message->getMessage(), cmd[2], strlen(cmd[2])) == 0)
		{
		float data = (float)atoi(&message->getMessage()[strlen(cmd[2])+1]);

		charStats.statType = STI_Movement;
		charStats.statValue = data;
		Logging->writeLine("set champ speed to %f\n", data);
		sendPacket(peer,reinterpret_cast<uint8*>(&charStats),sizeof(charStats), CHL_LOW_PRIORITY, 2);
		return true;
		}
		//health
		if(strncmp(message->getMessage(), cmd[3], strlen(cmd[3])) == 0)
		{
		float data = (float)atoi(&message->getMessage()[strlen(cmd[3])+1]);

		charStats.statType = STI_Health;
		charStats.statValue = data;
		Logging->writeLine("set champ health to %f\n", data);
		sendPacket(peer,reinterpret_cast<uint8*>(&charStats),sizeof(charStats), CHL_LOW_PRIORITY, 2);
		return true;
		}
		//experience
		if(strncmp(message->getMessage(), cmd[4], strlen(cmd[4])) == 0)
		{
		float data = (float)atoi(&message->getMessage()[strlen(cmd[4])+1]);

		charStats.statType = STI_Exp;
		charStats.statValue = data;
		Logging->writeLine("set champ exp to %f\n", data);
		sendPacket(peer,reinterpret_cast<uint8*>(&charStats),sizeof(charStats), CHL_LOW_PRIORITY, 2);
		return true;
		}
		//AbilityPower
		if(strncmp(message->getMessage(), cmd[5], strlen(cmd[5])) == 0)
		{
		float data = (float)atoi(&message->getMessage()[strlen(cmd[5])+1]);

		charStats.statType = STI_AbilityPower;
		charStats.statValue = data;
		Logging->writeLine("set champ abilityPower to %f\n", data);
		sendPacket(peer,reinterpret_cast<uint8*>(&charStats),sizeof(charStats), CHL_LOW_PRIORITY, 2);
		return true;
		}
		//Attack damage
		if(strncmp(message->getMessage(), cmd[6], strlen(cmd[6])) == 0)
		{
		float data = (float)atoi(&message->getMessage()[strlen(cmd[6])+1]);

		charStats.statType = STI_AttackDamage;
		charStats.statValue = data;
		Logging->writeLine("set champ attack damage to %f\n", data);
		sendPacket(peer,reinterpret_cast<uint8*>(&charStats),sizeof(charStats), CHL_LOW_PRIORITY, 2);
		return true;
		}
		//Mana
		if(strncmp(message->getMessage(), cmd[7], strlen(cmd[7])) == 0)
		{
		float data = (float)atoi(&message->getMessage()[strlen(cmd[7])+1]);

		charStats.statType = STI_Mana;
		charStats.statValue = data;
		Logging->writeLine("set champ mana to %f\n", data);
		sendPacket(peer,reinterpret_cast<uint8*>(&charStats),sizeof(charStats), CHL_LOW_PRIORITY, 2);
		return true;
		}
		*/

	}

	switch (message->type)
	{
	case CMT_ALL:
		return broadcastPacket(packet->data, packet->dataLength, CHL_COMMUNICATION);
		break;
	case CMT_TEAM:
		//!TODO make a team class and foreach player in the team send the message
		return sendPacket(peer, packet->data, packet->dataLength, CHL_COMMUNICATION);
		break;
	default:
		Logging->errorLine("Unknown ChatMessageType\n");
		return sendPacket(peer, packet->data, packet->dataLength, CHL_COMMUNICATION);
		break;
	}
	return false;
}

bool PacketHandler::handleSkillUp(HANDLE_ARGS) {

	SkillUpPacket* skillUpPacket = reinterpret_cast<SkillUpPacket*>(packet->data);
	//!TODO Check if can up skill? :)
	SkillUpResponse skillUpResponse;

	skillUpResponse.skill = skillUpPacket->skill;
	skillUpResponse.level = 0x01;
	skillUpResponse.pointsLeft = 5;

	return sendPacket(peer, reinterpret_cast<uint8*>(&skillUpResponse), sizeof(skillUpResponse), CHL_GAMEPLAY);

}

bool PacketHandler::handleBuyItem(HANDLE_ARGS) {
	static int slot = 0;
	BuyItemReq* request = reinterpret_cast<BuyItemReq*>(packet->data);
	
	BuyItemAns response;
	response.header.netId = request->header.netId;
	response.itemId = request->id;
	response.slotId = slot++; //check for trinket ID and addapt slot
	response.stack = 1;


	return broadcastPacket(reinterpret_cast<uint8*>(&response), sizeof(response), CHL_S2C);
}

bool PacketHandler::handleEmotion(HANDLE_ARGS) {
	EmotionPacket* emotion = reinterpret_cast<EmotionPacket*>(packet->data);
	//for later use -> tracking, etc.
	switch (emotion->id)
	{
	case 0:
		//dance
		Logging->writeLine("dance");
		break;
	case 1:
		//taunt
		Logging->writeLine("taunt");
		break;
	case 2:
		//laugh
		Logging->writeLine("laugh");
		break;
	case 3:
		//joke
		Logging->writeLine("joke");
		break;
	}
	EmotionResponse response;
	response.header.netId = emotion->header.netId;
	response.id = emotion->id;

	return broadcastPacket(reinterpret_cast<uint8*>(&response), sizeof(response), CHL_S2C);

}
