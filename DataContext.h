#pragma once

#include <windows.h>
#include <winsock2.h> 
#include <array>
#include <string_view>
#include <cstring>  // std::memcpy


inline constexpr auto DATA_BUFSIZE = 8192;

enum ASYNC_TYPE { RECV,SEND,CONNECT,EXTERNAL};

class DataContext
{
public:
    explicit DataContext(SOCKET soc) : socket(soc)
    {
        for (auto& ov : overlapped)
        {
            ZeroMemory(&ov, sizeof(ov));
        }

        // Initialize the WSABUF structure
        wsabuf[RECV].buf = buffer[RECV];
        wsabuf[RECV].len = sizeof(buffer[RECV]);

        wsabuf[SEND].buf = buffer[SEND];
        wsabuf[SEND].len = 0;

    }
    SOCKET socket;
    std::array<char[DATA_BUFSIZE],2> buffer;
    std::array<WSABUF,2> wsabuf;
    std::array<WSAOVERLAPPED,4> overlapped;
    DWORD bytesReceived{0};
    void prepareRecv()
    {
        bytesReceived=0;
        std::memset(buffer[RECV],0, sizeof(buffer[RECV]));
    }
    void prepareSend(const std::string_view msg)
    {
        std::memcpy(buffer[SEND], msg.data(), msg.size());
        buffer[SEND][msg.size()]='\0';
        wsabuf[SEND].len = msg.size()+1;
    }
};
