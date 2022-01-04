#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
typedef int SOCKET;
#endif

#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	enum class ServerStatus {
		SERVER_STATUS_UP = 0,
		SERVER_STATUS_ERROR = 1,
		SERVER_STATUS_CLOSED = 2
	};

	enum InetProtocolType {
		PROTOCOL_TCP,
		PROTOCOL_UDP
	};

	class Server {
	private:
		SOCKET _server_socket;
		uint16 _port;

		ServerStatus _status;
	public:

		Server();
		~Server();

		void Start(uint16 port, InetProtocolType protocol);
		void Stop();

		uint16 GetPort();
		ServerStatus GetStatus();
	};
}