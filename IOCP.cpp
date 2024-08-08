#include "IOCP.h"
#include "WSAErrors.h"

#include <stdexcept>

namespace IOCP
{
    IOCP::IOCP()
    {
        _handle = CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL, 0, 0);
        if (!_handle)
        {
            throw std::runtime_error(WSA::ErrToString(GetLastError()));
        }

        /* start Completion Port thread */
        _completionThread= std::thread(&IOCP::waitForCompletion,this);
    }

    IOCP::~IOCP()
    {
        PostQueuedCompletionStatus(_handle, 0, (DWORD) NULL, NULL); //TODO: finish all threads for now only one

        // /* release all handles related to Completion Port */
        // for (auto ctx : _endpoints)
        // {
        //     closesocket(ctx.socket);
        // }

        if (_completionThread.joinable())
        {
            _completionThread.join();
        }

        /* release completion port */
        CloseHandle(_handle); //TODO: wait for finish all IO threads
    }

    void IOCP::waitForCompletion()
    {
        while (1)
        {
            /* code */
        }

    }


    int IOCP::associateHandle(ServerContext* completionKey)
    {
        /* Appends reference to socket in completion port */
        _handle = CreateIoCompletionPort((HANDLE)completionKey->socket, _handle, (ULONG_PTR)completionKey, 0);
        if (!_handle)
        {
            std::cout<<"CreateIoCompletionPort: "<<WSA::ErrToString(GetLastError())<<"\n";
            // closesocket(clientContext->socket);
            return EXIT_FAILURE;
        }
        return 0;
    }
}


