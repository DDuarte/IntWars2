#pragma once
#ifndef SERVER_H_
#define SERVER_H_
#include "GameMap.h"
#include "NetworkListener.h"
#include "ObjectManager.h"

class WorkItem {
    public:
        bool bDrop;
        bool bSleep;
        bool bRecurrent;
        float fLastTime;
        float fInterval;
        std::function<void()> func;
        WorkItem(float _time, float _interval, std::function<void()> _func, bool _bRecurrent);
        void Drop();
        bool IsValid();
        double GetTime();
        bool IsReady();
        void Execute();
};

struct CompareWork {
    bool operator()(WorkItem *p1, WorkItem *p2) {
        return p1->bSleep > p2->bSleep || p1->GetTime() > p2->GetTime();
    }
};

class CServerManager {
    private:
        static CServerManager *_instance;
        std::priority_queue<WorkItem *, std::vector<WorkItem * >, CompareWork> dequeWork;
    public:
        static CServerManager *GetInstance();
        NetworkListener *listener;
        std::unique_ptr<GameMap> gameMap;

        CTeam *blueTeam;
        CTeam *redTeam;

        DWORD fLoadStart;
        DWORD fGameStart;
        float GetLoadTime();
        float GetGameTime();
        float GetTime();

        CServerManager(NetworkListener *listener);
        void Update();
        WorkItem *ScheduleWork(float fStartTime, float fDelay, std::function<void()> fnc, bool bRecurrent = false);
        WorkItem *ScheduleWork(float fDelay, std::function<void()> fnc);
        GameMap &GetMap();
};
#define server (CServerManager::GetInstance())
#endif