#pragma once
#include "RiotArchiveFile.h"
#include "RiotArchiveFileEntry.h"
#include "RiotArchiveFileObject.h"
#include <iostream>
#include <string>
#include <list>
#include <unordered_map>

class RiotArchiveFileHandler : protected RiotArchiveFileObject {
    public:
        RiotArchiveFileHandler(void);
        ~RiotArchiveFileHandler(void);
        RiotArchiveFileEntry *operator[](std::string path);
        int size();
        std::string version();
        std::unordered_map<string, RiotArchiveFileEntry> GetFiles() {
            return files;
        }
    private:
        std::unordered_map<string, RiotArchiveFileEntry> files;
        bool RiotArchiveFileHandler::CreateCache(void);
        bool RiotArchiveFileHandler::ReadCache(void);
        std::list<std::string> RiotArchiveFileHandler::GetArchiveDirList();
        std::list<std::string> RiotArchiveFileHandler::GetArchiveFileList();
        std::unordered_map<string, RiotArchiveFileEntry> RiotArchiveFileHandler::GetMergedFileList();
};

