#pragma once

#include "../Client.hpp"
#include <enet/enet.h>
#include <thread>
#include <mutex>

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace VSGE {
	class EnetGameClient : public IGameClient {
	private:
		uint16 _port;
		std::string _ip;
		ENetHost* _enet_client;
		ENetPeer* _enet_peer;

		std::thread _client_thread;
		std::mutex _client_mutex;

		void client_events_loop();
		void process_events();
	public:

		EnetGameClient();
		~EnetGameClient();

		bool Connect(const std::string& ip, uint16 port);
		bool Connect();

		void Disconnect();

		void SendPacket(byte* data, uint32 size, bool reliable = false);
	};
}