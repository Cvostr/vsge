#pragma once

#include <string>
#include <vector>
#include <Core/DateTime.hpp>
#include <fstream>

namespace Mpi {

    class File {
    private:
        std::string mPath;

        int getPathLastDivisorIndex() const;

        void removeLastSlash();

        const std::string getPathPlatformSlash() const;

        void toPathDefaultSlash();
    public:
        File(const std::string& path);

        File(const std::string& absDirPath, const std::string fileName);

        File(const File& dir, const std::string fileName);

        const std::string getAbsolutePath() const;

        const std::string& getPath() const;

        std::string getParent() const;

        std::string getName() const;

        std::string getExtension() const;

        bool exists() const;

        bool isFile() const;

        bool isDirectory() const;

        bool isReadonly() const;

        uint64 getFileSize() const;

        bool mkdir() const;

        void remove(bool recursively = false) const;

        void rename(const std::string& newName);

        std::vector<File> listFiles() const;

        const DateTime getLastEditTime() const;

        void createNewFile() const;

        std::ifstream getIfstream(std::ios_base::openmode openmode = std::ios_base::in) const;

        std::ofstream getOfstream(std::ios_base::openmode openmode = std::ios_base::out) const;
    };


}