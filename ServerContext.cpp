#include "ServerContext.h"

IOCP::ServerContext::ServerContext(std::string ip, uint16_t port) : _ip(ip), _port(port)
{
    bondAsync();

}

void IOCP::ServerContext::bondAsync()
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
}

void IOCP::ServerContext::connect()
{
    
}
