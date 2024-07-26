#pragma once

#include <array>
#include <winsock2.h> //WSABUF
#include <cstdint> //uint16_t
#include <string>


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

    class ServerContext
    {
    public:
        ServerContext(std::string ip, uint16_t port);
        std::string _ip;
        uint16_t _port;
        SOCKET socket;
        std::array<char[BUFSIZE],ASYNC_RW> _buffer;
        std::array<WSABUF,ASYNC_RW> _asyncBuffer;
        std::array<AsyncResult,ASYNC_TYPES> _asyncRes;
    private:


        void bondAsync();
        void connect();

    };


}

