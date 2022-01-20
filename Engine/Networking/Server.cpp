#include "Server.hpp"
#include <Core/Logger.hpp>
#include "NetworkingEvents.hpp"
#include <Engine/Application.hpp>

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace VSGE;

EnetGameServer::EnetGameServer() {
	_port = 54683;
	_max_connections = 1024;
	_enet_server = nullptr;
}

EnetGameServer::~EnetGameServer() {
	StopServer();
}

void EnetGameServer::SetServerPort(uint16 port) {
	_port = port;
}

void EnetGameServer::SetMaxConnections(uint32 max_connections) {
	_max_connections = max_connections;
}

bool EnetGameServer::StartServer(uint16 port) {
	_port = port;

	return StartServer();
}

bool EnetGameServer::StartServer() {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = _port;

	_enet_server = enet_host_create(&address, _max_connections, 2, 0, 0);
	if (!_enet_server) {
		Logger::Log() << "Failed to create server at port " << _port << "\n";
		return false;
	}

    Logger::Log() << "Successfully started server at port " << _port << "\n";

	return true;
}

void EnetGameServer::StopServer() {
    if (_enet_server) {
        enet_host_destroy(_enet_server);
        _enet_server = nullptr;
    }
}

void EnetGameServer::server_events_loop() {
    while (_enet_server) {
        ProcessEvents();
    }
}

void EnetGameServer::ProcessEvents() {
    ENetEvent event;
    const int result = enet_host_service(_enet_server, &event, 0);

    if (result > 0)
    {
        // Copy sender data
        const uint32 connectionId = enet_peer_get_id(event.peer);

        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:

            NetworkClientConnectedEvent* connect_event = 
                new NetworkClientConnectedEvent(connectionId);
            Application::Get()->QueueEvent(connect_event);

            _peers.insert(std::make_pair(connectionId, event.peer));
            break;

        case ENET_EVENT_TYPE_DISCONNECT:

            NetworkClientDisconnectedEvent* disconnect_event =
                new NetworkClientDisconnectedEvent(connectionId);
            Application::Get()->QueueEvent(disconnect_event);

            _peers.erase(connectionId);
            break;

        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:

            _peers.erase(connectionId);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            NetworkServerDataReceiveEvent* receive_event =
                new NetworkServerDataReceiveEvent(
                    event.channelID,
                    event.packet->data,
                    event.packet->dataLength);
            Application::Get()->QueueEvent(receive_event);

            break;

        default:
            break;
        }
    }
}