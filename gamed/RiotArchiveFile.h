#pragma once
#include <iostream>
using namespace std;

#include <regex>
#include <hash_map>
#include "RiotArchiveFileEntry.h"
#include "RiotArchiveFileObject.h"

class RiotArchiveFile : protected RiotArchiveFileObject {
public:
	RiotArchiveFile(string path);
	~RiotArchiveFile(void);
	hash_map<string,RiotArchiveFileEntry> RiotArchiveFile::getFiles();
	RiotArchiveFileEntry RiotArchiveFile::getFile(string filePath);
	bool RiotArchiveFile::parseFile();
	string RiotArchiveFile::getPath();
	long RiotArchiveFile::getMagicNumber();
	long RiotArchiveFile::getVersion();
	long RiotArchiveFile::getManagerIndex();
	FILE* RiotArchiveFile::getDataFileHandle();
	bool extract(string dir);
	string path;
private:
	long magicNumber;
	long version;
	long managerIndex;
	hash_map<string,RiotArchiveFileEntry> files;
};

