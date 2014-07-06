#include "stdafx.h"
#include "RpcBuffer.h"
#include "LoLPacket.h"

DWORD CLoLPacket::dwIndex = 0;

#define EncodeMacro(x,y) if(pos >= size) { size += sizeof(x); buffer = (char*)realloc(buffer,size); } \
    *(x*)&buffer[pos] = y; \
    pos = size;

#define DecodeMacro(x) if(pos+sizeof(x) > size) return 0; \
    pos += sizeof(x);\
    return *(x*)&buffer[pos-sizeof(x)];

#define ReplaceMacro(x,y,nPos) if(!bCanEdit || nPos+sizeof(x) > size) return false; \
    if(lpBatch && nPos > 4) *(x*)&lpBatch[nPos-5] = y;\
    else *(x*)&buffer[nPos] = y; \
    return true;

void CRPCBuffer::Encode1(BYTE b) {
    EncodeMacro(BYTE, b);
}
void CRPCBuffer::Encode2(WORD w) {
    EncodeMacro(WORD, w);
}
void CRPCBuffer::Encode4(DWORD i) {
    EncodeMacro(DWORD, i);
}
void CRPCBuffer::EncodeF(float f) {
    EncodeMacro(float, f);
}
void CRPCBuffer::EncodeStr(char *str) {
    UINT nLen = strlen(str);
    EncodeBuffer(str, nLen);
    Encode1(0);
}
void CRPCBuffer::EncodeStr(LPCWSTR wStr) {
    static char szStr[124];
    if(WideCharToMultiByte(0, 0, wStr, -1, szStr, 124, 0, 0)) {
        EncodeStr(szStr);
    }
}
void CRPCBuffer::EncodeStrW(LPCWSTR wStr) {
    UINT nLen = wcslen(wStr);
    EncodeBuffer((char *)wStr, nLen * 2);
    Encode2(0);
}
void CRPCBuffer::EncodeBuffer(const char *buffer, int len) {
    for(int i = 0; i < len; i++)
    { Encode1(buffer[i]); }
}
BYTE CRPCBuffer::Decode1() {
    DecodeMacro(BYTE);
}
WORD CRPCBuffer::Decode2() {
    DecodeMacro(WORD);
}
DWORD CRPCBuffer::Decode4() {
    DecodeMacro(DWORD);
}
float CRPCBuffer::DecodeF() {
    DecodeMacro(float);
}
const char *CRPCBuffer::DecodeStr() {
    UINT nLen = 0;
    while(buffer[pos + nLen]) {
        if(pos + nLen > size) { return 0; }
        nLen++;
    }
    pos += nLen + 1;
    if(nLen == 0 || pos + nLen > size) { return 0; }
    const char *szStr = (const char *)&buffer[pos];
    return szStr;
}

bool CRPCBuffer::Replace1(BYTE b, INT nPos) {
    ReplaceMacro(BYTE, b, nPos);
}
bool CRPCBuffer::Replace2(WORD w, INT nPos) {
    ReplaceMacro(WORD, w, nPos);
}
bool CRPCBuffer::Replace4(DWORD i, INT nPos) {
    ReplaceMacro(DWORD, i, nPos);
}
bool CRPCBuffer::ReplaceF(float f, INT nPos) {
    ReplaceMacro(float, f, nPos);
}

CRPCBuffer::CRPCBuffer() {
    buffer = NULL;
    size = 0;
    pos = 0;
    bSkipDelete = FALSE;
    bCanEdit = TRUE;
    lpBatch = NULL;
}

CRPCBuffer::CRPCBuffer(char *szBuf, UINT nLen) {
    buffer = szBuf;
    size = nLen;
    pos = 0;
    bSkipDelete = TRUE;
    bCanEdit = TRUE;
    lpBatch = NULL;
}

CRPCBuffer::~CRPCBuffer() {
    if(!bSkipDelete && buffer) {
        free(buffer);
    }
}

void CRPCBuffer::skip(INT n) {
    pos += n;
}

INT CRPCBuffer::getRemaining() {
    return size - pos;
}

void CRPCBuffer::setPos(INT nPos) {
    if(nPos > 0) {
        pos = size - nPos;
    } else {
        pos = nPos;
    }
}

void CRPCBuffer::EncodeStrL(const char *str) {
    EncodeStr((char *)str);
}
