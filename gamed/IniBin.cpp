#include "stdafx.h"
#include "IniBin.h"
DWORD getInibinNumericKey(char *szSection, char *szVar) {
    DWORD dwHash = 0;
    if(szSection) {
        for(UINT i = 0; i < strlen(szSection); i++) {
            dwHash = tolower(szSection[i]) + 65599 * dwHash;
        }
        dwHash = 65599 * dwHash + 42;
        if(szVar) {
            for(UINT i = 0; i < strlen(szVar); i++) {
                dwHash = tolower(szVar[i]) + 65599 * dwHash;
            }
        }
    }
    return dwHash;
}

#define READ_KEYS() vKeys.clear(); \
    nCount = b.read<WORD>(); \
    for (UINT i = 0; i < nCount; i++) {\
        vKeys.push_back(b.read<DWORD>()); \
    }
#define X_MACRO(b,val) if((dataFormat & b) != 0) { \
        READ_KEYS() \
        for (UINT i = 0; i < nCount; i++) {\
            mMap[vKeys[i]] = val; \
        } \
    }
bool CIniBin::Read() {
    FILE *f;
    fopen_s(&f, szPath, "rb");
    if(f) {
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buffer = (char *)malloc(size + 1);
        buffer[size] = 0;
        fread(buffer, 1, size, f);
        fclose(f);
        //..
        _Read(buffer, size);
        free(buffer);
        return true;
    }
    return false;
}

bool CIniBin::_Read(char *lpBuffer, DWORD nSize) {
    if(!lpBuffer || nSize == 0) { return false; }
    CReader b(lpBuffer, nSize);
    BYTE bVersion = b.read<BYTE>(); //version
    DWORD oldLength = b.read<WORD>(); //oldLenght
    DWORD dataFormat = b.read<WORD>(); //dataFormat
    DWORD oldstyleOffset = nSize - oldLength;
    //for(UINT nBits = 1; nBits < 14; nBits++) {     //0x0001 to 0x1000
    DWORD nCount = 0;// b.read<WORD>();
    std::vector<DWORD> vKeys;
    X_MACRO(0x0001, b.read<DWORD>()) //u32
    X_MACRO(0x0002, (float)b.read<float>()) //float //missing precision by 5
    X_MACRO(0x0004, (float)(b.read<BYTE>() * 0.1)) //u8 / 10
    X_MACRO(0x0008, b.read<WORD>()) //u16
    X_MACRO(0x0010, b.read<BYTE>()) //u8
    if((dataFormat & 0x0020) != 0) {        //boolean
        READ_KEYS();
        BYTE bVal;
        for(UINT i = 0; i < nCount; i++) {
            bVal = (i % 8 == 0) ? b.read<BYTE>() : bVal >> 1;
            mMap[vKeys[i]] = (BYTE)(bVal & 1);
        }
    }
    if((dataFormat & 0x0040) != 0) {   //3 bytes value
        READ_KEYS();
        for(UINT i = 0; i < nCount; i++) {
            char szFormat[64];
            sprintf_s(szFormat, 64, "%i %i %i", b.read<BYTE>(), b.read<BYTE>(), b.read<BYTE>());
            mMap[vKeys[i]] = szFormat;
        }
    }
    if((dataFormat & 0x0080) != 0) {   //12 bytes value
        READ_KEYS();
        for(UINT i = 0; i < nCount; i++) {
            char szFormat[64];
            sprintf_s(szFormat, 64, "%i %i %i", b.read<DWORD>(), b.read<DWORD>(), b.read<DWORD>());
            mMap[vKeys[i]] = szFormat;
        }
    }
    X_MACRO(0x0100, b.read<WORD>()) //u16
    if((dataFormat & 0x0200) != 0) {      //8 bytes value
        READ_KEYS();
        for(UINT i = 0; i < nCount; i++) {
            char szFormat[64];
            sprintf_s(szFormat, 64, "%i %i %i %i", b.read<WORD>(), b.read<WORD>(), b.read<WORD>(), b.read<WORD>());
            mMap[vKeys[i]] = szFormat;
        }
    }
    X_MACRO(0x0400, (DWORD)b.read<DWORD>()) //u32
    if((dataFormat & 0x0800) != 0) {       //16 bytes value
        READ_KEYS();
        for(UINT i = 0; i < nCount; i++) {
            char szFormat[64];
            sprintf_s(szFormat, 64, "%i %i %i %i", b.read<DWORD>(), b.read<DWORD>(), b.read<DWORD>(), b.read<DWORD>());
            mMap[vKeys[i]] = szFormat;
        }
    }
    if((dataFormat & 0x1000) != 0) {   //string value
        READ_KEYS();
        for(UINT i = 0; i < nCount; i++) {
            DWORD nOffset = oldstyleOffset + b.read<WORD>();
            char *szString = lpBuffer + nOffset;
            mMap[vKeys[i]] = szString;
        }
    }
    return true;
}