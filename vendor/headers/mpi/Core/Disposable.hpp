#pragma once

namespace Mpi {
    class Disposable {
    public:

        Disposable() {}

        virtual ~Disposable() {
            
        }

        virtual void close() = 0;
    };
}