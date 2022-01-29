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
	};
}