#pragma once

#include "FilesystemWatcherEvent.hpp"
#include <functional>

namespace Mpi {

    class PlatformWatcherRunnable;

#ifdef __linux__
	typedef int FILE_WATCHER_HANDLE;
#endif

#ifdef _WIN32
	typedef void* FILE_WATCHER_HANDLE;
#endif

    class FilesystemWatcher {
    private:
        FILE_WATCHER_HANDLE mHandle;

        File mDirectory;

        bool mEnabled;

        bool mWatchSubdirs;

        FILE_WATCHER_HANDLE getHandle() const;

        std::function<void (const FilesystemWatcherEvent&)> eventHandlerFunction;

        static void registerWatcher(FilesystemWatcher* watcher);

        static void unregisterWatcher(FilesystemWatcher* watcher);

        void onEvent(const FilesystemWatcherEvent& event);

        friend class PlatformWatcherRunnable;

    public:
        FilesystemWatcher(const File& directory, bool watchSubdirs = false);

        ~FilesystemWatcher();

        void setEnabled(bool enabled);

        bool isEnabled() const;

        bool isWatchingSubdirs() const;

        const std::string getDirectoryAbsolutePath() const;

        const File& getDirectory() const;

        template <typename F>
        void setEventHandler(F func) {
            eventHandlerFunction = func;
        }
    };
}