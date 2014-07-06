#include "stdafx.h"
#include "ReceivablePacket.h"
ReceivablePacket::ReceivablePacket(unsigned char *buf, unsigned int length) : PacketBuffer(buf, length) {
}

ReceivablePacket::~ReceivablePacket() {
}

unsigned char ReceivablePacket::getHeader() {
    return this->size() > 0 ? this->data()[0] : 0;
}
