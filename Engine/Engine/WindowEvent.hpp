#pragma once

namespace VSGE {

	enum class EventType {
		EventNone = 0,
		EventMouseMotion,
		EventMouseButtonDown,
		EventMouseButtonUp,
		EventMouseScrolled,

		EventKeyButtonDown,
		EventKeyButtonUp,
		
		EventWindowMinimized,
		EventWindowRestored,
		EventWindowFocus,
		EventWindowFocusLost,
		EventWindowClose,
		EventWindowResized
	};

	class IWindowEvent {
	public:
		EventType type;

		IWindowEvent() {
			type = EventType::EventNone;
		}
	};

	class EventMouseMotion : public IWindowEvent {
	public:

		int mMouseX;
		int mMouseY;

		int mMouseRelX;
		int mMouseRelY;

		EventMouseMotion(int mX, int mY, int mRx, int mRy) : 
			mMouseX(mX), mMouseY(mY), mMouseRelX(mRx), mMouseRelY(mRy) {
			type = EventType::EventMouseMotion;
		}
	};

	class EventMouseButtonDown : public IWindowEvent {
	public:

		int MouseButton;

		EventMouseButtonDown(int _MouseButton) :
			MouseButton(_MouseButton) {
			type = EventType::EventMouseButtonDown;
		}
	};
	class EventMouseButtonUp : public IWindowEvent {
	public:

		int MouseButton;

		EventMouseButtonUp(int mouseButton) :
			MouseButton(mouseButton) {
			type = EventType::EventMouseButtonUp;
		}
	};

	class EventMouseScrolled : public IWindowEvent {
	public:

		int xOffset;
		int yOffset;

		EventMouseScrolled(int xOffset, int yOffset) :
			xOffset(xOffset), yOffset(yOffset) {
			type = EventType::EventMouseScrolled;
		}
	};


	class EventKeyButtonDown : public IWindowEvent {
	public:

		int KeyCode;

		EventKeyButtonDown(int keyCode) :
			KeyCode(keyCode) {
			type = EventType::EventKeyButtonDown;
		}
	};
	class EventKeyButtonUp : public IWindowEvent {
	public:

		int KeyCode;

		EventKeyButtonUp(int keyCode) :
			KeyCode(keyCode) {
			type = EventType::EventKeyButtonUp;
		}
	};


	class EventWindowClose : public IWindowEvent {
	public:
		EventWindowClose() {
			type = EventType::EventWindowClose;
		}
	};
	class EventWindowMinimized : public IWindowEvent {
	public:
		EventWindowMinimized() {
			type = EventType::EventWindowMinimized;
		}
	};
	class EventWindowRestored : public IWindowEvent {
	public:
		EventWindowRestored() {
			type = EventType::EventWindowRestored;
		}
	};
	class EventWindowResized : public IWindowEvent {
	public:
		int x;
		int y;
		EventWindowResized(int x, int y) : x(x), y(y)
		{
			type = EventType::EventWindowResized;
		}
	};
}