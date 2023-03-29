#pragma once

#include <Os/Thread.hpp>
#include <Os/Mutex.hpp>
#include <functional>
#include <vector>
#include "NetPlatformIncludes.hpp"
#include "Socket.hpp"
#include "NetAddress.hpp"

namespace Mpi {

    class ServerAcceptThreadRunnable;
    class ServerPollThreadRunnable;

    class Server {
    private:
        Socket                          mSocket;
        NetServerStatus                 mStatus;
        //Потоки
        Thread*                         mAcceptThread;
        Thread*                         mPollThread;
        Mutex                           mClientsMutex;
        //Массив pollfd
        std::vector<pollfd>             mPfds;

        std::function<void(const Socket&, const NetAddress&)>     clientConnectedHandler;
        std::function<void(const Socket&, char*, int)>  clientRecvHandler;
        std::function<void(const Socket&)>              clientDisconnectHandler;

        friend class ServerAcceptThreadRunnable;
        friend class ServerPollThreadRunnable;

        pollfd* getFreePollfd();
        void disconnect(pollfd* pfd);
    public:
        Server();
        ~Server();

        NetResultCode start(uint16_t port);

        NetServerStatus getStatus() const;

        void setClientConnectedHandler(std::function<void(const Socket&, const NetAddress&)> const& handler);
        void setClientReceiveHandler(std::function<void(const Socket&, char*, int)> const& handler);
        void setClientDisconnectHandler(std::function<void(const Socket&)> const& handler);

        void stop();

        std::vector<Socket> getConnectedSockets() const;
    };

}