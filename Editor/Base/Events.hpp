#pragma once

#include <Engine/Event.hpp>
#include <mpi/Filesystem/FilesystemWatcherEvent.hpp>

namespace VSGE {

	class EventSceneViewResized : public IEvent {
	private:
		int _sizeX;
		int _sizeY;
	public:

		EVENT_CLASS_TYPE(EventType::EventSceneViewResized)

		int GetSizeX() const { return _sizeX; }
		int GetSizeY() const { return _sizeY; }

		EventSceneViewResized(int mX, int mY) : 
			_sizeX(mX), _sizeY(mY) {
		}
	};

	class FilesystemEvent : public IEvent {
		Mpi::FilesystemWatcherEvent event;
	public:
		EVENT_CLASS_TYPE(EventType::EventFileChanged)

		const Mpi::FilesystemWatcherEvent& getFilesystemWatcherEvent() const {
			return event;
		} 

		FilesystemEvent(const Mpi::FilesystemWatcherEvent& event) :
			event(event)
		{}
	};
}