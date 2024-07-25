#pragma once

#include <windows.h>



constexpr auto DATA_BUFSIZE = 8192;

class DataContext
{
public:
    explicit DataContext(SOCKET soc) : socket(soc){ ZeroMemory(&overlapped, sizeof(overlapped));}
    SOCKET socket;
    WSAOVERLAPPED overlapped;
    char buffer[DATA_BUFSIZE];
    WSABUF wsabuf;
    DWORD bytesReceived{0};
};
