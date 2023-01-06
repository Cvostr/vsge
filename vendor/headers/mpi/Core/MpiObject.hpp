#pragma once

#include <string>

namespace Mpi
{
    class MpiObject {
    public:
        virtual std::string toString(){
            return "MpiObject " + std::to_string((int)this);
        }
    };
} // namespace Mpi
