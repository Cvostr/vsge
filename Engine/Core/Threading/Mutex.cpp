#include "Mutex.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h> //Include POSIX thread implementation
#endif

using namespace VSGE;

Mutex::Mutex() {
#ifdef _WIN32
	mMutexHandle = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)this->mMutexHandle);
#else
	mMutexHandle = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*)mMutexHandle, NULL);
#endif
}
Mutex::~Mutex() {
#ifdef _WIN32
	DeleteCriticalSection((CRITICAL_SECTION*)this->mMutexHandle);
#else
	pthread_mutex_destroy((pthread_mutex_t*)mMutexHandle);
#endif
	delete mMutexHandle;
}

void Mutex::Lock() {
#ifdef _WIN32
	EnterCriticalSection((CRITICAL_SECTION*)mMutexHandle);
#else
	pthread_mutex_lock((pthread_mutex_t*)mMutexHandle);
#endif
}
void Mutex::Release() {
#ifdef _WIN32
	LeaveCriticalSection((CRITICAL_SECTION*)mMutexHandle);
#else
	pthread_mutex_unlock((pthread_mutex_t*)mMutexHandle);
#endif
}
