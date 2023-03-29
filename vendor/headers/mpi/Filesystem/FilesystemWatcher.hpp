#pragma once

#include "FilesystemWatcherEvent.hpp"
#include <Os/PlatformHandles.hpp>
#include <functional>

namespace Mpi {

    class PlatformWatcherRunnable;

    class FilesystemWatcher {
    private:
        FILE_WATCHER_HANDLE mHandle;

        File mDirectory;

        bool mEnabled;

        bool mWatchSubdirs;

        bool mDecomposeRename;

        FILE_WATCHER_HANDLE getHandle() const;

        std::function<void (const FilesystemWatcherEvent&)> eventHandlerFunction;

        static void registerWatcher(FilesystemWatcher* watcher);

        static void unregisterWatcher(FilesystemWatcher* watcher);

        void onEvent(const FilesystemWatcherEvent& event);

        friend class PlatformWatcherRunnable;

    public:
        FilesystemWatcher(const File& directory, bool watchSubdirs = false);

        FilesystemWatcher(const FilesystemWatcher& other) = delete;

        ~FilesystemWatcher();

        void setEnabled(bool enabled);

        bool isEnabled() const;

        void setDecomposeRename(bool enabled);

        bool isDecomposeRenameEnabled() const;

        bool isWatchingSubdirs() const;

        const std::string getDirectoryAbsolutePath() const;

        const File& getDirectory() const;

        template <typename F>
        void setEventHandler(F func) {
            eventHandlerFunction = func;
        }
    };
}