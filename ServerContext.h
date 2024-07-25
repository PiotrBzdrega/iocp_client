#pragma once

#include <array>
#include "IOCP.h"
#include <winsock2.h>


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
    private:
        std::array<char[BUFSIZE],ASYNC_RW> _buffer;
        std::array<WSABUF,ASYNC_RW> _asyncBuffer;
        std::array<AsyncResult,ASYNC_TYPES> _asyncRes;
    public:
        ServerContext(/* args */)
        {
            /* assign pointer to the current object instance */
            for (auto &i : _asyncRes)
            {
               i.context=this;
            }

            /* copy out addresses of buffers */
            _asyncBuffer[RECV].buf=_buffer[RECV];
            _asyncBuffer[RECV].len=BUFSIZE;

            _asyncBuffer[SEND].buf=_buffer[SEND];
            _asyncBuffer[SEND].len=0;

        };
        ~ServerContext();
    };


}

