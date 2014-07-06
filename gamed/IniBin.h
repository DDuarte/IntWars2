#ifndef INIBIN_H_
#define INIBIN_H_

#include <Windows.h>
#include <map>
#include <vector>
#include <string>
#include "RiotArchiveFileHandler.h"


extern DWORD getInibinNumericKey(char *szSection, char *szVar);

class CReader {
    public:
        char *buffer;
        UINT nSize;
        UINT nPos;
        CReader(char *_buffer, UINT _nSize) {
            buffer = _buffer;
            nSize = _nSize;
            nPos = 0;
        }
        template<typename T>
        T read() {
            T var = *reinterpret_cast<T *>(buffer + nPos);
            nPos += sizeof(T);
            return var;
        }
};

class CIniVar {
    public:
        DWORD nVal;
        float fVal;
        std::string str;
        CIniVar() {
            nVal = 0;
            fVal = 0;
            str = std::string();
        }
        CIniVar(DWORD _nVal) : CIniVar() {
            nVal = _nVal;
        }
        CIniVar(WORD _nVal) : CIniVar() {
            nVal = _nVal;
        }
        CIniVar(BYTE _nVal) : CIniVar() {
            nVal = _nVal;
        }
        CIniVar(float _fVal) : CIniVar() {
            fVal = _fVal;
        }
        CIniVar(char *szValue) : CIniVar() {
            str = szValue;
        }
        operator int() {
            return ToInt();
        }
        operator DWORD() {
            return ToInt();
        }
        operator float() {
            return ToFloat();
        }
        operator std::string() {
            return ToString();
        }
        int type() {
            if(str.length()) { return 2; }
            if(fVal) { return 1; }
            return 0;
        }
        std::string ToString() {
            if(type() == 0) { return std::to_string(nVal); }
            if(type() == 1) { return std::to_string(fVal); }
            return str;
        }
        float ToFloat() {
            if(str.length()) {
                return std::stof(str);
            }
            if(fVal == 0 && nVal != 0) {
                return nVal;
            }
            return fVal;
        }
        int ToInt() {
            if(str.length()) {
                return std::stoi(str);
            }
            //if(fVal) { return (DWORD)fVal; }
            return nVal;
        }
};
class CIniBin {
    public:
        char *szPath;
        std::map<DWORD, CIniVar> mMap;
        CIniBin() {
            szPath = NULL;
        }
        operator bool() {
            return mMap.size() > 0;
        }
        CIniBin(char *_szPath) {
            szPath = _szPath;
            Read();
        }
        CIniBin(RiotArchiveFileEntry *f) {
            if(f) {
                auto extracted = f->extractData();
                _Read(extracted.data(), extracted.size());
            }
        }
        CIniBin(RiotArchiveFileHandler *handler, char *szPath) : CIniBin((*handler)[szPath]) {
        }
        bool Read();
        bool _Read(char *lpBuffer, DWORD nSize);
        CIniVar GetVar(char *szSection, char *szVar) {
            return mMap[getInibinNumericKey(szSection, szVar)];
        }
};

#endif