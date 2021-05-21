#include "AsyncLoader.hpp"
#include <fstream>
#include <Core/Logger.hpp>

using namespace VSGE;

void _LDR_load(Resource* resource) {
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
        //if size explicitly specified, then try to load file
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
    //Do some specific work (on MeshGroupResource as example)
    resource->Prepare();
    //Set resource state to loaded
    resource->SetState(RESOURCE_STATE_LOADED);
}

void AsyncLoader::AddToQueue(Resource* resource) {
    mMutex->Lock();

    resource->SetState(RESOURCE_STATE_QUEUED);
    loadQueue[LOADER_QUEUE_SIZE - 1 - (queue_length)] = resource;
    queue_length++;

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
            Resource* res = loadQueue[LOADER_QUEUE_SIZE - queue_length];
            //Reduce requests pool amount
            queue_length--;
            //unlock thread
            mMutex->Release();
            //Load resource by request
            _LDR_load(res);
        }
    }
}