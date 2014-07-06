#include "stdafx.h"
#include "PacketHandler.h"
#include "LuaVM.h"
#include "time.h"
#include <intrin.h>
#include <algorithm>

#include "SpellCmdPacket.h"
#include "DashCmdPacket.h"

#include "LoLPacket.h"
#include "ObjectManager.h"
//#include "Game.h"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#define  _CRT_SECURE_NO_WARNINGS TRUE

CLuaVM *luaVM;

#include <iostream>
#include "Packets.h"

BOOL CLuaVM::ExecuteFunction(const char *szFunction) {
    BOOL bRet = false;
    try {
        bRet = luaL_dostring(L, szFunction) == 0;
        if(!bRet) {
            const char *szError = lua_tostring(L, -1);
            printf(szError);
            //Remove the error message from the stack
            lua_pop(L, 1);
        }
    } catch(...) {
        const char *szError = lua_tostring(L, -1);
        printf(szError);
        //Remove the error message from the stack
        lua_pop(L, 1);
    }
    return bRet;
}

object CLuaVM::getFunction(luabind::object env, const char *szFunction) {
    try {
        if(env) {
            luabind::object g = env[szFunction];
            if(g && type(g) == LUA_TFUNCTION) {
                return g;
            }
        }
    } catch(const std::exception &e) {
    }
    return object();
}
#include <sstream>
int AddFileAndLineNumber(lua_State *L) {
    lua_Debug d;
    lua_getstack(L, 1, &d);
    lua_getinfo(L, "Sln", &d);
    std::string err = lua_tostring(L, -1);
    lua_pop(L, 1);
    std::stringstream msg;
    msg << "Script error: " << d.short_src << ":" << d.currentline;
    if(d.name != 0)
    { msg << " (" << d.namewhat << " " << d.name << ")"; }
    msg << " " << err;
    lua_pushstring(L, msg.str().c_str());
    return 1;
}

void CLuaVM::callFunction(const char *szFunction) {
    if(L == NULL)
    { return; }
    for(UINT i = 0; i < vEnvironments.size(); i++) {
        object fnc = getFunction(vEnvironments[i], szFunction);
        if(fnc) {
            try {
                call_function<void>(fnc);
            } catch(error &e) {
                const char *szError = lua_tostring(L, -1);
                printf("Error in %s >> %s\r\n", szFunction, szError);
                //Remove the error message from the stack
                lua_pop(L, 1);
            }
        }
    }
}

template <class T>
void CLuaVM::callFunction(const char *szFunction, T t) {
    for(UINT i = 0; i < vEnvironments.size(); i++) {
        object fnc = getFunction(vEnvironments[i], szFunction);
        if(fnc) {
            try {
                call_function<void>(fnc, t);
            } catch(error &e) {
                const char *szError = lua_tostring(L, -1);
                //LPrintChat(szError);
            }
        }
    }
}

template <class T1, class T2>
UINT CLuaVM::callFunction(const char *szFunction, T1 t1, T2 t2) {
    UINT r = 0;
    for(UINT i = 0; i < vScripts.size(); i++) {
        object fnc = getFunction(vScripts[i].c_str(), szFunction);
        if(fnc) {
            try {
                r = call_function<UINT>(fnc, t1, t2);
            } catch(error &e) {
                const char *szError = lua_tostring(L, -1);
                //LPrintChat(szError);
            }
        }
    }
    return r;
}

void LPrint(const char *szStr) {
    printf("%s\r\n", szStr);
}


void LExit() {
    ExitProcess(0);
}

void OutputDbgString(const char *szStr) {
    OutputDebugStringA(szStr);
}

UINT LGetTickCount() {
    return GetTickCount();
}


const char *LowerFirstLetter(char *_szText) {
    static char szText[124];
    strncpy(szText, _szText, 124);
    szText[0] = tolower(szText[0]);
    return szText;
}

void LSetConsoleTitle(const char *szTitle) {
    SetConsoleTitle(szTitle);
}

