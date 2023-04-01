#include "AsyncLoader.hpp"
#include <fstream>
#include <Core/Logger.hpp>
#include "ResourceLayer.hpp"
#include <Engine/Application.hpp>
#include <mpi/Filesystem/File.hpp>

using namespace VSGE;

AsyncLoader::AsyncLoader() :
    mMutex(new Mpi::Mutex)
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
    if (desc.size == 0) {
        //Если размер не указан (равен 0)
        Mpi::File file(file_path);
        desc.size = static_cast<uint32>(file.getFileSize());
        resource->SetDataDescription(desc);
    }

    stream.open(file_path, std::ofstream::binary);
    stream.seekg(static_cast<long>(desc.offset));

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

    //Вызов обработки ресурса из этого потока
    resource->Prepare();
    //Отправка события в главный поток
    ResourceLoadEvent* rle = new ResourceLoadEvent(resource);
    Application::Get()->QueueEvent(rle);
}

void AsyncLoader::AddToQueue(Resource* resource) {
    if (resource->GetState() != RESOURCE_STATE_UNLOADED)
        return;
    //Lock array mutex
    mMutex->lock();
    
    resource->SetState(RESOURCE_STATE_QUEUED);
    loadQueue.push(resource);
    //release array mutex
    mMutex->unlock();
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
            mMutex->lock();
            //obtain next resource pointer
            Resource* resource = loadQueue.front();
            loadQueue.pop();
            //unlock thread
            mMutex->unlock();
            //Load resource by request
            LoadResource(resource);
        }
        SleepThread(5);
    }
}