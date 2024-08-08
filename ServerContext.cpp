#include "ServerContext.h"

IOCP::ServerContext::ServerContext(std::string ip, uint16_t port) : _locip(ip), _locport(port)
{
    bondAsync();

}

void IOCP::ServerContext::processCompletionStatus(LPOVERLAPPED ov)
{
}

void IOCP::ServerContext::getConnectFunc()
{
}

void IOCP::ServerContext::bondAsync()
{
    if(ConnectEx)


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
}

void IOCP::ServerContext::connect()
{
    if (!ConnectEx)
    {
        /* code */
    }
    
}
