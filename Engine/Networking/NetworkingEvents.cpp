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

IGameServer* NetworkServerDataReceiveEvent::GetServer() const{
	return _server;
}

uint32 NetworkServerDataReceiveEvent::GetConnectionId() const{
	return _client_connection_id;
}

byte* NetworkServerDataReceiveEvent::GetData() const{
	return _data;
}

uint32 NetworkServerDataReceiveEvent::GetDataSize() const{
	return _data_size;
}

NetworkServerDataReceiveEvent::~NetworkServerDataReceiveEvent() {
	SAFE_RELEASE(_data);
}

//-----------------Client received data from server

NetworkClientDataReceiveEvent::NetworkClientDataReceiveEvent(IGameClient* client, byte* data, uint32 size) :
	_client(client),
	_data_size(size)
{
	//copy data
	_data = new byte[size];
	memcpy(_data, data, size);
}

IGameClient* NetworkClientDataReceiveEvent::GetClient() const{
	return _client;
}

byte* NetworkClientDataReceiveEvent::GetData() const{
	return _data;
}

uint32 NetworkClientDataReceiveEvent::GetDataSize() const{
	return _data_size;
}

NetworkClientDataReceiveEvent::~NetworkClientDataReceiveEvent() {
	SAFE_RELEASE(_data);
}