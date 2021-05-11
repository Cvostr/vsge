#pragma once

#include "Event.hpp"

namespace VSGE {

	enum MouseButton {
		MOUSE_BUTTON_LEFT = 1,
		MOUSE_BUTTON_MIDDLE,
		MOUSE_BUTTON_RIGHT
	};

	class EventMouseMotion : public IEvent {
	private:
		int _mouseX;
		int _mouseY;

		int _mouseRelX;
		int _mouseRelY;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseMotion)

		int GetMouseX() { return _mouseX; }
		int GetMouseY() { return _mouseY; }

		int GetMouseRelativeX() { return _mouseRelX; }
		int GetMouseRelativeY() { return _mouseRelY; }

		EventMouseMotion(int mX, int mY, int mRx, int mRy) : 
			_mouseX(mX), _mouseY(mY), _mouseRelX(mRx), _mouseRelY(mRy) {
		}
	};

	class EventMouseButtonDown : public IEvent {
		MouseButton _mouseButton;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseButtonDown)

		MouseButton GetMouseButton() { return _mouseButton; }

		EventMouseButtonDown(int _MouseButton) :
			_mouseButton((MouseButton)_MouseButton) {
		}
	};
	class EventMouseButtonUp : public IEvent {
		MouseButton _mouseButton;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseButtonUp)

		MouseButton GetMouseButton() { return _mouseButton; }

		EventMouseButtonUp(int mouseButton) :
			_mouseButton((MouseButton)mouseButton) {
		}
	};

	class EventMouseScrolled : public IEvent {
	private:
		int _offsetX;
		int _offsetY;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseScrolled)

		int GetOffsetX() const { return _offsetX; }
		int GetOffsetY() const { return _offsetY; }

		EventMouseScrolled(int xOffset, int yOffset) :
			_offsetX(xOffset), _offsetY(yOffset) {
		}
	};


	class EventKeyButtonDown : public IEvent {
	private:
		int _keyCode;
	public:

		EVENT_CLASS_TYPE(EventType::EventKeyButtonDown)

		int GetKeyCode() { return _keyCode; }

		EventKeyButtonDown(int keyCode) :
			_keyCode(keyCode) {
		}
	};
	class EventKeyButtonUp : public IEvent {
	private:
		int _keyCode;
	public:

		EVENT_CLASS_TYPE(EventType::EventKeyButtonUp)

		int GetKeyCode() { return _keyCode; }

		EventKeyButtonUp(int keyCode) :
			_keyCode(keyCode) {
		}
	};


	class EventWindowClose : public IEvent {
	public:

		EVENT_CLASS_TYPE(EventType::EventWindowClose)

		EventWindowClose() {
		}
	};
	class EventWindowMinimized : public IEvent {
	public:

		EVENT_CLASS_TYPE(EventType::EventWindowMinimized)

		EventWindowMinimized() {
			
		}
	};
	class EventWindowRestored : public IEvent {
	public:

		EVENT_CLASS_TYPE(EventType::EventWindowRestored)

		EventWindowRestored() {
			
		}
	};
	class EventWindowResized : public IEvent {
	public:
		int x;
		int y;

		EVENT_CLASS_TYPE(EventType::EventWindowRestored)

		EventWindowResized(int x, int y) : x(x), y(y)
		{
			
		}
	};
}