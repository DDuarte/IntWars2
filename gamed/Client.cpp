#include "stdafx.h"
#include "Client.h"
#include "common.h"
#include "ObjectManager.h"

uint32 ClientInfo::getTicks() {
    ticks++;
    return ticks;
}

void ClientInfo::setPlayer(CHero *unit) {
    this->unit = unit;
    netId = unit->GetNetworkID();
    bLoadScreen = false;
}

ClientInfo::~ClientInfo() {
    if(unit) {
        unit->_peer = 0;
    }
}

ClientInfo::ClientInfo() {
    keyChecked = false;
    ticks = 0;
    unit = 0;
    bLoadScreen = false;
}
