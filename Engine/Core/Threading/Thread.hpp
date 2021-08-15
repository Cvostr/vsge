#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class Thread {
	protected:
		bool mShouldRun;
	public:
		Thread() : mThreadHandle(nullptr),
			mShouldRun(false){
		}

		~Thread() {
			Stop();
		}

		bool Run();
		void Stop();
		void SleepThread(uint32 ms);
		bool IsRunning();
		void SetThreadPriority_(int priority);
		void SetThreadName(const std::string& name);
		virtual void THRFunc() = 0;
	private:
		void* mThreadHandle;
	};
}