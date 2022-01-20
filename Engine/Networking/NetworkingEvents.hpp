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

	class NetworkClientDisconnectedEvent : public IEvent {
	private:
		uint32 _connection_id;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientDisconnected);

		NetworkClientDisconnectedEvent(uint32 id) :
			_connection_id(id)
		{}
	};

	class NetworkServerDataReceiveEvent : public IEvent {
	private:
		uint32 _client_connection_id;
		byte* _data;
		uint32 _data_size;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkServerDataReceive);

		NetworkServerDataReceiveEvent(uint32 id, byte* data, uint32 size) :
			_client_connection_id(id),
			_data_size(size)
		{
			//copy data
			_data = new byte[size];
			memcpy(_data, data, size);
		}

		~NetworkServerDataReceiveEvent() {
			SAFE_RELEASE(_data);
		}
	};

	class NetworkClientDataReceive : public IEvent {
	private:
		byte* _data;
		uint32 _data_size;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientDataReceive);

		NetworkClientDataReceive(byte* data, uint32 size) :
			_data_size(size)
		{
			//copy data
			_data = new byte[size];
			memcpy(_data, data, size);
		}

		~NetworkClientDataReceive() {
			SAFE_RELEASE(_data);
		}
	};
}