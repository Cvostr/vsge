#pragma once

#include "../Client.hpp"
#include <enet/enet.h>

namespace VSGE {
	class EnetClient : public IGameClient {
	private:
		uint16 _port;
		std::string _ip;
		ENetHost* _enet_client;
		ENetPeer* _enet_peer;
	public:

		EnetClient();
		~EnetClient();

		bool Connect(const std::string& ip, uint16 port);
		bool Connect();

		void Disconnect();
	};
}