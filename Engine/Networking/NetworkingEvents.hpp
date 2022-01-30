#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Engine/Event.hpp>
#include "Server.hpp"
#include "Client.hpp"

namespace VSGE {

	class NetworkClientConnectedEvent : public IEvent {
	private:
		IGameServer* _server;
		uint32 _connection_id;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientConnected);

		NetworkClientConnectedEvent(IGameServer* server, uint32 id);

		IGameServer* GetServer() const;

		uint32 GetConnectionId() const;
	};

	class NetworkClientDisconnectedEvent : public IEvent {
	private:
		IGameServer* _server;
		uint32 _connection_id;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientDisconnected);

		NetworkClientDisconnectedEvent(IGameServer* server, uint32 id);

		IGameServer* GetServer() const;

		uint32 GetConnectionId() const;
	};

	class NetworkServerDataReceiveEvent : public IEvent {
	private:
		IGameServer* _server;
		uint32 _client_connection_id;
		byte* _data;
		uint32 _data_size;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkServerDataReceive);

		NetworkServerDataReceiveEvent(IGameServer* server, uint32 id, byte* data, uint32 size);

		IGameServer* GetServer();

		uint32 GetConnectionId();

		byte* GetData();

		uint32 GetDataSize();

		~NetworkServerDataReceiveEvent();
	};

	class NetworkClientDataReceive : public IEvent {
	private:
		IGameClient* _client;
		byte* _data;
		uint32 _data_size;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientDataReceive);

		NetworkClientDataReceive(IGameClient* client, byte* data, uint32 size);

		IGameClient* GetClient();

		byte* GetData();

		uint32 GetDataSize();

		~NetworkClientDataReceive();
	};
}