#pragma once

#include <string>
#include <Core/Base.hpp>
#include <tuple>
#include <Core/Runnable.hpp>

namespace Mpi {

#define THREAD_ROUTINE_FP(x) int (*x)(void*)

#ifdef __linux__
	typedef void* THREAD_ROUTINE_RET_TYPE;
#endif

#ifdef _WIN32
	typedef unsigned long THREAD_ROUTINE_RET_TYPE;
#endif

	class Thread {
	private:
		void* mHandle;
		uint32_t mThreadId;

		THREAD_ROUTINE_FP(mFunction);
		void* mArg;
		Runnable* runnableRoutine;

		bool run();

		int executeThreadRoutine();
	public:
		Thread(THREAD_ROUTINE_FP(func), void* arg);

		Thread(const Thread& other) = delete;

		Thread(Thread&& other) noexcept;

		Thread(Runnable* routine);

		~Thread();

		int join();
		void setPriority(int priority);
		int setName(const std::string& name);
		std::string getName() const;

		uint32_t getThreadId();

		static void sleep(uint32_t ms);
		static uint32_t getCurrentThreadId();

		static THREAD_ROUTINE_RET_TYPE threaded_func(void* data);
	};
}