#include "AsyncLoader.hpp"
#include <fstream>
#include <Core/Logger.hpp>
#include "ResourceLayer.hpp"
#include <Engine/Application.hpp>
#include <mpi/Filesystem/File.hpp>

using namespace VSGE;

AsyncLoader::AsyncLoader() {

}

AsyncLoader::~AsyncLoader() {
    _running = false;
    queued_event.notify_one();
    Stop();
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

void AsyncLoader::AddToQueue(Resource* resource) 
{
    if (resource->GetState() != RESOURCE_STATE_UNLOADED)
        return;
    
    {
        std::lock_guard<std::mutex> q_guard(q_mtx);
        resource->SetState(RESOURCE_STATE_QUEUED);
        loadQueue.push(resource);
    }

    queued_event.notify_one();
}

size_t AsyncLoader::GetQueuedResourcesCount() {
    return loadQueue.size(); 
}

void AsyncLoader::WaitForLoading(Resource* resource) {
    while (resource->GetState() == RESOURCE_STATE_QUEUED || resource->GetState() == RESOURCE_STATE_LOADING);
}

void AsyncLoader::THRFunc() 
{
    while (_running) 
    {
        std::unique_lock<std::mutex> lock(q_mtx);

        // wait for tasks
        queued_event.wait(lock, [this] { return !loadQueue.empty() || _running == false; });

        if (_running == false) {
            lock.unlock();
            return;
        }

        // obtain next resource pointer
        Resource* resource = loadQueue.front();
        loadQueue.pop();
        // unlock thread
        lock.unlock();

        // Load resource by request  
        LoadResource(resource);
    }
}