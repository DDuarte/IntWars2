#include "stdafx.h"
#include "RiotArchiveFileObject.h"

std::string RiotArchiveFileObject::archivesDir = GetArchiveDirFromRegistry();

RiotArchiveFileObject::RiotArchiveFileObject(void) {
}

RiotArchiveFileObject::~RiotArchiveFileObject(void) {
}

std::string RiotArchiveFileObject::GetArchivesDir() const {
    return archivesDir;
}

std::string RiotArchiveFileObject::GetRegistryValue(const std::string &location, const std::string &name) {
    HKEY key;
    TCHAR value[1024];
    DWORD bufLen = 1024 * sizeof(TCHAR);
    long ret;
    ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, location.c_str(), 0, KEY_QUERY_VALUE, &key);
    if(ret != ERROR_SUCCESS) {
        return std::string();
    }
    ret = RegQueryValueExA(key, name.c_str(), 0, 0, (LPBYTE) value, &bufLen);
    RegCloseKey(key);
    if((ret != ERROR_SUCCESS) || (bufLen > 1024 * sizeof(TCHAR))) {
        return std::string();
    }
    std::string stringValue = std::string(value, (size_t)bufLen - 1);
    size_t i = stringValue.length();
    while(i > 0 && stringValue[i - 1] == '\0') {
        --i;
    }
    return stringValue.substr(0, i);
}

std::string RiotArchiveFileObject::GetArchiveDirFromRegistry() {
    std::string regKey = GetRegistryValue("SOFTWARE\\Riot Games\\RADS", "LocalRootFolder").append("\\projects\\lol_game_client\\filearchives\\");
    replace(regKey.begin(), regKey.end(), '/', '\\');
    return regKey;
}


