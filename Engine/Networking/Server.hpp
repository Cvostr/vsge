#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class IGameServer {
	private:
	public:
		virtual void SetServerPort(uint16 port) = 0;
		virtual void SetMaxConnections(uint32 max_connections) = 0;

		virtual bool StartServer() = 0;
		virtual bool StartServer(uint16 port) = 0;
		virtual void StopServer() = 0;

		virtual uint32 GetConnectedClientsCount() = 0;
		virtual void DisconnectClient(uint32 client_id) = 0;
		virtual void SendPacketToClient(uint32 client_id, byte* data, uint32 size, bool reliable = false) = 0;
	};
}