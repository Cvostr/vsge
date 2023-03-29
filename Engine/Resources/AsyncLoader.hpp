#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/Threading/Thread.hpp>
#include <mpi/Os/Mutex.hpp>
#include "Resource.hpp"
#include <string>
#include <queue>

#define LOADER_QUEUE_SIZE 3000

namespace VSGE {

	class AsyncLoader : public Thread {
	private:
		Mpi::Mutex * mMutex;
		std::queue<Resource*> loadQueue;
	public:
		AsyncLoader();

		~AsyncLoader();

		uint32 GetQueuedResourcesCount();

		void AddToQueue(Resource* resource);

		void WaitForLoading(Resource* resource);

		void THRFunc();

		void LoadResource(Resource* resource);
	};
}