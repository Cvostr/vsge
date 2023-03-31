#pragma once

#include "Base.hpp"

namespace Mpi {
    class Disposable {
    public:

        Disposable() {}

        virtual ~Disposable() {
            
        }

        virtual void close() = 0;
    };
}