class CMissile {
    public:
        CMissile() {
            ZeroMemory(this, sizeof(CMissile));
            wFlag1 = 0x66;
            wFlag2 = 0x10;
        }
        CMissile(const char *szName) : CMissile() {
            dwHash = getHash(szName);
        }
        DWORD dwHash;
        WORD wFlag1;
        WORD wFlag2;
        float fChannelTime;
        float fDelay;
        float fVisible;
        float fCooldown;
        //..
        float fMana;
        //..
        float fromX;
        float fromY;
        float toX;
        float toY;
        //..
        DWORD netId1;
        DWORD netId2;
        DWORD hash2;
};
#include "NetworkListener.h"
#include "Server.h"
void ReleaseMissile(CHero *p, DWORD netID) {
    _ReleaseMissilePacket rlsMissile(p->GetNetworkID(), netID); //6D creates obj
    server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&rlsMissile), sizeof(_ReleaseMissilePacket), CHL_S2C);
}
DWORD SendMissile(CHero *p, CMissile *m) {
    p->Pause();
    auto obj1 = CObjectManager::NewObject();// CObjectManager::AddObject(new CUnit(CObjectManager::GetNewIndex(), "JavelinToss", "Missile", p->tTeam));
    auto obj2 = CObjectManager::NewObject();
    DisplaySpellPacket dsP(m->dwHash, obj1->GetNetworkID(), obj2->GetNetworkID(), p->GetNetworkID());
    dsP.spawn.flags = m->wFlag1;
    dsP.spawn.SetPos(m->toX, m->toY);
    dsP.spawn.flags = m->wFlag1;
    dsP.wFlags = m->wFlag2;
    dsP.fFromX = m->fromX;
    dsP.fFromZ = m->fromY;
    dsP.fCastTime = m->fChannelTime;
    dsP.fDelay = m->fDelay;
    dsP.fVisible = m->fVisible;
    dsP.fCooldown = m->fCooldown;
    dsP.fMana = m->fMana;
    server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&dsP), sizeof(DisplaySpellPacket), CHL_S2C);
    return obj1->GetNetworkID();
}
void SendMissile2(CHero *p, const char *szName, float fX, float fY, DWORD dw15) {
    p->Pause();
    auto obj1 = CObjectManager::NewObject();// CObjectManager::AddObject(new CUnit(CObjectManager::GetNewIndex(), "JavelinToss", "Missile", p->tTeam));
    auto obj2 = CObjectManager::NewObject();
    SpellCmdPacket spellCmd(p->GetNetworkID(), 0x66, dw15/*0x10*/, getHash(szName), p->GetHash(), obj1->GetNetworkID(), obj2->GetNetworkID(), { p->GetX(), 0, p->GetY() }, { fX, 0, fY }, 0.25, 0, 1.0, 3.0, 25.0);
    server->listener->_handler->sendPacket(p->GetPeer(), spellCmd);
    /*SpawnMissile nidaTrap(getHash(szName), p->GetNetworkID(), obj1->GetNetworkID(), obj2->GetNetworkID()); //B4 creates obj
    nidaTrap.unk15 = dw15;
    nidaTrap.spawn.toX = fX;
    nidaTrap.spawn.toZ = fY;
    //...
    nidaTrap.spawn.fromX = 0;
    nidaTrap.spawn.fromZ = 0;
    //..
    nidaTrap.fUnk10 = 0.25; //channel time
    nidaTrap.fUnk11 = 0; //delay
    nidaTrap.fUnk12 = 1.0; //>0 visible
    nidaTrap.fUnk13 = 14.0; //cd, works sometimes
    //nidaTrap.unk15 = m->dw15;
    //..
    nidaTrap.fUnk1 = 50.0; //mana
    nidaTrap.fUnk2 = p->x; //x
    nidaTrap.fUnk3 = 0; //y
    nidaTrap.fUnk4 = p->y; //z
    server->listener->_handler->sendPacket(p->peer, reinterpret_cast<uint8 *>(&nidaTrap), sizeof(SpawnMissile), CHL_S2C);*/
    //ReleaseMissile(p, obj1->GetNetworkID());
}
void SendMissile3(CHero *p, const char *szName, float fX, float fY) {
    SendMissile2(p, szName, fX, fY, 0x10);
}
void SendSpell(CHero *p, const char *szSpell, DWORD a1, float f1) {
    _AnimateSpellPacket nidaPounce2(p->GetNetworkID(), a1, (char *)szSpell); //AF
    nidaPounce2.f1 = f1;
    server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&nidaPounce2), sizeof(_AnimateSpellPacket), CHL_S2C);
}
void SetSpell(CHero *p, int nSlot, const char *szSpell) {
    SetSpellPacket setSpell(p->GetNetworkID(), nSlot, (char *)szSpell); //17
    server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&setSpell), sizeof(SetSpellPacket), CHL_S2C);
}
void SetUnitPos(CHero *p, float fX, float fY) {
    UpdateDrawPos drawPos(p->GetNetworkID(), fX, fY, 0, 0); //EF
    server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&drawPos), sizeof(UpdateDrawPos), CHL_S2C);
}
void DoDash(CHero *p, float f1, float f2, float f3, float f4) { //Probably set facing position
    UpdateDash drawPos(p->GetNetworkID(), f1, f2, f3, f4); //4F
    server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&drawPos), sizeof(UpdateDash), CHL_S2C);
}
void DoDash2(CHero *p, float fX, float fY) {
    std::vector<Vector2f> waypoints;
    waypoints.push_back({ p->GetX(), p->GetY() });
    waypoints.push_back({ fX, fY });
    DashCmdPacket dashP(p->GetNetworkID(), 1233.626709, waypoints); //63
    server->listener->_handler->sendPacket(p->GetPeer(), dashP);
}
DWORD LGetHash(const char *szParticle) {
    return getHash(szParticle);
}
DWORD LGetHash2(const char *szCharacter, UINT nSkin) {
    return getCharacterHash((char *)szCharacter, nSkin);
}
void SendPacket(CHero *hero, CLoLPacket *p) {
    server->listener->_handler->sendPacket(hero->GetPeer(), (uint8 *)p->buffer, p->size, p->dwArg1, p->dwArg2);
}
void BroadcastPacket(CLoLPacket *p) {
    server->listener->_handler->broadcastPacket((uint8 *)p->buffer, p->size, p->dwArg1, p->dwArg2);
}
CObject *NewObject() {
    return CObjectManager::NewObject();
}

