#include "stdafx.h"
#include "RiotArchiveFileEntry.h"
#include "miniz.c"
#include <windows.h>
#define RAF_CACHE_DELIMITER '|'

RiotArchiveFileEntry::RiotArchiveFileEntry() { //dummy for std::map and serialization
}

RiotArchiveFileEntry::RiotArchiveFileEntry(string riotArchiveFilePath, string path, long dataSize, long dataOffset) {
    this->riotArchiveFilePath = riotArchiveFilePath;
    this->path = path;
    this->dataSize = dataSize;
    this->dataOffset = dataOffset;
    //this->pathHash = pathHash;
}

RiotArchiveFileEntry::~RiotArchiveFileEntry(void) {
}

std::string RiotArchiveFileEntry::getRiotArchiveFilePath() const {
    return string(this->GetArchivesDir()).append(this->riotArchiveFilePath);
}

string RiotArchiveFileEntry::getPath() const {
    return this->path;
}

long RiotArchiveFileEntry::getDataSize() const {
    return this->dataSize;
}

long RiotArchiveFileEntry::getDataOffset() const {
    return this->dataOffset;
}

FILE *fopen_mkdir(const char *name, const char *mode) {
    char *mname = _strdup(name);
    int i;
    for(i = 0; mname[i] != '\0'; i++) {
        if(i > 0 && (mname[i] == '\\' || mname[i] == '/')) {
            char slash = mname[i];
            mname[i] = '\0';
            if(!(CreateDirectory(mname, NULL) || ERROR_ALREADY_EXISTS == GetLastError())) {
                return NULL;
            }
            mname[i] = slash;
        }
    }
    free(mname);
    return fopen(name, mode);
}

ostream &operator<< (ostream &out, const RiotArchiveFileEntry &per) {
    out << per.path.c_str() << RAF_CACHE_DELIMITER;
    out << per.riotArchiveFilePath.c_str() << RAF_CACHE_DELIMITER;
    out << per.dataOffset << '\t';
    out << per.dataSize << std::endl;
    return out;
}

istream &operator>> (istream &in, RiotArchiveFileEntry &per) {
    in.ignore();
    char buffer[MAX_PATH];
    in.getline(buffer, MAX_PATH, RAF_CACHE_DELIMITER);
    per.path = string(buffer);
    in.getline(buffer, MAX_PATH, RAF_CACHE_DELIMITER);
    per.riotArchiveFilePath = string(buffer);
    in >> per.dataOffset;
    in >> per.dataSize;
    return in;
}

std::vector<char> readFile(std::string path, unsigned int offset, unsigned int length) {
    std::vector<char> result;
    FILE *hFile = NULL;
    fopen_s(&hFile, path.c_str(), "rb");
    if(hFile == NULL) {
        return result;
    }
    fseek(hFile, offset, SEEK_SET);
    result.resize(length);
    int dwRead = fread(result.data(), 1, result.size(), hFile);
    result.resize(dwRead);
    fclose(hFile);
    return result;
}

std::vector<char> RiotArchiveFileEntry::extractData() const {
    std::vector<char> result;
    //Input
    string archivePath = this->getRiotArchiveFilePath().append(".dat");
    auto src = readFile(archivePath.c_str(), this->dataOffset, this->dataSize);
    //Extraction
    unsigned long pDest_len = src.size() * 2;
    unsigned char *pDest = (unsigned char *)malloc(pDest_len);
    if(pDest == NULL) { return result; }
    int r = 0;
    while(r = uncompress(pDest, &pDest_len, (unsigned char *)src.data(), src.size()) == Z_BUF_ERROR) {      //Make buffer bigger until it fits
        free(pDest);
        pDest_len *= 2;
        pDest = (unsigned char *)malloc(pDest_len);
        if(pDest == NULL) { return result; }
    }
    if(r != MZ_OK) {
        result.resize(0);
    } else {
        result.resize(pDest_len);
        memcpy(result.data(), pDest, result.size());
    }
    //Output
    free(pDest);
    return result;
}

bool RiotArchiveFileEntry::extract(string path) const {
    //Input
    auto extracted = extractData();
    //Output
    char driveA[MAX_PATH], dirA[MAX_PATH], nameA[MAX_PATH], extA[MAX_PATH];
    _splitpath(path.c_str(), driveA, dirA, nameA, extA);
    char driveB[MAX_PATH], dirB[MAX_PATH], nameB[MAX_PATH], extB[MAX_PATH];
    _splitpath(this->path.c_str(), driveB, dirB, nameB, extB);
    string outputPath = string(driveA).append(dirA);//path.append(); //ToDo: We need something better here
    if(nameA[0] == '\0') { outputPath = outputPath.append(nameB).append(extB); }
    else { outputPath = outputPath.append(nameA).append(extA); }
    FILE *oFile = fopen_mkdir(outputPath.c_str(), "wb");
    if(oFile == NULL) { return false; }
    fwrite(extracted.data(), 1, extracted.size(), oFile);
    if(ferror(oFile)) { return false; }
    fclose(oFile);
    return true;
}
