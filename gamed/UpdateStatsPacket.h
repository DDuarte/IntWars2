#pragma once
#include "SendablePacket.h"
#include "ObjectAttributes.h"

/*
(updateCount                                ; PACKET
(blockMask|NwId                            ; OBJECTUPDATE
(valueMask|length|value|value...)    ; BLOCK
(valueMask|length|value|value...)    ; BLOCK
...                                  ; ...BLOCK
) ...                                      ; ...OBJECTUPDATE
)
Max 8 Blocks
Max 32 Values per Block
MaxSize of Block = 32 * 4 + 5 = 128 + 5
Max 255 Updates per Packet
*/

class UpdateStatsPacket : public SendablePacket {
    public:
        UpdateStatsPacket(std::unordered_map<unsigned int, std::unordered_map<ObjectAttributeIndex, double>> objectUpdates);
        ~UpdateStatsPacket();
        static void QueueUpdate(unsigned int nwId, ObjectAttributeIndex index, double value);
        static void SendQueue();
    private:
        void writeStatsForObject(unsigned int networkId, std::unordered_map<ObjectAttributeIndex, double> attributes);
};



