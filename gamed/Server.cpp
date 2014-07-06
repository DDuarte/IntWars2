#include "stdafx.h"
#include "Server.h"
#include "RafDB.h"
#include "LuaVM.h"
#include "Packets.h"

CServerManager *CServerManager::_instance = nullptr;

bool WorkItem::IsReady() {
    return GetTime() < server->GetTime();
}
void WorkItem::Execute() {
    fLastTime = server->GetTime();
    if(func) {
        func();
    }
}

double WorkItem::GetTime() {
    return fLastTime + fInterval;
}

bool WorkItem::IsValid() {
    return this && bDrop == false;
}

void WorkItem::Drop() {
    if(IsValid()) {
        bDrop = true;
    }
}

WorkItem::WorkItem(float _time, float _interval, std::function<void()> _func, bool _bRecurrent) {
    fLastTime = _time;
    fInterval = _interval;
    func = _func;
    bRecurrent = _bRecurrent;
    bDrop = false;
    bSleep = false;
}

void CServerManager::Update() {
    /*char szTitle[124];
    sprintf(szTitle, "%f [%i] %f", server->GetTime(), dequeWork.size(), dequeWork.size() ? dequeWork.top().GetTime() : 0);
    SetConsoleTitle(szTitle);*/
    while(!dequeWork.empty()) {
        auto w = dequeWork.top();
        if(w->bDrop) {
            dequeWork.pop();
            delete w;//delete?
            continue;
        }
        if(!w->IsReady()) {
            break;
        }
        dequeWork.pop();
        w->Execute();
        if(w->bRecurrent) {
            dequeWork.push(w);
        } else {
            delete w;//delete?
        }
    }
}
WorkItem *CServerManager::ScheduleWork(float fStartTime, float fDelay, std::function<void()> fnc, bool bRecurrent) {
    WorkItem *w = new WorkItem(fStartTime, fDelay, fnc, bRecurrent);
    dequeWork.push(w);
    return w;
}
WorkItem *CServerManager::ScheduleWork(float fDelay, std::function<void()> fnc) {
    return ScheduleWork(GetTime(), fDelay, fnc);
}
CServerManager::CServerManager(NetworkListener *listener) {
    _instance = this;
    srand(time(0));
    Logging->writeDate();
    RafDB::Init();
    this->gameMap = GameMap::Create(SUMMONERS_RIFT);
    //..
    this->listener = listener;
    fLoadStart = 0;
    fGameStart = 0;
    blueTeam = new CTeam(BLUE);
    redTeam = new CTeam(RED);
    luaVM = new CLuaVM();
    //...
    ScheduleWork(10, 0, [this]() { //Release Towers
        this->listener->_handler->ReleaseTowers();
    });
    ScheduleWork(0, 10, [this]() { //Update game timer
        GameTimer timer(GetGameTime()); //0xC0
        this->listener->_handler->broadcastPacket(reinterpret_cast<uint8 *>(&timer), sizeof(GameTimer), CHL_S2C);
    }, true);
    //float fGoldGain = 9.5f; //ai_AmbientGoldAmount = 9.500; Gold granted on ambient gold tick
    ScheduleWork(10, 0.5f, []() { //Passive Gold Increase
        for(auto p : CObjectManager::GetPlayers()) {
            p->AddGold(9.5f / 10);
            p->RegenTick();
        }
    }, true);
    /*CIniBin bin = RafDB::GetItem(1039);
    int nPrice = bin.GetVar("Data", "Price");
    printf("[%i] Inibin[%i]: %f\r\n", nPrice, 1039, nPrice * (float)bin.GetVar("Data", "SellBackModifier"));*/
    //CIniBin bin = RafDB::GetSpellByIndex("Graves", 1);
    //printf("Desc: %s\r\n", bin.GetVar("SpellData", "Description").ToString().c_str());
}

GameMap &CServerManager::GetMap() {
    return *this->gameMap;
}

float CServerManager::GetTime() {
    return GetGameTime();
}

float CServerManager::GetGameTime() {
    if(fGameStart == 0) {
        return 0;
    }
    float fTime = GetTickCount() - fGameStart;
    return fTime / 1000;
}

float CServerManager::GetLoadTime() {
    if(fLoadStart == 0) {
        return 0;
    }
    float fTime = GetTickCount() - fLoadStart;
    return fTime / 1000;
}

CServerManager *CServerManager::GetInstance() {
    return _instance;
}
