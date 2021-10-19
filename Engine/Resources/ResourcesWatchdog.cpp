#include "ResourcesWatchdog.hpp"
#include <Core/Time.hpp>
#include <Core/Logger.hpp>
#include <algorithm>

using namespace VSGE;

ResourcesWatchdog::ResourcesWatchdog() :
    _mutex(new Mutex),
    _resource_lifetime(6000)
{

}

ResourcesWatchdog::~ResourcesWatchdog(){
    delete _mutex;
}

void ResourcesWatchdog::SetResourceLifetime(uint32 lifetime){
    _resource_lifetime = lifetime;
}

void ResourcesWatchdog::AddResource(Resource* resource) {
    _mutex->Lock();
    _resources.push_back(resource);
    _mutex->Release();
}

void ResourcesWatchdog::RemoveResource(Resource* resource) {
    _mutex->Lock();
    std::remove(_resources.begin(), _resources.end(), resource);
    _mutex->Release();
}

void ResourcesWatchdog::THRFunc(){
    while (_running) {
        for(uint32 resource_i = 0; resource_i < _resources.size(); resource_i ++){
            _mutex->Lock();
            Resource* resource = _resources.at(resource_i);
            if(resource->IsReady() && resource->GetLoadedData()){
                resource->FreeLoadedData();
            }
            if(resource->IsReady() && resource->GetLastUseTime() == 0){
                resource->Use();
            }

            if(resource->IsReady() && !resource->IsDefault()){
                uint64 time_delta = TimePerf::Get()->GetCurrentTime() - resource->GetLastUseTime();
                if(time_delta > _resource_lifetime && (
                    resource->GetResourceType() == RESOURCE_TYPE_MESH
                    || resource->GetResourceType() == RESOURCE_TYPE_TEXTURE
                    || resource->GetResourceType() == RESOURCE_TYPE_AUDIOCLIP
                    || resource->GetResourceType() == RESOURCE_TYPE_ANIMATION
                    || resource->GetResourceType() == RESOURCE_TYPE_MATERIAL
                    )){
                    resource->Release();
                }
            }
            _mutex->Release();
        }
        SleepThread(100);
    }
}