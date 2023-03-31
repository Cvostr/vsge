#pragma once

#include "Base.hpp"
#include <string>

namespace Mpi {

    class Random {
    public:
        static int random();

        static int random(int min, int max);

        static double randomNormalized();

        static std::string randomString(uint32_t len);
    };
}