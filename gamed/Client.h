/*
IntWars playground server for League of Legends protocol testing
Copyright (C) 2012  Intline9 <Intline9@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#ifndef _CLIENT_H
#define _CLIENT_H

#include "Hero.h"

struct ClientInfo {
    ClientInfo();
    ~ClientInfo();
    void setPlayer(CHero *unit);
    uint32 getTicks();
    bool keyChecked;
    uint64 userId;
    uint32 ticks;
    uint32 netId;
    CHero *unit;
    bool bLoadScreen;

};
#define peerInfo(p) ((ClientInfo*)p->data)
#define peerUnit(p) (((ClientInfo*)p->data)->unit)

#endif