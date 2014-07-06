#include "stdafx.h"
#include "RafDB.h"
#include "Server.h"

RiotArchiveFileHandler *RafDB::rafHandler = nullptr;
void RafDB::Init() {
    if(rafHandler == nullptr) {
        Logging->write(" Waiting for RAF...");
        rafHandler = new RiotArchiveFileHandler(); //todo: resource leak!
        stringstream s;
        s << " done. " << rafHandler->size() << " files read. " << "Version " << rafHandler->version() << std::endl;
        Logging->write(s.str().c_str());
    }
}

CIniBin RafDB::GetCharacter(char *szCharacter) {
    char szPath[256];
    sprintf_s(szPath, 256, "DATA\\Characters\\%s\\%s.inibin", szCharacter, szCharacter);
    return CIniBin(rafHandler, szPath);
}
CIniBin RafDB::GetSpell(char *szCharacter, char *szSpell) {
    CIniBin bin;
    char szPath[512];
    sprintf_s(szPath, 512, "DATA\\Spells\\%s.inibin", szSpell);
    bin = CIniBin(rafHandler, szPath);
    if(!bin) {
        sprintf_s(szPath, 512, "DATA\\Characters\\%s\\Spells\\%s.inibin", szCharacter, szSpell);
        bin = CIniBin(rafHandler, szPath);
    }
    return bin;
}
CIniBin RafDB::GetSpellByIndex(char *szCharacter, UINT nSpell) {
    if(nSpell > 4) { return CIniBin(); }
    if(nSpell == 0) { nSpell++; }
    char szVar[124];
    sprintf(szVar, "Spell%i", nSpell);
    CIniBin charBin = GetCharacter(szCharacter);
    if(charBin) {
        std::string spellName = charBin.GetVar("Data", szVar);
        return GetSpell(szCharacter, (char *)spellName.c_str());
    }
    return CIniBin();
}
CIniBin RafDB::GetItem(UINT itemId) {
    char szPath[124];
    sprintf_s(szPath, 124, "DATA\\Items\\%i.inibin", itemId);
    return CIniBin(rafHandler, szPath);
}