void ApplyBuff(CHero *hero, int nSlot, int nType, int nStacks, int iVisible, const char *szBuff, float fTimer) {
    ApplyBuffPacket drawPos(hero->GetNetworkID(), nStacks, nType, nStacks, iVisible, getHash(szBuff), hero->GetHash(), fTimer); //4F
    server->listener->_handler->sendPacket(hero->GetPeer(), reinterpret_cast<uint8 *>(&drawPos), sizeof(ApplyBuffPacket), CHL_S2C);
}
#include "EmitParticlesPacket.h"
void SendPE(EmitParticlesPacket *pe, CHero *hero) {
    pe->Send((int)hero->_peer);
}
void KillParticle(CHero *hero, int nSlot, DWORD buffHash) {
    RemoveBuffPacket removeBuff(hero->GetNetworkID(), nSlot, buffHash); //37
    server->listener->_handler->sendPacket(hero->GetPeer(), reinterpret_cast<uint8 *>(&removeBuff), sizeof(RemoveBuffPacket), CHL_S2C);
}

#include "MissileSpawnPacket.h"
void CreateMissile(CHero *hero, const char *szMissile, float fX, float fY) {
    MissileSpawnPacket p(hero, szMissile, fX, fY);
    p.Send(hero->_peer);
}

void DoDash3(CHero *p, float fX, float fY, float fAngle) {
    printf("DoDash3: %f / %f / %f\r\n", fX, fY, fAngle);
    //HeroSpawn3 h2(p->GetNetworkID(), p->spawnX, p->spawnY, ,fAngle);
    //server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&h2), sizeof(HeroSpawn3), CHL_S2C);
    //UpdateDrawPos drawPos(p->GetNetworkID(), fX, 0, fY, fDash); //EF
    //server->listener->_handler->sendPacket(p->GetPeer(), reinterpret_cast<uint8 *>(&drawPos), sizeof(UpdateDrawPos), CHL_S2C);
}

