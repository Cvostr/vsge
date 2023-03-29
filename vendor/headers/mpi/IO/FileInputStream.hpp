#pragma once

#include <IO/InputStream.hpp>
#include <Filesystem/File.hpp>
#include <Os/PlatformHandles.hpp>

namespace Mpi {

    class FileInputStream : public InputStream {
    private:
        FD_HANDLE mFd;
    public:

        FileInputStream(FD_HANDLE handle);

        FileInputStream(const FileInputStream& other) = delete;

        FileInputStream();

        ~FileInputStream();

        FD_HANDLE getFD();

        int open(const File& file);

        int read();

        int32_t read(char* buffer, int32_t len);

        int64_t seek(StreamSeekDir direction, int64_t offset);

        void close();
    };
}