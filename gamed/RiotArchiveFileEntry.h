#pragma once
#include <iostream>
#include "RiotArchiveFileObject.h"
using namespace std;
class RiotArchiveFileEntry : protected RiotArchiveFileObject {
    public:
        RiotArchiveFileEntry();//dummy for std::map
        RiotArchiveFileEntry(string riotArchiveFilePath, string path, long dataSize, long dataOffset);
        ~RiotArchiveFileEntry(void);
        std::string RiotArchiveFileEntry::getRiotArchiveFilePath() const;
        std::string RiotArchiveFileEntry::getPath() const;
        long RiotArchiveFileEntry::getDataSize() const;
        long RiotArchiveFileEntry::getDataOffset() const;
        bool RiotArchiveFileEntry::extract(std::string path) const;
        std::vector<char> RiotArchiveFileEntry::extractData() const;
        friend ostream &operator<< (ostream &out, const RiotArchiveFileEntry &per);
        friend istream &operator>> (istream &out, RiotArchiveFileEntry &per);

    private:
        long dataSize;
        long dataOffset;
        std::string path;
        std::string riotArchiveFilePath;
};

