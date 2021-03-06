#include "Server.hpp"
#include <Core/Logger.hpp>
#include "../NetworkingEvents.hpp"

#ifdef CreateWindow
#undef CreateWindow
#endif

#include <Engine/Application.hpp>


#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace VSGE;

EnetGameServer::EnetGameServer() {
	_port = 54683;
	_max_connections = 128;
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
	ENetAddress address = { 0 };
	address.host = ENET_HOST_ANY;
	address.port = _port;

	_enet_server = enet_host_create(&address, _max_connections, 2, 0, 0);
	if (!_enet_server) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Failed to create server at port " << _port << "\n";
		return false;
	}

    server_thread = std::thread([this] {server_events_loop(); });

    Logger::Log() << "Successfully started server at port " << _port << "\n";

	return true;
}

void EnetGameServer::StopServer() {
    if (_enet_server) {
        enet_host_destroy(_enet_server);
        _enet_server = nullptr;
        server_thread.join();
    }
}

void EnetGameServer::DisconnectClient(uint32 client_id) {
    server_mutex.lock();
    auto it = _peers.find(client_id);
    if (it != _peers.end()) {
        //client exists
        ENetPeer* peer = _peers.at(client_id);
        //disconnect peer gently
        enet_peer_disconnect(peer, 0);
        //remove from peer list
        _peers.erase(client_id);
    }
    server_mutex.unlock();
}

uint32 EnetGameServer::GetConnectedClientsCount() {
    server_mutex.lock();
    uint32 result = static_cast<uint32>(_peers.size());
    server_mutex.unlock();
    return result;
}

void EnetGameServer::SendPacketToClient(uint32 client_id, byte* data, uint32 size, bool reliable) {
    server_mutex.lock();
    auto it = _peers.find(client_id);
    if (it != _peers.end()) {
        //client exists
        ENetPeer* peer = _peers.at(client_id);
        //packet flag
        ENetPacketFlag flag = ENET_PACKET_FLAG_UNSEQUENCED;
        if (reliable)
            flag = ENET_PACKET_FLAG_RELIABLE;
        ENetPacket* packet = enet_packet_create(data, size, flag);
        enet_peer_send(peer, 0, packet);
    }
    server_mutex.unlock();
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
        case ENET_EVENT_TYPE_CONNECT: {

            NetworkClientConnectedEvent* connect_event =
                new NetworkClientConnectedEvent(this, connectionId);

            server_mutex.lock();
            Application::Get()->QueueEvent(connect_event);
            _peers.insert(std::make_pair(connectionId, event.peer));
            server_mutex.unlock();

            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {

            NetworkClientDisconnectedEvent* disconnect_event =
                new NetworkClientDisconnectedEvent(this, connectionId);

            server_mutex.lock();
            Application::Get()->QueueEvent(disconnect_event);
            _peers.erase(connectionId);
            server_mutex.unlock();
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            server_mutex.lock();
            _peers.erase(connectionId);
            server_mutex.unlock();
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            NetworkServerDataReceiveEvent* receive_event =
                new NetworkServerDataReceiveEvent(
                    this,
                    event.peer->connectID,
                    event.packet->data,
                    event.packet->dataLength);

            server_mutex.lock();
            Application::Get()->QueueEvent(receive_event);
            server_mutex.unlock();

            break;
        }
        default:
            break;
        }
    }
}