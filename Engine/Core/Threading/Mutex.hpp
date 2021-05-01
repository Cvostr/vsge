#pragma once


namespace VSGE {
	class Mutex {
	public:
		Mutex();
		~Mutex();

		void Lock();
		void Release();

	private:
		void* mMutexHandle;
	};
}