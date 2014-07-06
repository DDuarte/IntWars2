#pragma once
#include "SendablePacket.h"
#include "Packets.h"

class WaypointGroupPacket : public SendablePacket {
    public:
        WaypointGroupPacket(std::unordered_map<unsigned int, std::vector<Vector2f>>);
        ~WaypointGroupPacket();
};
