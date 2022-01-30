#include "NetworkingEvents.hpp"
#include "string.h"

using namespace VSGE;

NetworkClientConnectedEvent::NetworkClientConnectedEvent(IGameServer* server, uint32 id) :
	_server(server),
	_connection_id(id)
{}

IGameServer* NetworkClientConnectedEvent::GetServer() const{
	return _server;
}

uint32 NetworkClientConnectedEvent::GetConnectionId() const{
	return _connection_id;
}

//--------------Client disconnected from server

NetworkClientDisconnectedEvent::NetworkClientDisconnectedEvent(IGameServer* server, uint32 id) :
	_server(server),
	_connection_id(id)
{}

IGameServer* NetworkClientDisconnectedEvent::GetServer() const{
	return _server;
}

uint32 NetworkClientDisconnectedEvent::GetConnectionId() const{
	return _connection_id;
}

//-----------------Server received data from client

NetworkServerDataReceiveEvent::NetworkServerDataReceiveEvent(IGameServer* server, uint32 id, byte* data, uint32 size) :
	_server(server),
	_client_connection_id(id),
	_data_size(size)
{
	//copy data
	_data = new byte[size];
	memcpy(_data, data, size);
}

IGameServer* NetworkServerDataReceiveEvent::GetServer() {
	return _server;
}

uint32 NetworkServerDataReceiveEvent::GetConnectionId() {
	return _client_connection_id;
}

byte* NetworkServerDataReceiveEvent::GetData() {
	return _data;
}

uint32 NetworkServerDataReceiveEvent::GetDataSize() {
	return _data_size;
}

NetworkServerDataReceiveEvent::~NetworkServerDataReceiveEvent() {
	SAFE_RELEASE(_data);
}

//-----------------Client received data from server

NetworkClientDataReceive::NetworkClientDataReceive(IGameClient* client, byte* data, uint32 size) :
	_client(client),
	_data_size(size)
{
	//copy data
	_data = new byte[size];
	memcpy(_data, data, size);
}

IGameClient* NetworkClientDataReceive::GetClient() {
	return _client;
}

byte* NetworkClientDataReceive::GetData() {
	return _data;
}

uint32 NetworkClientDataReceive::GetDataSize() {
	return _data_size;
}

NetworkClientDataReceive::~NetworkClientDataReceive() {
	SAFE_RELEASE(_data);
}