#include "Server.hpp"
#include <Core/Logger.hpp>
#include "NetworkingEvents.hpp"

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace VSGE;

Server::Server() {
	_port = 54683;
	_max_connections = 1024;
	_enet_server = nullptr;
}

Server::~Server() {
	StopServer();
}

void Server::SetServerPort(uint16 port) {
	_port = port;
}

void Server::SetMaxConnections(uint32 max_connections) {
	_max_connections = max_connections;
}

bool Server::StartServer(uint16 port) {
	_port = port;

	return StartServer();
}

bool Server::StartServer() {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = _port;

	_enet_server = enet_host_create(&address, _max_connections, 2, 0, 0);
	if (!_enet_server) {
		Logger::Log() << "Failed to create server at port " << _port << "\n";
		return false;
	}
	return true;
}

void Server::StopServer() {
	if(_enet_server)
		enet_host_destroy(_enet_server);
}

void Server::ProcessEvents() {
    ENetEvent event;
    const int result = enet_host_service(_enet_server, &event, 0);

    if (result > 0)
    {
        // Copy sender data
        const uint32 connectionId = enet_peer_get_id(event.peer);

        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:

            _peers.insert(std::make_pair(connectionId, event.peer));
            break;

        case ENET_EVENT_TYPE_DISCONNECT:

            _peers.erase(connectionId);
            break;

        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:

            _peers.erase(connectionId);
            break;

        case ENET_EVENT_TYPE_RECEIVE:

            break;

        default:
            break;
        }
    }
}