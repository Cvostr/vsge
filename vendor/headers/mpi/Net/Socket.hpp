#pragma once

#include <Net/NetDefs.hpp>
#include <string>

namespace Mpi {
	class Socket {
	private:
		SOCKET mSocket;

	public:
		Socket();
		Socket(SOCKET socket);
		~Socket();

		SOCKET getNativeSocket() const;

		bool isInvalid() const;

		void create(int af, int type, int protocol);

		int connect(void* addr_struct, int addr_struct_len);

		int bind(void* addr_struct, int addr_struct_len);

		int listen(int n_connections);

		int send(const char* data, int size, int flags) const;

		int send(const std::string& data, int flags) const;
		
		int recv(char* data, int size, int flags) const;

		int sendto(const char* buffer, int size, void* addr_struct, int addr_struct_len, int flags = 0) const;

		int recvfrom(char* data, int size, void* addr_struct, unsigned int& addr_struct_len, int flags = 0) const;

		int shutdown(int how) const;

		SOCKET accept(void* addr_struct, int* struct_size);

		int close() const;

		int disableBlocking();
	};
}