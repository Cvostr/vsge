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
		Mutex(const Mutex& other) = delete;
		Mutex(Mutex&& other) noexcept;

		~Mutex();

		/// @brief Заблокировать данную критическую секцию.
		void lock() const;

        bool tryLock();

		/// @brief Разблокировать критическую секцию.
		void unlock() const;
	};
}