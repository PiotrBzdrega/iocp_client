#pragma once

#define WIN32_LEAN_AND_MEAN //reduce the size of the Windows header files by excluding some of the less commonly used APIs
#include <winsock2.h>
#include <windows.h>
#include <cstdio>
#include <stdexcept>
#include <iostream>
#include <string>

#include "WSAErrors.h"

class WinSock
{
private:
    WinSock()
    {
        WSADATA wsaData;
        int res=WSAStartup(MAKEWORD(2,2), &wsaData);
        if (res)
        {
            std::cout<<WSA::ErrToString(WSAGetLastError())<<"\n";

            throw std::runtime_error("WSAStartup failed with error: "+res);
            /* WSACleanup should be called olny after succesfull call to WSAStartup */
        }
    }
public:
    WinSock(const WinSock&) = delete;
    WinSock& operator=(const WinSock&) = delete;
    WinSock(WinSock&&) = delete;
    WinSock& operator=(WinSock&&) = delete;

    ~WinSock() 
    {
        WSACleanup();
    }
    inline static WinSock& instance()
    {
       static WinSock inst;
       return inst;
    };
};

