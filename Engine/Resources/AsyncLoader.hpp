#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/Threading/Thread.hpp>
#include <mutex>
#include <condition_variable>
#include "Resource.hpp"
#include <string>
#include <queue>

namespace VSGE {

	class AsyncLoader : public Thread {
	private:
		std::mutex q_mtx;
		std::condition_variable queued_event;
		std::queue<Resource*> loadQueue;

		void THRFunc();
		void LoadResource(Resource* resource);
	public:
		AsyncLoader();

		~AsyncLoader();

		size_t GetQueuedResourcesCount();

		void AddToQueue(Resource* resource);
		void WaitForLoading(Resource* resource);
	};
}