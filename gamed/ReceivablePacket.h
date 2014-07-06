#pragma once
#include "PacketBuffer.h"
class ReceivablePacket : public PacketBuffer {
    public:
        ReceivablePacket(unsigned char *buf, unsigned int length);
        virtual ~ReceivablePacket();
        unsigned char getHeader();
};

