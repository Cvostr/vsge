#pragma once

#include <Core/Base.hpp>

namespace Mpi {
	class Mutex {
	protected:
		void* mHandle;
		
		void init();
        void destroy();
	public:

		Mutex();

		~Mutex();

		void lock();

        void tryLock();

		void unlock();
	};
}