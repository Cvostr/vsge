#include "AsyncLoader.hpp"
#include <fstream>
#include <Core/Logger.hpp>
#include "ResourceLayer.hpp"
#include <Engine/Application.hpp>

using namespace VSGE;

AsyncLoader::AsyncLoader() :
    mMutex(new Mutex)
{
}

AsyncLoader::~AsyncLoader() {
    SAFE_RELEASE(mMutex);
}

void AsyncLoader::LoadResource(Resource* resource) {
    resource->SetState(RESOURCE_STATE_LOADING);
    //Declare blob loader stream
    std::ifstream stream;
    //Get absolute path to blob file
    DataDescription desc = resource->GetDataDescription();
    std::string file_path = desc.file_path;
    //Check, if size explicitly specified
    if (desc.size > 0) {
        //if size explicitly specified
        stream.open(file_path, std::ofstream::binary);
        stream.seekg(static_cast<long>(desc.offset));
    }
    else {
        //if size not explicitly specified, then try to load file
        //and get file size
        stream.open(file_path, std::ofstream::binary | std::ofstream::ate);
        desc.size = static_cast<uint32>(stream.tellg());
        resource->SetDataDescription(desc);
        stream.seekg(0);
    }

    if (!stream.is_open()) {
        resource->SetState(RESOURCE_STATE_LOADING_FAILED);
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error loading resource " << resource->GetName() << "\n";
        return;
    }

    byte* data = new byte[desc.size];
    stream.read(reinterpret_cast<char*>(data), desc.size);
    stream.close();

    resource->SetLoadedData(data);
    //Set resource state to loaded
    resource->SetState(RESOURCE_STATE_LOADED);
    //if mutex was destroyed outside
    if(!mMutex){
        return;
    }

    //Do some specific work (on MeshGroupResource as example)
    //Send Resource load event
    if (mMutex != nullptr)
        mMutex->Lock();
    resource->Prepare();
    ResourceLoadEvent* rle = new ResourceLoadEvent(resource);
    Application::Get()->QueueEvent(rle);
    if (mMutex != nullptr)
        mMutex->Release();
}

void AsyncLoader::AddToQueue(Resource* resource) {
    if (resource->GetState() != RESOURCE_STATE_UNLOADED)
        return;
    //Lock array mutex
    mMutex->Lock();
    
    resource->SetState(RESOURCE_STATE_QUEUED);
    loadQueue.push(resource);
    //release array mutex
    mMutex->Release();
}

uint32 AsyncLoader::GetQueuedResourcesCount() {
    return loadQueue.size(); 
}

void AsyncLoader::WaitForLoading(Resource* resource) {
    while (resource->GetState() == RESOURCE_STATE_QUEUED || resource->GetState() == RESOURCE_STATE_LOADING);
}

void AsyncLoader::THRFunc() {
    while (_running) {

        if (loadQueue.size() > 0) {
            mMutex->Lock();
            //obtain next resource pointer
            Resource* resource = loadQueue.front();
            loadQueue.pop();
            //unlock thread
            mMutex->Release();
            //Load resource by request
            LoadResource(resource);
        }
        SleepThread(5);
    }
}