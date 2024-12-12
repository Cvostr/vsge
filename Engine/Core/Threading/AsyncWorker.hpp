#pragma once

#include <queue>
#include <future>
#include <mutex>

class AsyncWorker {
	
private:
	std::queue<std::future<void>> queuedTasks;
	std::mutex queueMutex;

	std::thread thr;

	void ThreadRoutine();

public:

	AsyncWorker();

	template<typename F, typename ...Args>
	void QueueTask(const F& func, Args... args)
	{
		queueMutex.lock();

		queuedTasks.emplace(std::async(std::launch::deferred, func, args...));	

		queueMutex.unlock();
	}
};