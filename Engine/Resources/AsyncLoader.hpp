#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>
#include "Resource.hpp"
#include <string>
#include <vector>

#define LOADER_QUEUE_SIZE 500

namespace VSGE {

	class AsyncLoader : public Thread {
	private:
		Mutex* mMutex;
		Resource** loadQueue;
		uint32 queue_length;
	public:
		AsyncLoader() :
			mMutex(new Mutex),
			queue_length(0)
		{
			loadQueue = new Resource*[LOADER_QUEUE_SIZE];
		}

		~AsyncLoader() {
			delete mMutex;
			delete[] loadQueue;
		}

		uint32 GetQueuedResourcesCount() { return queue_length; }

		void AddToQueue(Resource* resource);

		void WaitForLoading(Resource* resource);

		void THRFunc();
	};
}