void CLuaVM::register_modules() {
    set_pcall_callback(AddFileAndLineNumber);
    open(L);
    ExecuteFunction(
        //Disable the DLL and all-in-one package loaders
        "package.searchers[3] = nil\n"
        "package.searchers[4] = nil\n"
        //Disable loadlib
        //"package.loadlib = function() end\n"
        //Disable I/O functionality
        /*"for k, v in pairs(io) do\n"
        "if type(v) == \"function\" then\n"
        "io[k] = function() end\n"
        "end\n"
        "end\n"*/
        //Disable some OS functionality
        "local osOriginal = os\n"
        "os = \n"
        "{\n"
        "date = os.date,\n"
        "time = os.time, \n"
        "setlocale = os.setlocale,\n"
        "clock = os.clock, \n"
        "difftime = os.difftime,\n"
        "}\n"
        "for k, v in pairs(osOriginal) do\n"
        "if not os[k] and type(v) == \"function\" then\n"
        "os[k] = function() end\n"
        "end\n"
        "end\n"
        //Customize the print function
        "function print(...)\n"
        "   local result = \"\"\n"
        "   for i, v in ipairs{...} do\n"
        "       result = result .. v .. ' '\n"
        "   end\n"
        "   XPrint(result)\n"
        "end\n");
    /*char szPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH,szPath);
    object package = globals(L)["package"];
    package["path"] = (std::string(szPath) + "\\?.lua").c_str();*/
    share_table = luabind::newtable(L);
    module(L)[ //functions
        def("NewObject", NewObject),
        def("SendPacket", SendPacket),
        def("BroadcastPacket", BroadcastPacket),
        def("GetHash", LGetHash),
        def("GetCharacterHash", LGetHash2),
        def("DoDash", DoDash),
        def("DoDash2", DoDash2),
        def("DoDash3", DoDash3),
        def("SendSpell", SendSpell),
        def("ApplyBuff", ApplyBuff),
        def("CreateMissile", CreateMissile),
        def("ReleaseMissile", ReleaseMissile),
        def("KillParticle", KillParticle),
        def("AnimateSpell", SendSpell),
        def("SetSpellName", SetSpell),
        def("SetSpell", SetSpell),
        def("SendUnitPos", SetUnitPos),
        def("SendMissile", SendMissile),
        def("SendMissile", SendMissile2),
        def("SendMissile", SendMissile3),
        def("exit", LExit),
        def("GetTickCount", LGetTickCount),
        def("SetConsoleTitle", LSetConsoleTitle),
        def("XPrint", LPrint),
        def("OutputDebugString", OutputDbgString)
    ];
    module(L)[
        class_<CParticle>("CParticle")
        .def_readonly("netId", &CParticle::netID),
        class_<CObject>("CObject")
        .property("netId", &CObject::GetNetworkID)
        .def_readonly("index", &CObject::index)
        .def_readwrite("x", &CObject::x)
        .def_readwrite("y", &CObject::y)
        .def_readwrite("z", &CObject::z),
        class_<CUnit, CObject>("CUnit")
        .def_readwrite("skinId", &CUnit::mSkinId)
        .property("x", &CUnit::GetX)
        .property("y", &CUnit::GetY)
        .property("name", &CUnit::GetName, &CUnit::SetName)
        .property("type", &CUnit::GetType, &CUnit::SetType)
        .property("model", &CUnit::GetModel, &CUnit::SetModel)
        .def("GetVar", &CUnit::GetVar),
        class_<CHero, CUnit>("CHero")
        .property("hash", &CHero::GetHash)
        .property("x", &CHero::GetX)
        .property("y", &CHero::GetY)
        .property("isMorphed", &CHero::IsMorphed)
        .property("type", &CHero::GetType, &CHero::SetType)
        .property("model", &CHero::GetModel, &CHero::SetModel)
        .def("SetActionState", &CHero::SetActionState)
        .def("GetSpell", &CHero::GetSpell)
        .def("CastSpell", &CHero::_CastSpell)
        .def("stop", &CHero::Stop)
        .def("SetupSpell", &CHero::SetupSpell)
        .def("ApplyBuff", &CHero::ApplyBuff)
        .def("QueueSpell", &CHero::QueueSpell),
        class_<CSpellWork>("CSpellWork")
        .def(constructor<CHero *, DWORD>())
        .def(constructor<CHero *, DWORD, float, float>())
        .def_readwrite("stop", &CSpellWork::bStop)
        .def_readwrite("iUpdatePos", &CSpellWork::iUpdatePos)
        .def_readwrite("setFacing", &CSpellWork::bSetFacing)
        .def_readwrite("animation", &CSpellWork::bAnimation)
        .def_readwrite("manaCost", &CSpellWork::fManaCost)
        .def_readwrite("cooldown", &CSpellWork::fCooldown)
        .def_readwrite("fReleaseCast", &CSpellWork::fReleaseCast)
        //.def_readwrite("animation", &CSpellWork::animation)
        .def("SetTeleport", &CSpellWork::SetTeleport)
        .def("AddParticle", &CSpellWork::AddParticle)
        .def("AddParticle", &CSpellWork::AddTargetParticle)
        .def("AddMissile", &CSpellWork::AddMissile)
        .def("AddSpell", &CSpellWork::AddSpell)
        .def("AddBuff", &CSpellWork::AddBuff)
        .def("AddSpellSet", &CSpellWork::AddSpellSet)
    ];
    module(L)[
        class_<CLoLPacket>("CLoLPacket")
        .def(constructor<BYTE>())
        .def("Block", &CLoLPacket::Block)
        .def("Hide", &CLoLPacket::Hide)
        .def("Encode1", &CLoLPacket::Encode1)
        .def("Encode2", &CLoLPacket::Encode2)
        .def("Encode4", &CLoLPacket::Encode4)
        .def("EncodeF", &CLoLPacket::EncodeF)
        .def("EncodeStr", &CLoLPacket::EncodeStrL)
        .def("Decode1", &CLoLPacket::Decode1)
        .def("Decode2", &CLoLPacket::Decode2)
        .def("Decode4", &CLoLPacket::Decode4)
        .def("DecodeF", &CLoLPacket::DecodeF)
        .def("DecodeStr", &CLoLPacket::DecodeStr)
        .def("Replace1", &CLoLPacket::Replace1)
        .def("Replace2", &CLoLPacket::Replace2)
        .def("Replace4", &CLoLPacket::Replace4)
        .def("ReplaceF", &CLoLPacket::ReplaceF)
        .def("skip", &CLoLPacket::skip)
        .def("getRemaining", &CLoLPacket::getRemaining)
        .def("setPos", &CLoLPacket::setPos)
        .property("header", &CLoLPacket::GetHeader)
        .def_readonly("bBlock", &CLoLPacket::bBlock)
        .def_readonly("size", &CLoLPacket::size)
        .def_readwrite("pos", &CLoLPacket::pos)
        .def_readwrite("channel", &CLoLPacket::dwArg1)
        .def_readwrite("flag", &CLoLPacket::dwArg2)
        .def_readwrite("pIndex", &CLoLPacket::dwPIndex),
        class_<CMissile>("CMissile")
        .def(constructor<const char *>())
        .def_readonly("netId1", &CMissile::netId1)
        .def_readonly("netId2", &CMissile::netId2)
        .def_readwrite("dwHash", &CMissile::dwHash)
        .def_readwrite("fromX", &CMissile::fromX)
        .def_readwrite("fromY", &CMissile::fromY)
        .def_readwrite("toX", &CMissile::toX)
        .def_readwrite("toY", &CMissile::toY)
        .def_readwrite("flag1", &CMissile::wFlag1)
        .def_readwrite("flag2", &CMissile::wFlag2)
        .def_readwrite("channelTime", &CMissile::fChannelTime)
        .def_readwrite("delay", &CMissile::fDelay)
        .def_readwrite("visible", &CMissile::fVisible)
        .def_readwrite("cooldown", &CMissile::fCooldown)
        .def_readwrite("mana", &CMissile::fMana)
        .def_readwrite("hash2", &CMissile::hash2),
        class_<EmitParticlesPacket>("ParticleEmiter")
        .def(constructor<>())
        .def("AddParticle", &EmitParticlesPacket::AddParticle)
        .def("AddParticle", &EmitParticlesPacket::AddParticle2)
        .def("Send", &SendPE)
    ];
    //setGlobal("sh", &sh);
    //setGlobal("ZERO",0);
    setGlobal("_Q", 0);
    setGlobal("_W", 1);
    setGlobal("_E", 2);
    setGlobal("_R", 3);
    ExecuteFunction("_G.environment = {}\r\nfunction setEnv(env) _ENV = _G.environment[env]; end");
}

