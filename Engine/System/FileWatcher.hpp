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
		FCAT_MODIFIED = 0x00000003,
		FCAT_RENAMED = 0x00000004
	};

	class FileChageEvent : public IEvent {
		std::string _filePath;
		std::string _absFilePath;

		std::string _NewFilePath;
		std::string _NewAbsFilePath;

		FileChangeActionType _actionType;
	public:
		EVENT_CLASS_TYPE(EventType::EventFileChanged)

		const std::string& GetFilePath() const { return _filePath; }
		const std::string& GetAbsFilePath() const { return _absFilePath; }

		const std::string& GetNewFilePath() const { return _NewFilePath; }
		const std::string& GetNewAbsFilePath() const { return _NewAbsFilePath; }

		FileChangeActionType GetActionType() const { return _actionType; }

			FileChageEvent(
				const std::string& filePath, 
				const std::string& absFilePath,
				const FileChangeActionType& actionType,
				const std::string& newFilePath = "",
				const std::string& newAbsFilePath = "") :

			_filePath(filePath),
			_absFilePath(absFilePath),
			_actionType(actionType),
			_NewFilePath(newFilePath),
			_NewAbsFilePath(newAbsFilePath)
		{}
	};

	class FileWatcher : public Thread {
	private:
		void* handle;
		Mutex* mMutex;
		std::string mWatchingDirectory;

		std::string _oldFileName;
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