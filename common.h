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
#ifndef _COMMON_H
#define _COMMON_H

enum GameCmd : unsigned char {
    PKT_S2C_MoveAns                 = 0x60,
};

//Currently attuned to Live 4.5.0.264
enum PacketCmd : unsigned char {
    //Channel //Type
    PKT_KeyCheck = 0x00, //0      //UPDATED
    PKT_C2S_TimerResponse = 0x08, //1 //UPDATED
    PKT_C2S_SellItemReq = 0x09, //1 //UPDATED
    PKT_S2C_SellItemAns = 0x0B, //3 //UPDATED

    PKT_S2C_EndSpawn = 0x11, //? //UPDATED
    PKT_C2S_QueryStatusReq = 0x14, //1 //UPDATED
    PKT_S2C_SkillUp = 0x15, //3 //UPDATED
    PKT_C2S_Ping_Load_Info = 0x16, //1  //UPDATED
    PKT_S2C_SetSpellName = 0x17, // 1//UPDATED

    PKT_C2S_SwapItemReq = 0x20, //1 //UPDATED
    PKT_S2C_FogUpdate2 = 0x23, //3 //UPDATED?
    PKT_S2C_ViewAns = 0x2B, //3  //UPDATED
    PKT_C2S_ViewReq = 0x2D, //1 //UPDATED

    PKT_C2S_SkillUp = 0x38, //1 //UPDATED
    PKT_C2S_SwapItemAns = 0x3D, //1 //UPDATED
    PKT_S2C_AttentionPing = 0x3F, //3 //UPDATED

    PKT_S2C_Emotion = 0x41, //3 UPDATED
    PKT_C2S_Emotion = 0x47, //1 UPDATED
    PKT_S2C_HeroSpawn = 0x4B, //3 UPDATED //PKT_S2C_CreateHero_s
    PKT_S2C_Announce = 0x4C, //3

    PKT_C2S_StartGame = 0x51, //1 //UPDATED
    PKT_S2C_SynchVersion = 0x53, //3 //UPDATED
    PKT_C2S_ScoreBord = 0x55, //1 //UPDATED
    PKT_C2S_AttentionPing = 0x56, //1 //UPDATED
    PKT_S2C_StartGame = 0x5B, //? //UPDATED

    PKT_S2C_WaypointGroups = 0x60,
    PKT_S2C_StartSpawn = 0x61, //? //UPDATED
    //PKT_C2S_OpenShop = 0x63, //1 - DEPRECATED
    PKT_S2C_CreateNeutral = 0x62, //3 - UPDATED
    PKT_C2S_ClientReady = 0x64, //6 //UPDATED ?
    PKT_S2C_LoadHero = 0x65, //6 //UPDATED
    PKT_S2C_LoadName = 0x66, //6 //UPDATED
    PKT_S2C_LoadScreenInfo = 0x67, //6 //UPDATED
    PKT_ChatBoxMessage = 0x68, //5 //UPDATED
    PKT_S2C_BuyItemAns = 0x6E, //3 //UPDATED

    PKT_C2S_MoveReq = 0x71, //1 //UPDATED
    PKT_C2S_MoveConfirm = 0x76, //1 //UPDATED

    PKT_C2S_LockCamera = 0x80, //1 //UPDATED
    PKT_C2S_BuyItemReq = 0x81, //1 //UPDATED
    PKT_S2C_QueryStatusAns = 0x87, //? //UPDATED
    PKT_C2S_Exit = 0x8E, //1 //UPDATED

    PKT_World_SendGameNumber = 0x91, //3 //UPDATED
    PKT_S2C_Ping_Load_Info = 0x94, //4 //UPDATED
    PKT_C2S_CastSpell = 0x99, //1 //UPDATED
    PKT_S2C_TurretSpawn = 0x9C, //3 UPDATED

    PKT_C2S_Surrender = 0xA3, //1 //UPDATED
    //PKT_C2S_GameNumberReq = 0xA6, //1 //DEPRECATED
    PKT_C2S_StatsConfirm = 0xA7, //1 UPDATED
    PKT_C2S_Click = 0xAE, //1 //UPDATED
    PKT_S2C_AnimateSpell = 0xAF, // 1//UPDATED

    PKT_S2C_Waypoints = 0xB9,
    PKT_C2S_SynchVersion = 0xBC, //1 //UPDATED
    PKT_C2S_CharLoaded = 0xBD, //1 //UPDATED

    PKT_S2C_GameTimer = 0xC0, //3 //UPDATED
    PKT_S2C_GameTimerUpdate = 0xC1, //3 //UPDATED
    PKT_S2C_CharStats = 0xC3, //4 //UPDATED
    PKT_S2C_LevelPropSpawn = 0xCF, //3 UPDATED //PKT_SpawnLevelPropS2C_s

    PKT_Batch = 0xFF, //3
};

enum MoveType : unsigned char { //IssueOrder?
    EMOTE = 1, //Hold Position
    _MOVE = 2, //Move / Attack Move
    ATTACK = 3, //Attack
    MOVE = 4,
    ATTACK_MOVE = 7,
    STOP = 10, //StopPosition
};

enum ChatType : unsigned int {
    CHAT_ALL = 0,
    CHAT_TEAM = 1,
};

#define CHL_MAX = 7
enum Channel : unsigned char {
    CHL_HANDSHAKE = 0,
    CHL_C2S = 1,
    CHL_GAMEPLAY = 2,
    CHL_S2C = 3,
    CHL_LOW_PRIORITY = 4,
    CHL_COMMUNICATION = 5,
    CHL_LOADING_SCREEN = 7,
};

enum Spell : unsigned int {
    SPL_Revive = 0x05C8B3A5,
    SPL_Smite = 0x065E8695,
    SPL_Exhaust = 0x08A8BAE4,
    SPL_Barrier = 0x0CCFB982,
    SPL_Teleport = 0x004F1364,
    SPL_Ghost = 0x064ACC95,
    SPL_Heal = 0x0364AF1C,
    SPL_Cleanse = 0x064D2094,
    SPL_Clarity = 0x03657421,
    SPL_Ignite = 0x06364F24,
    SPL_Promote = 0x0410FF72,
    SPL_Clair = 0x09896765,
    SPL_Flash = 0x06496EA8,
    SPL_Test = 0x0103D94C,
};
#endif
