#pragma once

#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>
#include <vector>
#include "Resource.hpp"

namespace VSGE{
    class ResourcesWatchdog : public Thread{
    private:
        Mutex* _mutex;
        std::vector<Resource*>* _resources;
    public:
        ResourcesWatchdog();

		~ResourcesWatchdog();

        void SetResourcesToWatch(std::vector<Resource*>& resources);

        void THRFunc();
    };
}