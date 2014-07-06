#include "stdafx.h"
#include "WaypointGroupPacket.h"
//todo: add queue like in statsupdate
//todo: untested packet
WaypointGroupPacket::WaypointGroupPacket(std::unordered_map<unsigned int, std::vector<Vector2f>> waypointGroups) : SendablePacket(CHL_LOW_PRIORITY, 1) {
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


WaypointGroupPacket::~WaypointGroupPacket() {
}