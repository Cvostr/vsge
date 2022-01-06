#pragma once

#include <Core/VarTypes/Base.hpp>
#include <enet/enet.h>
#include <map>

namespace VSGE {
	class Server {
	private:
		uint16 _port;
		uint32 _max_connections;
		ENetHost* _enet_server;
		std::map<uint32, ENetPeer*> _peers;
	public:

		Server();
		~Server();

		void SetServerPort(uint16 port);
		void SetMaxConnections(uint32 max_connections);

		bool StartServer();
		bool StartServer(uint16 port);
		void StopServer();
		void ProcessEvents();

	};
}