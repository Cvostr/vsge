#pragma once

#include <Core/Threading/Thread.hpp>
#include <Core/Threading/Mutex.hpp>
#include <Core/VarTypes/Base.hpp>
#include <Engine/Event.hpp>
#include <string>
#include <vector>

namespace VSGE {

	enum FileChangeActionType {
		FCAT_ADDED = 0x00000001,
		FCAT_DELETED = 0x00000002,
		FCAT_MODIFIED = 0x00000003
	};

	class FileChageEvent : public IEvent {
		std::string _filePath;
		std::string _absFilePath;
		FileChangeActionType _actionType;
	public:
		EVENT_CLASS_TYPE(EventType::EventFileChanged)

		const std::string& GetFilePath() { return _filePath; }
		const std::string& GetAbsFilePath() { return _absFilePath; }
		FileChangeActionType GetActionType() { return _actionType; }

			FileChageEvent(
				const std::string& filePath, 
				const std::string& absFilePath,
				const FileChangeActionType& actionType) :

			_filePath(filePath),
			_absFilePath(absFilePath),
			_actionType(actionType)
		{}
	};

	class FileWatcher : public Thread {
	private:
		void* handle;
		Mutex* mMutex;
		std::string mWatchingDirectory;
	public:
		
		FileWatcher() :
			mMutex(new Mutex),
			handle(nullptr)
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