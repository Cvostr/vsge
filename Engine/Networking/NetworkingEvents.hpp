#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Engine/Event.hpp>

namespace VSGE {
	class NetworkClientConnectedEvent : public IEvent {
	private:
		uint32 _connection_id;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientConnected);

		NetworkClientConnectedEvent(uint32 id) :
			_connection_id(id)
		{}
	};
}