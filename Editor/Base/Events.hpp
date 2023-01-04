#pragma once

#include <Engine/Event.hpp>

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
}