#pragma once

#include <vector>
#include <memory>
#include <string_view>

#include "IOCP.h"
#include "IManager.h"
#include "IServer.h"

namespace IOCP
{
    class EndPoint : public IManager
    {
    private:
        IOCP _completionPort;
        std::vector<std::unique_ptr<IServer>> _peer; //TODO: use unordered map if needed
        void addServer();
        void removeServer();
    public:
        EndPoint(/* args */);
        ~EndPoint();
        void processMail(std::string_view mail); //TODO: MailSlot Sender should obtain callback function that validate correctness of data, \
        then this function should deserialize data 

    };

    


}