void CLuaVM::UnloadScripts() {
    vEnvironments.clear();
    bLoaded = FALSE;
    NewVM();
}

luabind::object CLuaVM::GetFnc(char *szPath, char *szFnc) {
    luabind::object ret;
    FILE *f;
    fopen_s(&f, szPath, "rb");
    if(f) {
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buffer = (char *)malloc(size + 1);
        buffer[size] = 0;
        fread(buffer, 1, size, f);
        fclose(f);
        {
            lua_pushlstring(L, buffer, size);
            luabind::object obj(luabind::from_stack(L, -1));
            globals(L)["raw_script"] = obj;
            std::string str = " _G.env = setmetatable({}, {__index = _G}) _ENV = _G.env local script, error = load(raw_script,nil,nil,_ENV) if script ~= nil then local result, error = pcall(script) if not result then print(error) end else print(error) end";
            if(ExecuteFunction(str.c_str())) {
                globals(L)["env"]["shared"] = share_table;
                ret = globals(L)["env"][(const char *)szFnc];
            }
            lua_pop(L, -1);
        }
        free(buffer);
    }
    return ret;
}

CLuaVM::CLuaVM() {
    #ifdef ENABLE_LUA
    L = NULL;
    bLock = FALSE;
    bLoaded = FALSE;
    bRunning = false;
    NewVM();
    #endif
}

