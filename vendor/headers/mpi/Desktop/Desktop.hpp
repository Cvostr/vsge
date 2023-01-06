#pragma once

#include <Filesystem/File.hpp>

namespace Mpi {
    class Desktop {
    public:
        static void openFile(const File& file);
    };
}