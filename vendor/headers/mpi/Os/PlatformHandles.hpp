#pragma once

typedef void* WIN32HANDLE;

#ifdef __linux__
	typedef int FD_HANDLE;

    typedef int PROCESS_HANDLE;

    typedef int FILE_WATCHER_HANDLE;
#endif

#ifdef _WIN32
	typedef WIN32HANDLE FD_HANDLE;

    typedef WIN32HANDLE PROCESS_HANDLE;

    typedef WIN32HANDLE FILE_WATCHER_HANDLE;
#endif