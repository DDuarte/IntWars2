#pragma once
#include "SendablePacket.h"
#include "Hero.h"

class CHero;
class SpellCmdPacket : public SendablePacket {
    public:
        DWORD netID;
        SpellCmdPacket();
        SpellCmdPacket(unsigned int, unsigned short, char, unsigned int, unsigned int, unsigned int, unsigned int, Vector3f, Vector3f, float, float, float, float, float);
        static SpellCmdPacket *_BuildSpell(CHero *, unsigned int, DWORD, unsigned int, float, float, float, float, float, float, float);
        static SpellCmdPacket BuildSpell(CHero *, unsigned int, DWORD, unsigned int, float, float, float, float, float, float, float);
        static SpellCmdPacket BuildSpell(CHero *, unsigned int, unsigned int, float, float, float, float, float, float, float);
        ~SpellCmdPacket();
};
