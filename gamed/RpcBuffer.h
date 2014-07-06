#pragma once

class CRPCBuffer {
    public:
        char *buffer;
        UINT size;
        UINT pos;
        BOOL bSkipDelete;
        BOOL bCanEdit;
        char *lpBatch;

        CRPCBuffer();
        CRPCBuffer(char *szBuf, UINT nLen);
        ~CRPCBuffer();
        void skip(INT n);
        INT getRemaining();
        void setPos(INT nPos);
        void Encode1(BYTE b);
        void Encode2(WORD w);
        void Encode4(DWORD i);
        void EncodeF(float f);
        void EncodeStr(char *str);
        void EncodeStr(LPCWSTR wStr);
        void EncodeStrW(LPCWSTR str);
        void EncodeStrL(const char *str);
        void EncodeBuffer(const char *buffer, int len);
        BYTE Decode1();
        WORD Decode2();
        DWORD Decode4();
        float DecodeF();
        const char *DecodeStr();
        bool Replace1(BYTE b, INT nPos);
        bool Replace2(WORD w, INT nPos);
        bool Replace4(DWORD i, INT nPos);
        bool ReplaceF(float f, INT nPos);
};