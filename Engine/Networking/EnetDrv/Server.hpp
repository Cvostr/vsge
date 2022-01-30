#pragma once

#include "../Server.hpp"
#include <enet/enet.h>
#include <map>
#include <thread>
#include <mutex>

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace VSGE {
	class EnetGameServer : public IGameServer {
	private:
		uint16 _port;
		uint32 _max_connections;
		ENetHost* _enet_server;
		std::map<uint32, ENetPeer*> _peers;

		std::thread server_thread;
		std::mutex server_mutex;

		void server_events_loop();
	public:

		EnetGameServer();
		~EnetGameServer();

		void SetServerPort(uint16 port);
		void SetMaxConnections(uint32 max_connections);
		void DisconnectClient(uint32 client_id);

		bool StartServer();
		bool StartServer(uint16 port);
		void StopServer();
		void ProcessEvents();

	};
}