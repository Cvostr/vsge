#pragma once

#include "Event.hpp"
#include "Input/InputConstants.hpp"

namespace VSGE {

	class EventMouseMotion : public IEvent {
	private:
		int _mouseX;
		int _mouseY;
		int _mouseRelX;
		int _mouseRelY;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseMotion)

		int GetMouseX() const { return _mouseX; }
		int GetMouseY() const { return _mouseY; }

		int GetMouseRelativeX() const { return _mouseRelX; }
		int GetMouseRelativeY() const { return _mouseRelY; }

		EventMouseMotion(int mX, int mY, int mRx, int mRy) : 
			_mouseX(mX), _mouseY(mY), _mouseRelX(mRx), _mouseRelY(mRy) {
		}
	};

	class EventMouseButtonDown : public IEvent {
		MouseButton _mouseButton;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseButtonDown)

		MouseButton GetMouseButton() const { return _mouseButton; }

		EventMouseButtonDown(int _MouseButton) :
			_mouseButton((MouseButton)_MouseButton) {
		}
	};
	class EventMouseButtonUp : public IEvent {
		MouseButton _mouseButton;
	public:

		EVENT_CLASS_TYPE(EventType::EventMouseButtonUp)

		MouseButton GetMouseButton() const { return _mouseButton; }

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
		KeyCode _keyCode;
	public:

		EVENT_CLASS_TYPE(EventType::EventKeyButtonDown)

		KeyCode GetKeyCode() const { return _keyCode; }

		EventKeyButtonDown(KeyCode keyCode) :
			_keyCode(keyCode) {
		}
	};
	class EventKeyButtonUp : public IEvent {
	private:
		KeyCode _keyCode;
	public:

		EVENT_CLASS_TYPE(EventType::EventKeyButtonUp)

		KeyCode GetKeyCode() const { return _keyCode; }

		EventKeyButtonUp(KeyCode keyCode) :
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
	private:
		int _oldWidth;
		int _oldHeight;

		int x;
		int y;
	public:

		EVENT_CLASS_TYPE(EventType::EventWindowResized)

		int GetWidth() const { return x; }
		int GetHeight() const { return y; }

		int GetOldWidth() const { return _oldWidth; }
		int GetOldHeight() const { return _oldHeight; }

		EventWindowResized(int x, int y, int oldWidth, int oldHeight) : x(x), y(y), _oldWidth(oldWidth), _oldHeight(oldHeight)
		{
			
		}
	};
}