CLuaVM::~CLuaVM() {
    ReleaseVM();
}

void CLuaVM::Collect() {
    lua_gc(L, LUA_GCCOLLECT, 0);
}

void CLuaVM::NewVM() {
    ReleaseVM();
    L = luaL_newstate();
    luaL_openlibs(L);
    register_modules();
}

void CLuaVM::ReleaseVM() {
    if(L) {
        lua_gc(L, LUA_GCCOLLECT, 0);// Collect Garbage
        lua_close(L);
    }
    L = NULL;
}

template<class T>
void CLuaVM::setGlobal(const char *szGlobal, T obj) {
    luabind::globals(L)[szGlobal] = obj;
}

//#include "CApp.h"
/*#include <fstream>
BOOL CLuaVM::LoadFile(char *szFile) {
    BOOL bRet = FALSE;
    char szPath[MAX_PATH];
    //sprintf(szPath, "%s\\scripts\\%s", gApp.szDllPath, szFile);
    FILE *f;
    fopen_s(&f, szPath, "rb");
    if(f) {
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buffer = (char *)malloc(size + 1);
        buffer[size] = 0;
        fread(buffer, 1, size, f);
        fclose(f);
        bRet = LoadScript(szFile, buffer, size);
        free(buffer);
    }
    return bRet;
}
BOOL CLuaVM::LoadScript(char *szName, char *szScript, UINT len, UINT ID / * = -1 * /) {
    BOOL bRet = FALSE;
    if(ID != -1) { //do args
    }
    lua_pushlstring(L, szScript, len);
    luabind::object obj(luabind::from_stack(L, -1));
    globals(L)["raw_script"] = obj;
    std::string str = "FILE_NAME = '" + std::string(szName) + "' _G.environment[FILE_NAME] = setmetatable({}, {__index = _G}) _ENV = _G.environment[FILE_NAME] FILE_NAME = _G.FILE_NAME local script, error = load(raw_script,FILE_NAME,nil,_ENV) if script ~= nil then local result, error = pcall(script) if not result then print(error) end else print(error) end";
    if(ExecuteFunction(str.c_str())) {
        object env = globals(L)["environment"][(const char *)szName];
        vEnvironments.push_back(env);
        bRet = TRUE;
    }
    globals(L)["raw_script"] = luabind::nil;
    return bRet;
}*/