#include "stdafx.h"
#include "MovementReqPacket.h"


/* todo: check actual cmd and throw error if doesnt match */
MovementReqPacket::MovementReqPacket(unsigned char *buf, unsigned int len) : ReceivablePacket(buf, len) {
    auto cmd = this->read<unsigned char>();
    auto nID = this->read<unsigned int>();
    this->movementType = this->read<MoveType>();
    this->x = this->read<float>();
    this->y = this->read<float>();
    this->targetId = read<unsigned int>();
    auto coordCount = this->read<unsigned char>();
    this->netId = this->read<unsigned int>();
    this->vertices = this->readEncoded<std::vector<Vector2f>>(coordCount);
}

MovementReqPacket::~MovementReqPacket() {
}

std::vector<Vector2f> MovementReqPacket::getVertices() {
    return this->vertices;
}

unsigned int MovementReqPacket::getTargetID() {
    return this->targetId;
}
unsigned int MovementReqPacket::getNetworkID() {
    return this->netId;
}

MoveType MovementReqPacket::getMovementType() {
    return this->movementType;
}

float MovementReqPacket::getX() {
    return this->x;
}

float MovementReqPacket::getZ() {
    return this->y;
}
