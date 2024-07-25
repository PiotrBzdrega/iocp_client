#pragma once

#include <array>
#include "IOCP.h"


namespace IOCP
{
    

    class ServerContext
    {
    private:
        std::array<OVERLAPPED_TYPE,OVERLAPPED_TYPES> 
    public:
        ServerContext(/* args */);
        ~ServerContext();
    };

    ServerContext::ServerContext(/* args */)
    {
    }

    ServerContext::~ServerContext()
    {
    }
}

