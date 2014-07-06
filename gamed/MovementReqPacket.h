#pragma once
#include "ReceivablePacket.h"
#include "common.h"

class MovementReqPacket : public ReceivablePacket {
    public:
        MovementReqPacket(unsigned char *, unsigned int);
        ~MovementReqPacket();
        unsigned int getNetworkID();
        unsigned int getTargetID();
        MoveType getMovementType();
        float getX();
        float getZ();
        std::vector<Vector2f> getVertices();
    private:
        unsigned int netId;
        unsigned int targetId;
        MoveType movementType;
        float x;
        float y;
        std::vector<Vector2f> vertices;
};