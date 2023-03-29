#pragma once

#include <Core/Threading/Thread.hpp>
#include <mpi/Os/Mutex.hpp>
#include <vector>
#include "Resource.hpp"

namespace VSGE{
    class ResourcesWatchdog : public Thread{
    private:
        Mpi::Mutex * _mutex;
        std::vector<Resource*> _resources;
        uint32 _resource_lifetime;
    public:
        ResourcesWatchdog();

		~ResourcesWatchdog();

        void AddResource(Resource* resource);

        void SetResourceLifetime(uint32 lifetime);

        void RemoveResource(Resource* resource);

        void THRFunc();
    };
}