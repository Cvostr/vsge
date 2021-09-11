#include "AsyncLoader.hpp"
#include <fstream>
#include <Core/Logger.hpp>
#include "ResourceLayer.hpp"
#include <Engine/Application.hpp>

using namespace VSGE;

AsyncLoader::AsyncLoader() :
    mMutex(new Mutex),
    queue_length(0)
{
    loadQueue = new LoadRequest[LOADER_QUEUE_SIZE];
}

AsyncLoader::~AsyncLoader() {
    delete mMutex;
    delete[] loadQueue;
}

void AsyncLoader::LoadResource(Resource* resource, bool justRead) {
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

    if (!justRead) {
        //Do some specific work (on MeshGroupResource as example)
        resource->Prepare();
        //Send Resource load event
        mMutex->Lock();
        ResourceLoadEvent* rle = new ResourceLoadEvent(resource);
        Application::Get()->QueueEvent(rle);
        mMutex->Release();
   }
}

void AsyncLoader::AddToQueue(Resource* resource, bool justRead) {
    if (resource->GetState() != RESOURCE_STATE_UNLOADED)
        return;
    //Lock array mutex
    mMutex->Lock();
    
    resource->SetState(RESOURCE_STATE_QUEUED);
    LoadRequest request(resource, justRead);
    loadQueue[LOADER_QUEUE_SIZE - 1 - (queue_length)] = request;
    queue_length++;
    //release array mutex
    mMutex->Release();
}

void AsyncLoader::WaitForLoading(Resource* resource) {
    while (resource->GetState() == RESOURCE_STATE_QUEUED || resource->GetState() == RESOURCE_STATE_LOADING);
}

void AsyncLoader::THRFunc() {
    while (mShouldRun) {

        if (queue_length > 0) {
            mMutex->Lock();
            //Obtain pointer to LoadRequest
            LoadRequest* request = &loadQueue[LOADER_QUEUE_SIZE - queue_length];
            //Reduce requests pool amount
            queue_length--;
            //unlock thread
            mMutex->Release();
            //Load resource by request
            LoadResource(request->_resource, request->_justRead);
        }
        SleepThread(5);
    }
}