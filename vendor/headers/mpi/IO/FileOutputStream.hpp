#pragma once

#include <IO/OutputStream.hpp>
#include <Os/PlatformHandles.hpp>
#include <Filesystem/File.hpp>

namespace Mpi {
	class FileOutputStream : public OutputStream {
    private:
        FD_HANDLE mFd;
    public:
        FileOutputStream(FD_HANDLE handle);

        FileOutputStream(const FileOutputStream& other) = delete;

        FileOutputStream();

        ~FileOutputStream();

        FD_HANDLE getHandle();

        int open(const File& file);

        int write(const char* buffer, int len);

        void close();
	};
}