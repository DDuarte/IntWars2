#include "stdafx.h"
#include "MovementAnsPacket.h"
#include "common.h"


MovementAnsPacket::MovementAnsPacket(unsigned int networkID, std::vector<Vector2f> waypoints) : SendablePacket(CHL_LOW_PRIORITY, 1) {
    this->clear();
    this->write<unsigned char>(PKT_S2C_MoveAns);
    this->write<unsigned int>(0);				//netID
    this->write<unsigned int>(clock());
    this->write<unsigned short>(1);				//count
    this->write<unsigned char>(2 * waypoints.size());
    this->write<unsigned int>(networkID);
    this->writeEncoded<std::vector<Vector2f>>(waypoints);
}

MovementAnsPacket::MovementAnsPacket(std::unordered_map<unsigned int, std::vector < Vector2f >> waypointGroups) : SendablePacket(CHL_LOW_PRIORITY, 1) {
    this->write<unsigned char>(PKT_S2C_WaypointGroups);
    this->write<unsigned int>(0);				//netID
    this->write<unsigned int>(clock());
    this->write<unsigned short>(waypointGroups.size());
    for(auto waypointPair : waypointGroups) {
        auto nwID = waypointPair.first;
        auto waypoints = waypointPair.second;
        this->write<unsigned char>(waypoints.size() * 2);
        this->write<unsigned int>(nwID);
        this->writeEncoded<std::vector<Vector2f>>(waypoints);
    }
}


MovementAnsPacket::~MovementAnsPacket() {
}

#include <chrono>
#include <thread>
#include <future>
#include "Server.h"

/* Be brave, ugly code ahead */
static std::mutex updateLock;
static bool updateFired = false;
static std::unordered_map<unsigned int, Vector2f> updateQueue;
void MovementAnsPacket::QueueUpdate(unsigned int nwId, Vector2f pos) {
    updateLock.lock();
    updateQueue[nwId] = pos;
    if(updateQueue.size() >= UCHAR_MAX) {
        SendQueue();
    } else if(!updateFired) {
        updateFired = true;
        std::async(std::launch::async, []() {
            std::chrono::milliseconds delay(10);
            std::this_thread::sleep_for(delay);
            SendQueue();
            updateFired = false;
        });
    }
    updateLock.unlock();
}

void MovementAnsPacket::SendQueue() {
    updateLock.lock();
    if(updateQueue.size() > 0) {
        std::unordered_map<unsigned int, std::vector < Vector2f >> queue;
        for(auto item : updateQueue) {
            CUnit *p = static_cast<CHero *>(CObjectManager::GetUnit(item.first));
            if(!p) { continue; }
            std::vector<Vector2f> wps2D;
            for(auto waypoint3D : (*server).GetMap().Navigation().FindPath(p->GetPos(), item.second)) {
                wps2D.push_back(Vector2f(waypoint3D.x, waypoint3D.z));
            }
            p->SetWaypoints(wps2D);
            queue[item.first] = wps2D;
        }
        MovementAnsPacket packet(queue);
        listener->_handler->broadcastPacket(packet);
        updateQueue.clear();
    }
    updateLock.unlock();
}
