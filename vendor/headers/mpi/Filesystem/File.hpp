#pragma once

#include <string>
#include <vector>
#include <Core/DateTime.hpp>
#include <fstream>
#include <array>

namespace Mpi {

    class File {
    private:
        std::string mPath;

        mpi_pos_t getPathLastDivisorIndex() const;

        void removeLastSlash();

        const std::string getPathPlatformSlash() const;

        void toPathDefaultSlash();
    public:
        File(const std::string& path);

        File(const std::string& absDirPath, const std::string fileName);

        File(const File& dir, const std::string fileName);

        File(const File& file);

        const std::string getAbsolutePath() const;

        /// @brief Получить путь к файлу, назначенный для этого объекта File
        /// @return путь к файлу
        const std::string& getPath() const;

        std::string getParent() const;

        std::string getName() const;

        std::string getExtension() const;

        bool exists() const;

        bool isFile() const;

        /// @brief Проверяет, является ли файл по пути, указаном в объекте класса, директорией
        /// @return 
        bool isDirectory() const;

        bool isReadonly() const;

        uint64_t getFileSize() const;

        bool mkdir() const;

        bool remove(bool recursively = false) const;

        void rename(const std::string& newName);

        void rename(const File& dest);

        std::vector<File> listFiles() const;

        const DateTime getLastEditTime() const;

        void createNewFile() const;

        std::ifstream getIfstream(std::ios_base::openmode openmode = std::ios_base::in) const;

        std::ofstream getOfstream(std::ios_base::openmode openmode = std::ios_base::out) const;

        int readFile(char** data) const;
    };


}