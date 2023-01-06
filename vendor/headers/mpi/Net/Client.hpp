#pragma once

#include "Socket.hpp"
#include "NetAddress.hpp"

namespace Mpi {

    class Client {
    private:
        Socket mSocket;
        NetClientStatus mStatus;
    public:

        Client();
        ~Client();

        NetClientStatus getStatus();

        NetResultCode connect(const NetAddress& address);

        int send(const char* data, size_t size);

        int recv(char* buffer, size_t size);

        void disconnect();
    };

}