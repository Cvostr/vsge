#pragma once

namespace Mpi {
    class Runnable {
    public:

        virtual ~Runnable() {

        }

        virtual int run() = 0;
    };
}