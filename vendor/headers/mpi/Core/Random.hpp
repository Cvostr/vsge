#pragma once

namespace Mpi {

    class Random {
    public:
        static int Rand();

        static int Rand(int min, int max);
    };
}