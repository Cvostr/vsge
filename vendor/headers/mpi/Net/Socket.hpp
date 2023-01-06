#pragma once

#include <Net/NetDefs.hpp>

namespace Mpi {
	class Socket {
	private:
		SOCKET mSocket;

	public:
		Socket();
		Socket(SOCKET socket);
		~Socket();

		SOCKET getNativeSocket();

		bool isInvalid() const;

		void create(int af, int type, int protocol);

		int connect(void* addr_struct, int addr_struct_len);

		int listen(int n_connections);

		int send(const char* data, int size, int flags);
		
		int recv(char* data, int size, int flags);

		int shutdown(int how);

		SOCKET accept(void* addr_struct, int* struct_size);

		int close();
	};
}