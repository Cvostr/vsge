#include "ResourcesWatchdog.hpp"
#include <Core/Time.hpp>
#include <Core/Logger.hpp>

using namespace VSGE;

#define MAX_RESOURCE_LIFETIME 5000

ResourcesWatchdog::ResourcesWatchdog() :
    _mutex(new Mutex)
{

}

ResourcesWatchdog::~ResourcesWatchdog(){
    delete _mutex;
}

void ResourcesWatchdog::SetResourcesToWatch(std::vector<Resource*>& resources){
    _resources = &resources;
}

void ResourcesWatchdog::THRFunc(){
    while (mShouldRun) {
        for(uint32 resource_i = 0; resource_i < _resources->size(); resource_i ++){
            _mutex->Lock();
            Resource* resource = _resources->at(resource_i);
            if(resource->IsReady() && resource->GetLoadedData()){
                resource->FreeLoadedData();
            }
            if(resource->IsReady() && resource->GetLastUseTime() == 0){
                resource->Use();
            }

            if(resource->IsReady() && resource->GetName().rfind("Default") != 0){
                uint64 time_delta = TimePerf::Get()->GetCurrentTime() - resource->GetLastUseTime();
                if(time_delta > MAX_RESOURCE_LIFETIME && resource->GetResourceType() == RESOURCE_TYPE_TEXTURE){
                    resource->Release();
                }
            }
            _mutex->Release();
        }
    }
}