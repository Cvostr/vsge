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

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return type; } \
								virtual EventType GetEventType() const { return GetStaticType(); }

#define EVENT_FUNC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

	class IEvent {
	public:
		
		virtual EventType GetEventType() const = 0;

		IEvent() {
	
		}
	};

	template<typename T, typename F>
	bool DispatchEvent(const IEvent& event, const F& event_function) {
		if (event.GetEventType() == T::GetStaticType())
		{
			event_function(static_cast<const T&>(event));
			return true;
		}
		return false;
	}

}