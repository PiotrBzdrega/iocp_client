#pragma once

#include <array>
#include <winsock2.h> //WSABUF
#include <cstdint> //uint16_t
#include <string>
#include <mswsock.h>  //LPFN_CONNECTEX

#include "IOCP.h"
#include "IServer.h"

namespace IOCP
{

    constexpr auto BUFSIZE = 1024;
    enum ASYNC_TYPE { RECV=0,SEND,CONNECT};

    // struct AsyncResult
    // {
    //     OVERLAPPED overlapped;
    //     DWORD error;
    //     ServerContext *context;
    // };

    class ServerContext : public IServer
    {
    public:
        ServerContext(std::string ip, uint16_t port);
        std::array<char[BUFSIZE],2> _buffer;
        std::array<WSABUF,2> _asyncBuffer;
        std::array<OVERLAPPED,3> _ov;
        void processCompletionStatus(LPOVERLAPPED ov) override;
    private:
        inline static LPFN_CONNECTEX ConnectEx{nullptr};
        void getConnectFunc();
        void bondAsync();
        void connect();
        void disconnect();
    };


}

