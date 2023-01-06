#pragma once

namespace Mpi {
    class Runnable {
    public:

        virtual ~Runnable() {

        }

        virtual void run() = 0;
    };
}