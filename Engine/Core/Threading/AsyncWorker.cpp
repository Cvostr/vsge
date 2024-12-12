#include "AsyncWorker.hpp"

AsyncWorker::AsyncWorker() :
	thr(&AsyncWorker::ThreadRoutine, this)
{

}

void AsyncWorker::ThreadRoutine()
{
	while (true) 
	{
		queueMutex.lock();

		if (queuedTasks.size() > 0) {

			auto task = std::move(queuedTasks.front());
			queuedTasks.pop();
			task.get();
		}

		queueMutex.unlock();
	}
}