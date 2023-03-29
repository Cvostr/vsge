#pragma once

#include "Socket.hpp"
#include "NetAddress.hpp"
#include <functional>
#include <Os/Thread.hpp>

namespace Mpi {

    class ClientThreadRunnable;

    class Client {
    private:
        Socket mSocket;
        NetClientStatus mStatus;

        Thread* mWorkerThread;

        std::function<void(const char*, uint32_t)> clientReceiveHandler;
        std::function<void()> clientDisconnectedHandler;

        friend class ClientThreadRunnable;

        void closeSocket();
    public:

        Client();
        ~Client();

        NetClientStatus getStatus();

        NetResultCode connect(const NetAddress& address);

        int send(const char* data, size_t size);

        void disconnect();
        
        void setReceiveHandler(std::function<void(const char*, uint32_t)> const& handler);

        void setDisconnectHandler(std::function<void()> const& handler);
    };

}