#pragma once
#include <windows.h>
#include <vector>
#include <thread>
#include <memory> //std::shared_ptr

#include "ServerContext.h"

namespace IOCP
{
    class IOCP
    {
    private:
        static HANDLE _handle;
        // static std::vector<ServerContext> _endpoints;
        std::thread _completionThread;
        void waitForCompletion();
    public:        
        IOCP();
        int associateHandle(ServerContext* completionKey);        
    };
}



// typedef struct
// {
//     OVERLAPPED  Overlapped;
//     char        Buffer[DATA_BUFSIZE];
//     int         BufferLen;
//     int         OperationType;
// } PER_IO_DATA, * LPPER_IO_DATA;