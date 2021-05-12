#pragma once

#include "Event.hpp"

namespace VSGE {

	enum MouseButton {
		MOUSE_BUTTON_LEFT = 1,
		MOUSE_BUTTON_MIDDLE,
		MOUSE_BUTTON_RIGHT
	};
	
	enum KeyCode {
		KEY_CODE_A,
		KEY_CODE_B,
		KEY_CODE_C,
		KEY_CODE_D,
		KEY_CODE_E,
		KEY_CODE_F,
		KEY_CODE_G,
		KEY_CODE_H,
		KEY_CODE_I,
		KEY_CODE_J,
		KEY_CODE_K,
		KEY_CODE_L,
		KEY_CODE_M,
		KEY_CODE_N,
		KEY_CODE_O,
		KEY_CODE_P,
		KEY_CODE_Q,
		KEY_CODE_R,
		KEY_CODE_S,
		KEY_CODE_T,
		KEY_CODE_U,
		KEY_CODE_V,
		KEY_CODE_W,
		KEY_CODE_X,
		KEY_CODE_Y,
		KEY_CODE_Z,

		KEY_CODE_LCTRL,
		KEY_CODE_RCTRL,
		KEY_CODE_TAB,
		KEY_CODE_SHIFT,
		KEY_CODE_LALT,
		KEY_CODE_RALT,
		
		KEY_CODE_UP,
		KEY_CODE_DOWN,
		KEY_CODE_LEFT,
		KEY_CODE_RIGHT
	};

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
	private:
		int x;
		int y;
	public:

		EVENT_CLASS_TYPE(EventType::EventWindowRestored)

		int GetWidth() { return x; }
		int GetHeight() { return y; }

		EventWindowResized(int x, int y) : x(x), y(y)
		{
			
		}
	};
}