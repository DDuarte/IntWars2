#include "stdafx.h"
#include "SendablePacket.h"
#include "Server.h"

SendablePacket::SendablePacket(unsigned char channelNo = CHL_LOW_PRIORITY, unsigned int flag = 1U) {
    this->channelNo = channelNo;
    this->flag = flag;
}

void SendablePacket::Send(int peer) {
    server->listener->_handler->sendPacket((ENetPeer *)peer, *this);
}

void SendablePacket::Broadcast() {
    server->listener->_handler->broadcastPacket(*this);
}

unsigned char SendablePacket::getHeader() {
    return this->size() > 0 ? this->data()[0] : 0;
}
