#pragma once

class RiotArchiveFileObject {
    protected:
        RiotArchiveFileObject();
        ~RiotArchiveFileObject();
        static std::string RiotArchiveFileObject::GetArchiveDirFromRegistry();
        static std::string RiotArchiveFileObject::GetRegistryValue(const std::string &location, const std::string &name);
        static std::string archivesDir;
    public:
        std::string RiotArchiveFileObject::GetArchivesDir() const;
};
