#pragma once
#include "SendablePacket.h"
class WaypointPacket : public SendablePacket {
    public:
        WaypointPacket(unsigned int, std::vector<Vector2f>);
        ~WaypointPacket();
};

/*
B9                          -> header
9A 00 00 40                 -> nwID
17 00                       -> &1FFF = 23
15 00 03 9A 00 00 40        ->Inside_R_WAYPOINT_3

9A 00 00 40 ->nwID
40  -> blockmask?
E8 00 BA FF
01 00 00 00 ->valueMask?
0A 00
01 -> integer 1? /-> offset 23

00 00 00 00
00 00 00 00
00 00 00 00
00 00 80 3F -> float value 1.0f
00 00 00 00
00 00 00 00
00 00 00 00

02 13 31 00 -> packetNo
00 05 -> coordCount?
9A 00 00 40 -> nwID
00 00 9B 02 66 02 54 01 21 01

02 XX XX XX 00 XX nwID XX ...

-----------------------
B9
AD 00 00 40
17 00
15 00 03 AD 00 00 40

AD 00 00 40
40
AF C9 5E FF
01 01 01 00
07 00
01

00 00 00 00
00 00 00 00
00 00 00 00
00 80 3F 00
00 00 00 00
00 00 00 00
00 00 00 00
02 23 3B 00

00 05
AD 00 00 40
00 01 65 0B C0 F7 8C 99 F6 */
