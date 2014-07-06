#include "stdafx.h"
#include "RiotArchiveFile.h"
#include <windows.h>
#include <fstream>
#include <string>
#include <hash_map>
#define BUF_SIZE 4096


RiotArchiveFile::RiotArchiveFile(string path) {
    this->path = path;
    this->magicNumber = false;
}


RiotArchiveFile::~RiotArchiveFile(void) {
}

bool RiotArchiveFile::parseFile() {
    if(this->magicNumber == false) {
        FILE *descriptorHandle;
        string inputFilePath = string(GetArchivesDir()).append(path);
        if(fopen_s(&descriptorHandle, inputFilePath.c_str(), "rb") != 0) { return false; }
        if(fread(&(this->magicNumber), sizeof(long), 1, descriptorHandle) != 1) { return false; }
        if(fread(&(this->version), sizeof(long), 1, descriptorHandle) != 1) { return false; }
        if(fread(&(this->managerIndex), sizeof(long), 1, descriptorHandle) != 1) { return false; }
        long fileListOffset;
        if(fread(&fileListOffset, sizeof(long), 1, descriptorHandle) != 1) { return false; }
        long pathListOffset;
        if(fread(&pathListOffset, sizeof(long), 1, descriptorHandle) != 1) { return false; }
        vector<string> pathList;
        fseek(descriptorHandle, pathListOffset, SEEK_SET);
        long pathListSize;
        if(fread(&pathListSize, 4, 1, descriptorHandle) != 1) { return false; }
        long pathListCount;
        if(fread(&pathListCount, 4, 1, descriptorHandle) != 1) { return false; }
        for(int i = 0; i < pathListCount; i++) {
            long pathOffset, pathLength;
            fseek(descriptorHandle, pathListOffset + i * 8 + 8, SEEK_SET);
            if(fread(&pathOffset, 4, 1, descriptorHandle) != 1) { return false; }
            if(fread(&pathLength, 4, 1, descriptorHandle) != 1) { return false; }
            pathLength = pathLength - 1;
            fseek(descriptorHandle, pathListOffset + pathOffset, SEEK_SET);
            char *pathStringbuf = new char[pathLength];
            if(fread(pathStringbuf, 1, pathLength, descriptorHandle) != pathLength) { return false; }
            string pathString = string(pathStringbuf, pathLength);
            delete [] pathStringbuf;
            replace(pathString.begin(), pathString.end(), '/', '\\');
            pathList.push_back(pathString);
        }
        fseek(descriptorHandle, fileListOffset, SEEK_SET);
        long numberOfEntries;
        if(fread(&numberOfEntries, 4, 1, descriptorHandle) != 1) { return false; }
        for(int i = 0; i < numberOfEntries; i++) {
            long pathHash, dataOffset, dataSize, pathListIndex;
            if(fread(&pathHash, 4, 1, descriptorHandle) != 1) { return false; }
            if(fread(&dataOffset, 4, 1, descriptorHandle) != 1) { return false; }
            if(fread(&dataSize, 4, 1, descriptorHandle) != 1) { return false; }
            if(fread(&pathListIndex, 4, 1, descriptorHandle) != 1) { return false; }
            this->files[pathList[pathListIndex]] = RiotArchiveFileEntry(this->path, pathList[pathListIndex], dataSize, dataOffset);
        }
        fclose(descriptorHandle);
    }
    return true;
}

string RiotArchiveFile::getPath() {
    return this->path;
}

long RiotArchiveFile::getMagicNumber() {
    this->parseFile();
    return this->magicNumber;
}

long RiotArchiveFile::getVersion() {
    this->parseFile();
    return this->version;
}

long RiotArchiveFile::getManagerIndex() {
    this->parseFile();
    return this->version;
}

hash_map<string, RiotArchiveFileEntry> RiotArchiveFile::getFiles() {
    this->parseFile();
    return this->files;
}

RiotArchiveFileEntry RiotArchiveFile::getFile(string filePath) {
    this->parseFile();
    return this->files[filePath];
}

bool RiotArchiveFile::extract(string targetDirectory) {
    if(this->parseFile() == false) { return false; }
    for(std::hash_map<string, RiotArchiveFileEntry>::iterator it = this->files.begin(); it != this->files.end(); ++it) {
        if(it->second.extract(targetDirectory) == false) { return false; }
    }
    return true;
}

FILE *RiotArchiveFile::getDataFileHandle() {
    this->parseFile();
    FILE *dataFileHandle;
    fopen_s(&dataFileHandle, (this->path + ".dat").c_str(), "rb");
    return dataFileHandle;
}
