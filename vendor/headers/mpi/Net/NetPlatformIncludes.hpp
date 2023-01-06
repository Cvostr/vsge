#pragma once

#ifdef __linux__
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define _mpi_sock_close close
#endif

#ifdef _WIN32
#include <WinSock2.h>
#include <mstcpip.h>

#define _mpi_sock_close closesocket
#endif