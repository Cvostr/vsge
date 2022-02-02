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

		IGameServer* GetServer() const;

		uint32 GetConnectionId() const;

		byte* GetData() const;

		uint32 GetDataSize() const;

		~NetworkServerDataReceiveEvent();
	};

	class NetworkClientDataReceiveEvent : public IEvent {
	private:
		IGameClient* _client;
		byte* _data;
		uint32 _data_size;
	public:
		EVENT_CLASS_TYPE(EventType::EventNetworkClientDataReceive);

		NetworkClientDataReceiveEvent(IGameClient* client, byte* data, uint32 size);

		IGameClient* GetClient() const;

		byte* GetData() const;

		uint32 GetDataSize() const;

		~NetworkClientDataReceiveEvent();
	};

	enum ClientDisconnectedByServerType {
		DISCONNECTION_TYPE_GRACEFULLY,
		DISCONNECTION_TYPE_CONN_RESET
	};

	class NetworkClientDisconnectedByServerEvent : public IEvent {
	private:
		IGameClient* _client;
		ClientDisconnectedByServerType _disconnection_type;
	public:
		EVENT_CLASS_TYPE(EventType::EventClientDisconnectedByServer);

		NetworkClientDisconnectedByServerEvent(IGameClient* client, ClientDisconnectedByServerType disc_type);

		~NetworkClientDisconnectedByServerEvent(){}

		IGameClient* GetClient() const;

		ClientDisconnectedByServerType GetDisconnectionType() const;
	};
}