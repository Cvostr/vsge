#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>
#include "Resource.hpp"
#include <string>
#include <vector>

#define LOADER_QUEUE_SIZE 500

namespace VSGE {

	struct LoadRequest {
		Resource* _resource;
		bool _justRead;

		LoadRequest() 
		{

		}

		LoadRequest(Resource* resource, bool justRead = false) :
			_resource(resource),
			_justRead(justRead)
		{

		}
	};

	class AsyncLoader : public Thread {
	private:
		Mutex* mMutex;
		LoadRequest* loadQueue;
		uint32 queue_length;
	public:
		AsyncLoader();

		~AsyncLoader();

		uint32 GetQueuedResourcesCount() { return queue_length; }

		void AddToQueue(Resource* resource, bool justRead = false);

		void WaitForLoading(Resource* resource);

		void THRFunc();

		void LoadResource(Resource* resource, bool justRead);
	};
}