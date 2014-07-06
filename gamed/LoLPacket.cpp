#include "stdafx.h"
#include "LoLPacket.h"

CLoLPacket::CLoLPacket(BYTE bHeader) : CRPCBuffer() {
    dwPIndex = dwIndex++;
    dwArg1 = 1;
    dwArg2 = 0;
    bBlock = FALSE;
    bBatched = FALSE;
    bHide = FALSE;
    this->bHeader = bHeader;
    Encode1(bHeader);
}

CLoLPacket::CLoLPacket(BYTE bHeader, char *lpBuffer, UINT nSize) : CRPCBuffer(lpBuffer, nSize) {
    dwPIndex = dwIndex++;
    dwArg1 = 1;
    dwArg2 = 0;
    bBlock = FALSE;
    bBatched = FALSE;
    bHide = FALSE;
    this->bHeader = bHeader;
}

CLoLPacket::CLoLPacket(char *lpBuffer, UINT nSize, DWORD dwArg1, DWORD dwArg2) : CRPCBuffer(lpBuffer, nSize) {
    dwPIndex = dwIndex++;
    this->dwArg1 = dwArg1;
    this->dwArg2 = dwArg2;
    bHeader = lpBuffer[0];
    bBlock = FALSE;
    bBatched = FALSE;
    bHide = FALSE;
}

BYTE CLoLPacket::GetHeader() {
    return bHeader;
}

void CLoLPacket::Block() {
    bBlock = TRUE;
    bHeader = 0xFF;
    //buffer[0] = bHeader;
}

void CLoLPacket::Hide() {
    bHide = TRUE;
}
