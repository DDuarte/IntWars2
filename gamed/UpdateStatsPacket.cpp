#include "stdafx.h"
#include "UpdateStatsPacket.h"
#include "NetworkListener.h" //broadcastpacket

UpdateStatsPacket::UpdateStatsPacket(std::unordered_map<unsigned int, std::unordered_map<ObjectAttributeIndex, double>> objectUpdates) : SendablePacket(CHL_LOW_PRIORITY, 2) {
    /* Merge into one packet */
    this->clear();
    this->write<unsigned char>(PKT_S2C_CharStats);			//header
    this->write<unsigned int>(0);							//netId
    this->write<unsigned int>(clock());						//packetNo
    this->write<unsigned char>(objectUpdates.size());		//count of updates
    for(auto p : objectUpdates) {
        writeStatsForObject(p.first, p.second);
    }
}

UpdateStatsPacket::~UpdateStatsPacket() {
}

void UpdateStatsPacket::writeStatsForObject(unsigned int networkId, std::unordered_map<ObjectAttributeIndex, double> attributes) {
    /* acquire values */
    unsigned char blockMask = 0;
    unsigned int valueMasks[8] = { 0 };
    std::pair<ObjectAttributeIndex, double> values[8][32];
    for(auto attribute : attributes) {
        auto index = attribute.first;
        blockMask |= index.getMajorMask();
        valueMasks[index.major] |= index.getMinorMask();
        values[index.major][index.minor] = attribute;
    }
    /* compress/encode */
    this->write<unsigned char>(blockMask);
    this->write<unsigned int>(networkId);
    for(unsigned char blockIndex = 0; blockIndex < 8; blockIndex++) {
        unsigned char currentBlockMask = 1 << blockIndex;
        if(!(currentBlockMask & blockMask)) {
            continue;
        }
        unsigned int blockValueMask = valueMasks[blockIndex];
        this->write<unsigned int>(blockValueMask);
        int blockSizePos = getPos();
        this->write<unsigned char>(0);
        for(unsigned char valueIndex = 0; valueIndex < 32; valueIndex++) {
            unsigned int currentValueMask = 1 << valueIndex;
            if(!(blockValueMask & currentValueMask)) {
                continue;
            }
            std::pair<ObjectAttributeIndex, double> attribute = values[blockIndex][valueIndex];
            if(attribute.first.type == FLOAT_VALUE) {
                this->writeEncoded<float>(attribute.second);
            } else {
                this->writeEncoded<unsigned long long>(attribute.second);
            }
        }
        this->writeAt<unsigned char>(getPos() - blockSizePos - 1, blockSizePos);
    }
}

#include <chrono>
#include <thread>
#include <future>

/* Be brave, ugly code ahead */
static std::mutex updateLock;
static bool updateFired = false;
static std::unordered_map<unsigned int, std::unordered_map<ObjectAttributeIndex, double>> updateQueue;
void UpdateStatsPacket::QueueUpdate(unsigned int nwId, ObjectAttributeIndex index, double value) {
    updateLock.lock();
    auto updates = updateQueue.find(nwId);
    if(updates != updateQueue.end()) {
        updates->second[index] = value;
        //std::cout << "[0] Queued Stat update with value: " << value << std::endl;
    } else {
        std::unordered_map<ObjectAttributeIndex, double> attributes;
        attributes[index] = value;
        updateQueue[nwId] = attributes;
        //std::cout << "[1] Queued Stat update with value: " << value << std::endl;
    }
    //std::cout << updateQueue[nwId].size() << " attribute updates of this object in total." << std::endl;
    if(updateQueue.size() >= UCHAR_MAX) {
        SendQueue();
    } else if(!updateFired) {
        updateFired = true;
        //just wait a little and then send them batched together
        //todo: make this batched stuff in a dedicated thread loop
        std::async(std::launch::async, []() {
            //std::cout << "Started Async sending..." << std::endl;
            std::chrono::milliseconds delay(100);
            std::this_thread::sleep_for(delay);
            SendQueue();
            updateFired = false;
        });
    }
    updateLock.unlock();
}


void UpdateStatsPacket::SendQueue() {
    updateLock.lock();
    if(updateQueue.size() > 0) {
        UpdateStatsPacket packet(updateQueue);
        /*
        std::cout << "Sending updates for " << updateQueue.size() << " objects." << std::endl;
        for(auto o : updateQueue) {
            std::cout << "\t" << std::hex << o.first << ":" << std::endl;
            for(auto a : o.second) {
                std::cout << "\t\t" << std::dec << static_cast<int>(a.first.major) << "|" << static_cast<int>(a.first.minor) << " = " << a.second << std::endl;
            }
        }
        std::cout << packet.toString().c_str() << std::endl;
        */
        listener->_handler->broadcastPacket(packet);
        updateQueue.clear();
    }
    updateLock.unlock();
}
