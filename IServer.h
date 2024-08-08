#pragma once

#include "IManager.h"
#include <windows.h>
#include <winsock2.h>
#include <string>


class IServer
{
private:
    // IManager* _manager; //TODO: seems like not necessary for now
    std::string _locip;
    uint16_t _locport;
    std::string _ip;
    uint16_t _port;
public:
    IServer(std::string_view locip, uint16_t locport,std::string_view ip, uint16_t port)
    : _locip(locip), _locport(locport), _ip(ip), _port(port) {};
    virtual ~IServer() = default;
    SOCKET socket;
    virtual void processCompletionStatus(LPOVERLAPPED ov) = 0;
};

