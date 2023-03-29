#pragma once

#ifdef __linux__
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#define _mpi_sock_close close
#define LastNetError errno
#endif

#ifdef _WIN32
#include <WinSock2.h>
#include <mstcpip.h>

#define _mpi_sock_close closesocket

static inline int poll(struct pollfd* pfd, int nfds, int timeout) {
	return WSAPoll(pfd, nfds, timeout);
}

#define LastNetError ::WSAGetLastError()

int _mpi_WinsockInit();
#endif