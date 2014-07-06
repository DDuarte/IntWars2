#pragma once
#include "RpcBuffer.h"

class CLoLPacket : public CRPCBuffer {
    public:
        static DWORD dwIndex;
        BYTE bHeader;
        DWORD dwArg1;
        DWORD dwArg2;
        DWORD dwCalee;
        BOOL bBlock;
        BOOL bBatched;
        BOOL bHide;
        DWORD dwPIndex;
        CLoLPacket(BYTE bHeader);
        CLoLPacket(BYTE bHeader, char *lpBuffer, UINT nSize);
        CLoLPacket(char *lpBuffer, UINT nSize, DWORD dwArg1, DWORD dwArg2);
        BYTE GetHeader();
        void Block();
        void Hide();
};