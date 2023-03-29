#pragma once

#include <Core/Base.hpp>

#ifdef __linux__
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#ifdef _WIN32
typedef uint64_t SOCKET;
typedef int socklen_t;
#endif

enum NetClientStatus {
    CLIENT_STATUS_DISCONNECTED,
    CLIENT_STATUS_CONNECTED
};

enum NetServerStatus {
    SERVER_STATUS_DOWN,
    SERVER_STATUS_UP
};

enum class NetResultCode {
    NET_SUCCESS = 0,
    NET_SOCKET_CREATE_ERROR,
    NET_CLIENT_CONNECTION_ERROR,
    NET_SERVER_BIND_ERROR,
    NET_SERVER_LISTEN_ERROR
};

#define NET_COMMON_MTU 1500