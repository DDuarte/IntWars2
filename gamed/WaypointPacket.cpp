#include "stdafx.h"
#include "WaypointPacket.h"
#include "common.h"
//todo: check this packet
WaypointPacket::WaypointPacket(unsigned int nwId, std::vector<Vector2f> waypoints) : SendablePacket(CHL_LOW_PRIORITY, 1) {
    this->write<unsigned char>(PKT_S2C_Waypoints);
    this->write<unsigned int>(nwId);
    //count of more data:
    unsigned short moreDataLen = 0 ; //may not exceed 0x1FFF
    this->write<unsigned short>(moreDataLen & 0x1FFF);
    //write more data (unknown what it is)
    for(int i = 0; i < (moreDataLen & 0x1FFF); i++) {
        this->write<unsigned char>(0);
    }
    this->write<float>(0); //unknown
    this->write<float>(0); //unknown
    this->write<float>(0); //unknown
    this->write<float>(1); //unknown
    this->write<float>(0); //unknown
    this->write<float>(0); //unknown
    this->write<float>(0); //unknown
    this->write<unsigned int>(clock());
    this->write<unsigned short>(waypoints.size() * 2);
    this->write<unsigned int>(nwId);
    this->writeEncoded<std::vector<Vector2f>>(waypoints);
}


WaypointPacket::~WaypointPacket() {
}