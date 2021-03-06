#pragma once

#include <string>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	class Thread {
	private:
		void* _handle;
	protected:
		uint32 _thread_id;
		bool _running;
		uint32 _sleep_time;
	public:
		Thread() : 
			_handle(nullptr),
			_running(false),
			_sleep_time(5),
			_thread_id(0)
		{}

		virtual ~Thread() {
			Stop();
		}

		bool Run();
		void Stop();
		bool IsRunning();
		void SetThreadPriority_(int priority);
		void SetThreadName(const std::string& name);
		/// <summary>
		/// Do not use manually
		/// </summary>
		void SetThreadID();
		uint32 GetThreadID();
		virtual void THRFunc();
		virtual void ThreadedFunction() {}

		static void SleepThread(uint32 ms);
		static uint32 GetCurrentThreadID();
	};
}