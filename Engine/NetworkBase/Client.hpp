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
	class Client {
	private:
		SOCKET _client_socket;
	public:

		Client();
		~Client();


	};
}