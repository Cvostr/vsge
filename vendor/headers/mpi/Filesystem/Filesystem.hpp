#pragma once

#include <Core/Base.hpp>
#include <Core/DateTime.hpp>
#include <string>
#include <vector>

namespace Mpi{
    class Filesystem {
        public:

        static bool exists(const std::string& filePath);

        static bool isFile(const std::string& filePath);

        static bool isDirectory(const std::string& filePath);

        static bool deleteFile(const std::string& filePath);

        static uint64_t getFileSize(const std::string& filePath);

        static bool isFileReadonly(const std::string& filePath);

        static bool mkdir(const std::string& filePath);

        static bool removeDirectory(const std::string& filePath, bool deleteContents = false);

        static bool listFilesOfDirectory(const std::string& dirPath, std::vector<std::string>& paths);

        static bool getLastEditTime(const std::string& filePath, DateTime& dateTime);

        static void getAbsPath(const std::string& relativeFilePath, std::string& absoluteFilePath);

        static void getWorkingDirectory(std::string& workDirPath);

        static void setWorkingDirectory(const std::string& directoryPath);

        static bool moveFile(const std::string& oldPath, const std::string& newPath, bool overwrite = false);

        static bool copyFile(const std::string& oldPath, const std::string& newPath);
    };
}