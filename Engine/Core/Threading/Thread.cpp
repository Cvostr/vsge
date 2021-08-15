#define _CRT_SECURE_NO_WARNINGS
#include "Thread.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include "unistd.h"
#endif

using namespace VSGE;

#ifdef _WIN32
static DWORD WINAPI CallThreadFuncton(VOID* data)
{
	Thread* thread = static_cast<Thread*>(data);
	thread->THRFunc();
	return 0;
}
#elif __linux__
static void* CallThreadFuncton(void* data)
{
	Thread* thread = static_cast<Thread*>(data);
	thread->THRFunc();
	pthread_exit((void*)nullptr);
	return nullptr;
}
#endif

bool Thread::Run() {
	mShouldRun = true;
	if (mThreadHandle != nullptr)
		return false;
#ifdef _WIN32
	mThreadHandle = CreateThread(nullptr, 0, CallThreadFuncton, this, 0, nullptr);
#elif __linux__
	mThreadHandle = new pthread_t;
	pthread_attr_t type;
	pthread_attr_init(&type);
	pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
	pthread_create((pthread_t*)mThreadHandle, &type, CallThreadFuncton, this);
#endif
	
	return true;
}
void Thread::Stop() {
	mShouldRun = false;
#ifdef _WIN32
	WaitForSingleObject((HANDLE)mThreadHandle, INFINITE);
	CloseHandle((HANDLE)mThreadHandle);
#else
	auto* thread = (pthread_t*)mThreadHandle;
	if (thread)
		pthread_join(*thread, nullptr);
#endif
	mThreadHandle = nullptr;
}
bool Thread::IsRunning() {
	return mThreadHandle != nullptr;
}

void Thread::SleepThread(uint32 ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

void Thread::SetThreadPriority_(int priority) {
	if (!IsRunning())
		return;
#ifdef _WIN32
	SetThreadPriority((HANDLE)mThreadHandle, priority);
#elif __linux__
	pthread_t* thread = (pthread_t*)mThreadHandle;
	if (thread)
		pthread_setschedprio(*thread, priority);
#endif
}

void Thread::SetThreadName(const std::string& name) {
#ifdef _WIN32
	WCHAR* thr_name = new WCHAR[name.size() + 1];
	thr_name[name.size()] = L'\0';
	mbstowcs(thr_name, name.c_str(), name.size());
	SetThreadDescription((HANDLE)mThreadHandle, thr_name);
	delete[] thr_name;
#endif
#ifdef __linux__
	pthread_t* thread = (pthread_t*)mThreadHandle;
	pthread_setname_np(*thread, name.c_str());
#endif
}