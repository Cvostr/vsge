#pragma once

#include <string>
#include <Core/Base.hpp>
#include <tuple>
#include <Core/Runnable.hpp>

namespace Mpi {

#define THREAD_ROUTINE_FP(x) void (*x)(void*)

#ifdef __linux__
	typedef void* THREAD_ROUTINE_RET_TYPE;
#endif

#ifdef _WIN32
	typedef unsigned long THREAD_ROUTINE_RET_TYPE;
#endif

	class Thread {
	private:
		void* mHandle;
		uint32 mThreadId;

		THREAD_ROUTINE_FP(mFunction);
		void* mArg;
		Runnable* runnableRoutine;

		bool run();

		void executeThreadRoutine();
	public:
		Thread(THREAD_ROUTINE_FP(func), void* arg);

		Thread(Runnable* routine);

		~Thread();

		void join();
		void setPriority(int priority);
		void setName(const std::string& name);
		std::string getName() const;

		uint32 getThreadId();

		static void sleep(uint32 ms);
		static uint32 getCurrentThreadId();

		static THREAD_ROUTINE_RET_TYPE threaded_func(void* data);
	};
}