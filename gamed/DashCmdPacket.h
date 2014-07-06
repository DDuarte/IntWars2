#pragma once
#include "SendablePacket.h"

class DashCmdPacket : public SendablePacket {
    public:
        DashCmdPacket(unsigned int, float , std::vector<Vector2f>);
        ~DashCmdPacket();
};
