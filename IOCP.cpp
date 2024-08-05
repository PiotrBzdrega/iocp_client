#include "IOCP.h"
#include "WSAErrors.h"

#include <stdexcept>

IOCP::IOCP::IOCP()
{
    _handle = CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL, 0, 0);
    if (!_handle)
    {
        throw std::runtime_error(WSA::ErrToString(GetLastError()));
    }
}

IOCP::IOCP::~IOCP()
{
    send 
    PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) NULL, NULL);
    and wait for finnisha all threads

    /* release all handles related to Completion Port */
    for (auto ctx : _endpoints)
    {
        closesocket(ctx.socket);
    }
    

    /* release completion port */
    CloseHandle(_handle); //TODO: wait for finish all IO threads
}


int IOCP::IOCP::associateHandle(ServerContext serCtx)
{
    serCtx.
    return 0;
}
