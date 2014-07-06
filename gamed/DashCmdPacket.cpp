#include "stdafx.h"
#include "DashCmdPacket.h"
#include "common.h"


DashCmdPacket::DashCmdPacket(unsigned int networkID, float fSpeed, std::vector<Vector2f> waypoints) : SendablePacket(CHL_S2C, 0) {
    this->clear();
    this->write<unsigned char>(0x63);
    this->write<unsigned int>(0);				//netID
    this->write<unsigned int>(clock());
    this->write<unsigned short>(1);				//ok
    this->write<unsigned char>(2 * waypoints.size());
    this->write<unsigned int>(networkID);
    this->write<float>(fSpeed);
    this->write<unsigned int>(0);
    this->write<float>(waypoints[0].x);
    this->write<float>(waypoints[0].y);
    this->write<unsigned char>(0);
    this->write<unsigned int>(0);//targetID
    this->write<unsigned short>(0x260F);
    this->write<unsigned int>(0x2AE05C07);
    this->write<float>(waypoints[1].x);
    this->write<unsigned short>(0x3001);
    this->writeEncoded<std::vector<Vector2f>>(waypoints);
}


DashCmdPacket::~DashCmdPacket() {
}