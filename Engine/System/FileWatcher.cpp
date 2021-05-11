#include "FileWatcher.hpp"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/inotify.h>
#include "unistd.h"
#endif

using namespace VSGE;

#define BUFFERSIZE 8192

void FileWatcher::WatchDirectory(const std::string& directory) {
    mWatchingDirectory = directory;
#ifdef _WIN32

    LPWSTR wpath = new WCHAR[directory.size() + 1];
    mbstowcs((wchar_t*)wpath, directory.c_str(), directory.size());
    wpath[directory.size()] = L'\0';

    handle = (void*)CreateFileW(
        (LPCWSTR)wpath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr);

    delete[] wpath;
#endif

    Run();
}

void FileWatcher::THRFunc() {
    while (mShouldRun) {
        WatchWin32();
    }
}

void FileWatcher::WatchWin32() {
#ifdef _WIN32
    unsigned char buffer[BUFFERSIZE];
    DWORD bytesFilled = 0;
    if (ReadDirectoryChangesW((HANDLE)handle,
        buffer,
        BUFFERSIZE,
        true,
        FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_CREATION,
        &bytesFilled,
        nullptr,
        nullptr))
    {
        unsigned offset = 0;

        while (offset < bytesFilled)
        {
            FILE_NOTIFY_INFORMATION* record = (FILE_NOTIFY_INFORMATION*)&buffer[offset];

            if (record->Action == FILE_ACTION_MODIFIED || record->Action == FILE_ACTION_ADDED || record->Action == FILE_ACTION_REMOVED)
            {
                char* mb_filepath = new char[record->FileNameLength / sizeof(WCHAR) + 1];
                wcstombs(mb_filepath, record->FileName, record->FileNameLength / sizeof(WCHAR));
                mb_filepath[record->FileNameLength / sizeof(WCHAR)] = '\0';

                FileChange fc ( std::string(mb_filepath),
                                (FileChangeActionType)record->Action );
                
                fc.absoluteFilePath = this->mWatchingDirectory + "\\" + std::string(mb_filepath);

                changes.push_back(fc);
 
                delete[] mb_filepath;
            }

            if (!record->NextEntryOffset)
                break;
            else
                offset += record->NextEntryOffset;
        }
    }
#endif
}