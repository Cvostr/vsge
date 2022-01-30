#include "Client.hpp"
#include <Core/Logger.hpp>
#include <Engine/Application.hpp>
#include "../NetworkingEvents.hpp"

#ifdef CreateWindow
#undef CreateWindow
#endif

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace VSGE;

EnetGameClient::EnetGameClient() {
	_port = 14683;
	_ip = "172.0.0.1";
}
EnetGameClient::~EnetGameClient() {
	Disconnect();
}

bool EnetGameClient::Connect(const std::string& ip, uint16 port) {
	_ip = ip;
	_port = port;

	return Connect();
}

bool EnetGameClient::Connect() {
	_enet_client = enet_host_create(nullptr, 1, 2, 0, 0);

	if (!_enet_client) {
		return false;
	}

	ENetAddress address = { 0 };
	address.port = _port;
	enet_address_set_host(&address, _ip.c_str());

	_enet_peer = enet_host_connect(_enet_client, &address, 1, 0);

	if (!_enet_peer) {
		return false;
	}

	_client_thread = std::thread([this] {client_events_loop(); });

	return true;
}

void EnetGameClient::Disconnect() {
	if (_enet_peer)
	{
		enet_peer_disconnect_now(_enet_peer, 0);
		_enet_peer = nullptr;
	}
}

void EnetGameClient::client_events_loop() {
	while (_enet_peer) {
		process_events();
	}
}

void EnetGameClient::process_events() {
	ENetEvent event;
	const int result = enet_host_service(_enet_client, &event, 0);

    if (result > 0)
    {
        // Copy sender data
        const uint32 connectionId = enet_peer_get_id(event.peer);

        switch (event.type)
        {
        case ENET_EVENT_TYPE_DISCONNECT: {
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE: {
            NetworkClientDataReceive* receive_event =
                new NetworkClientDataReceive(
                    this,
                    event.packet->data,
                    event.packet->dataLength);

            _client_mutex.lock();
            Application::Get()->QueueEvent(receive_event);
			_client_mutex.unlock();

            break;
        }
        default:
            break;
        }
    }
}