#pragma once

#include <string>
#include <Filesystem/File.hpp>

namespace Mpi {

    enum FilesystemAction {
        FILESYSTEM_ACTION_CREATE,
        FILESYSTEM_ACTION_DELETE,
        FILESYSTEM_ACTION_MODIFY,
        FILESYSTEM_ACTION_RENAMED
    };

    class FilesystemWatcherEvent {
    private:
        FilesystemAction action;
        File oldFile;
        File file;
    public:

        FilesystemWatcherEvent(FilesystemAction action, const File& file);

        FilesystemWatcherEvent(FilesystemAction action, const File& oldFile, File& file);

        const FilesystemAction getAction() const;

        const File& getFile() const;

        const File& getOldFile() const;
    };

}