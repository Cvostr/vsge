#include "Client.hpp"
#include <Core/Logger.hpp>

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>
#undef _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace VSGE;

EnetClient::EnetClient() {
	_port = 14683;
	_ip = "172.0.0.1";
}
EnetClient::~EnetClient() {
	Disconnect();
}

bool EnetClient::Connect(const std::string& ip, uint16 port) {
	_ip = ip;
	_port = port;

	return Connect();
}

bool EnetClient::Connect() {
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

	return true;
}

void EnetClient::Disconnect() {
	if (_enet_peer)
	{
		enet_peer_disconnect_now(_enet_peer, 0);
		_enet_peer = nullptr;
	}
}