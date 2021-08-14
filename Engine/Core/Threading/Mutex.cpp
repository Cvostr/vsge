#include "Mutex.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h> //Include POSIX thread implementation
#endif

using namespace VSGE;

Mutex::Mutex() {
#ifdef _WIN32
	_mutexHandle = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)this->_mutexHandle);
#else
	_mutexHandle = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*)_mutexHandle, NULL);
#endif
}
Mutex::~Mutex() {
#ifdef _WIN32
	DeleteCriticalSection((CRITICAL_SECTION*)this->_mutexHandle);
#else
	pthread_mutex_destroy((pthread_mutex_t*)_mutexHandle);
#endif
	delete _mutexHandle;
}

void Mutex::Lock() {
#ifdef _WIN32
	EnterCriticalSection((CRITICAL_SECTION*)_mutexHandle);
#else
	pthread_mutex_lock((pthread_mutex_t*)_mutexHandle);
#endif
}
void Mutex::Release() {
#ifdef _WIN32
	LeaveCriticalSection((CRITICAL_SECTION*)_mutexHandle);
#else
	pthread_mutex_unlock((pthread_mutex_t*)_mutexHandle);
#endif
}
