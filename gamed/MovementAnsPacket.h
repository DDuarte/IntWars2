#pragma once
#include "SendablePacket.h"

class MovementAnsPacket : public SendablePacket {
    public:
        MovementAnsPacket(unsigned int, std::vector<Vector2f>);
        MovementAnsPacket(std::unordered_map<unsigned int, std::vector < Vector2f >>);
        static void QueueUpdate(unsigned int, Vector2f pos);
        static void SendQueue();
        ~MovementAnsPacket();
};
