#pragma once

#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>
#include <Core/VarTypes/Base.hpp>
#include <string>
#include <vector>

namespace VSGE {

	enum FileChangeActionType {
		FCAT_ADDED = 0x00000001,
		FCAT_DELETED = 0x00000002,
		FCAT_MODIFIED = 0x00000003
	};

	struct FileChange {
		std::string filePath;
		std::string absoluteFilePath;
		FileChangeActionType actionType;

		FileChange(const std::string& filePath,
			const FileChangeActionType& actionType) :

			filePath(filePath),
			actionType(actionType)
		{}
	};

	class FileWatcher : public Thread {
	private:
		void* handle;
		Mutex* mMutex;
		uint32 mDelay;
		std::vector<FileChange> changes;
		std::string mWatchingDirectory;
	public:
		
		FileWatcher() :
			mMutex(new Mutex),
			mDelay(700)
		{

		}

		~FileWatcher() {
			delete mMutex;
		}

		void WatchDirectory(const std::string& directory);

		void WatchWin32();

		void THRFunc();
	};
}