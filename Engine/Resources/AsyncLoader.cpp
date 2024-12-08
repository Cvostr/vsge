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

    try {
        resource->ReadData();
    }
    catch (std::exception& ex) {
        Logger::Log(LogType::LOG_TYPE_ERROR) << ex.what() << "\n";
        return;
    }

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