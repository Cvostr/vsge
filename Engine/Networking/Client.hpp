#pragma once

#include <Core/VarTypes/Base.hpp>
#include <enet/enet.h>
#include <string>

namespace VSGE {
	class EnetClient {
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