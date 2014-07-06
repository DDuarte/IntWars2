#pragma once
#include "PacketBuffer.h"
/* todo: input/ output flags */
class SendablePacket : public PacketBuffer {
    public:
        SendablePacket(unsigned char channelNo, unsigned int flag);
        virtual void encode() { }; //this must be called before sending

        unsigned char getHeader();
        unsigned char channelNo;
        unsigned int flag;
        void Send(int peer);
        void Broadcast();
    protected:
        bool isEncoded;
};

