#pragma once

#include <Core/Base.hpp>
#include <Net/IPAddress4.hpp>

namespace Mpi {
	class NetAddress {
	private:
		uint32_t mIpAddress4;
		uint16_t mPort;

	public:
		NetAddress();
		
		NetAddress(const IPAddress4& ip, uint16_t port);

		NetAddress(const uint32_t ip, uint16_t port);

		NetAddress(uint16_t port);

		void toStdAddrStruct(void** struct_ptr, int& struct_size) const;

		int getFamily() const;

		uint32_t getIpAddress4() const;

		uint16_t getPort() const;
	};
}