#pragma once
#include "stdafx.h"
#include "IniBin.h"
class RafDB {
    public:
        static void Init();
        static CIniBin GetCharacter(char *szCharacter);
        static CIniBin GetSpell(char *szCharacter, char *szSpell);
        static CIniBin GetSpellByIndex(char *szCharacter, UINT nSpell);
        static CIniBin GetItem(UINT itemId);
        static RiotArchiveFileHandler *GetHandler() {
            return rafHandler;
        }
    private:
        static RiotArchiveFileHandler *rafHandler;
};