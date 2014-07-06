#include "stdafx.h"
#include "RiotArchiveFileHandler.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <list>
#include <iterator>
#include <sstream>
#include <fstream>
#include <algorithm>

RiotArchiveFileHandler::RiotArchiveFileHandler(void) {
    if(!this->ReadCache()) { CreateCache(); }  //ToDo: Multithreading
}

RiotArchiveFileHandler::~RiotArchiveFileHandler(void) {
}

bool RiotArchiveFileHandler::ReadCache() {
    ifstream input("RAFCache.dat");
    if(!input.is_open()) {  //Create File if not exist
        input.close();
        ofstream file("RAFCache.dat");
        if(file && file.is_open()) { file.close(); }
        input.open("RAFCache.dat");
    }
    if(input.is_open() && input.good()) {
        this->files.clear();
        char buf[MAX_PATH];
        input >> buf;
        if(string(buf).compare(this->GetArchiveDirList().front()) == 0) {
            while(input.good()) {
                RiotArchiveFileEntry Entry = RiotArchiveFileEntry();
                input >> Entry;
                if(!Entry.getPath().empty()) {
                    std::string path = Entry.getPath();
                    std::transform(path.begin(), path.end(), path.begin(), ::tolower);;
                    this->files[path] = Entry;
                }
            }
        } else { input.close(); return false; }
    } else { return false; }
    input.close();
    return true;
}

bool RiotArchiveFileHandler::CreateCache() {
    ofstream output("RAFCache.dat");
    if(output.is_open()) {
        this->files = this->GetMergedFileList();
        output << version().c_str() << std::endl;
        for(std::unordered_map<string, RiotArchiveFileEntry>::const_iterator iterator = this->files.begin(), end = this->files.end(); iterator != end; ++iterator) {
            if(output.good()) {
                output << iterator->second;
            } else { return false; }
        }
    } else { return false; }
    output.close();
    return true;
}


int RiotArchiveFileHandler::size() {
    return this->files.size();
}

std::string RiotArchiveFileHandler::version() {
    return this->GetArchiveDirList().front();
}

std::list<std::string> RiotArchiveFileHandler::GetArchiveDirList() {
    std::string folder = this->GetArchivesDir();
    std::list<std::string> archives;
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(folder.append("*").c_str(), &ffd);
    do {
        if((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ffd.cFileName[0] != '.') {
            archives.push_back(ffd.cFileName);
        }
    } while(FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);
    archives.sort([](std::string first, std::string second) -> bool {
        replace(first.begin(), first.end(), '.', '0');
        replace(second.begin(), second.end(), '.', '0');
        return atol(first.c_str()) > atol(second.c_str());
    });
    return archives;
}

std::list<std::string> RiotArchiveFileHandler::GetArchiveFileList() {
    std::list<std::string> archiveFiles;
    std::list<std::string> archiveDirs = this->GetArchiveDirList();
    for(std::list<std::string>::const_iterator iterator = archiveDirs.begin(), end = archiveDirs.end(); iterator != end; ++iterator) {
        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile((this->GetArchivesDir() + (*iterator)).append("\\*").c_str(), &ffd);
        do {
            string fileName = string(ffd.cFileName);
            if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fileName.substr(fileName.size() - 4, 4) == string(".raf")) {
                archiveFiles.push_back(string(*iterator).append("\\").append(ffd.cFileName).c_str());
            }
        } while(FindNextFile(hFind, &ffd) != 0);
        FindClose(hFind);
    }
    return archiveFiles;
}

std::unordered_map<string, RiotArchiveFileEntry> RiotArchiveFileHandler::GetMergedFileList() {
    std::unordered_map<string, RiotArchiveFileEntry> mergedFileList;
    std::list<std::string> archiveFiles = this->GetArchiveFileList();
    for(std::list<std::string>::const_iterator iterator = archiveFiles.begin(), end = archiveFiles.end(); iterator != end; ++iterator) {
        RiotArchiveFile cArchive = RiotArchiveFile(*iterator);
        hash_map<string, RiotArchiveFileEntry> files = cArchive.getFiles();
        for(std::unordered_map<string, RiotArchiveFileEntry>::const_iterator it = files.begin(), end = files.end(); it != end; ++it) {
            if(mergedFileList.find(it->first) == mergedFileList.end()) {
                std::string path = it->first;
                std::transform(path.begin(), path.end(), path.begin(), ::tolower);
                mergedFileList[path] = it->second;
            } //insert if not already exists
        }
    }
    return mergedFileList;
}

RiotArchiveFileEntry *RiotArchiveFileHandler::operator[](std::string path) {
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);
    auto it = this->files.find(path);
    if(it == this->files.end()) {
        #ifdef _DEBUG
        //If you see this, you tried to find a nonexistent file. Check the path.
        DebugBreak();
        #endif
        return nullptr;
    }
    return &it->second;
}
