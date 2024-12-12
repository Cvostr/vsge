#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/Threading/Thread.hpp>
#include <mpi/Os/Mutex.hpp>
#include "Resource.hpp"
#include <string>
#include <queue>

namespace VSGE {

	class AsyncLoader : public Thread {
	private:
		Mpi::Mutex * mMutex;
		std::queue<Resource*> loadQueue;

		void THRFunc();
		void LoadResource(Resource* resource);
	public:
		AsyncLoader();

		~AsyncLoader();

		uint32 GetQueuedResourcesCount();

		void AddToQueue(Resource* resource);

		void WaitForLoading(Resource* resource);
	};
}