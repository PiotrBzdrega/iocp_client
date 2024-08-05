#pragma once

#include <array>
#include <winsock2.h> //WSABUF
#include <cstdint> //uint16_t
#include <string>
#include <mswsock.h>  //LPFN_CONNECTEX

#include "IOCP.h"
#include "IPeer.h"

namespace IOCP
{

    constexpr auto BUFSIZE = 1024;
    inline constexpr auto ASYNC_RW=2;
    inline constexpr auto ASYNC_TYPES=3;
    enum ASYNC_TYPE { RECV,SEND,CONNECT};

    struct AsyncResult
    {
        OVERLAPPED overlapped;
        DWORD error;
        ServerContext *context;
    };

    class ServerContext : public IPeer
    {
    public:
        ServerContext(std::string ip, uint16_t port);
        std::string _locip;
        uint16_t _locport;
        SOCKET socket;
        std::array<char[BUFSIZE],ASYNC_RW> _buffer;
        std::array<WSABUF,ASYNC_RW> _asyncBuffer;
        std::array<AsyncResult,ASYNC_TYPES> _asyncRes;
    private:
        inline static LPFN_CONNECTEX ConnectEx{nullptr};

        void bondAsync();
        void connect();
        void disconnect();
    };


}

