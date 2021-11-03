#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class Thread {
	private:
		void* _handle;
	protected:
		bool _running;
		uint32 _sleep_time;
	public:
		Thread() : 
			_handle(nullptr),
			_running(false),
			_sleep_time(5)
		{}

		virtual ~Thread() {
			Stop();
		}

		bool Run();
		void Stop();
		void SleepThread(uint32 ms);
		bool IsRunning();
		void SetThreadPriority_(int priority);
		void SetThreadName(const std::string& name);
		virtual void THRFunc();
		virtual void ThreadedFunction() {}
	};
}