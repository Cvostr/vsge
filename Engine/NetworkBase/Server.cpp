#include "Server.hpp"

using namespace VSGE;

Server::Server() {
	_server_socket = 0;
	_status = ServerStatus::SERVER_STATUS_CLOSED;
	_port = 29675;
}
Server::~Server() {

}

void Server::Start(uint16 port, InetProtocolType protocol) {
	_port = port;
	SOCKADDR_IN address;
	address.sin_addr.S_un.S_addr = INADDR_ANY; //Any IP address
	address.sin_port = htons(port); //Setting port
	address.sin_family = AF_INET; //IPv4 addresses

	//try to create socket
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == INVALID_SOCKET) {

	}
}
void Server::Stop() {

}

uint16 Server::GetPort() {
	return _port;
}
ServerStatus Server::GetStatus() {
	return _status;
}