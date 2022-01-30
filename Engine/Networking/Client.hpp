#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>

namespace VSGE {
	class IGameClient {
	public:
		virtual bool Connect(const std::string& ip, uint16 port) = 0;
		virtual bool Connect() = 0;

		virtual void Disconnect() = 0;

		virtual void SendPacket(byte* data, uint32 size, bool reliable = false) = 0;
	};
}