#pragma once

#include "Socket.hpp"
#include "NetAddress.hpp"

namespace Mpi {

    class Server {
    private:
        Socket mSocket;
    public:
        Server();
        ~Server();

        NetResultCode start(uint16_t port);

        Socket accept(NetAddress& clientAddress);

        void stop();
    };

}