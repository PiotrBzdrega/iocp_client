#pragma once
#include <windows.h>
#include <vector>

#include "ServerContext.h"

namespace IOCP
{
    class IOCP
    {
    private:
        /* to create only once in instance() */
        IOCP();
        HANDLE _handle;
        std::vector<ServerContext> _endpoints;
        
    public:        
        IOCP(const IOCP&) = delete;
        IOCP& operator=(const IOCP&) = delete;
        IOCP(IOCP&&) = delete;
        IOCP& operator=(IOCP&&) = delete;
        ~IOCP();

        inline static IOCP& instance()
        {
            static IOCP inst;
            return inst;
        };

        static int associateHandle();
        static int appendHandle();
    };
}



// typedef struct
// {
//     OVERLAPPED  Overlapped;
//     char        Buffer[DATA_BUFSIZE];
//     int         BufferLen;
//     int         OperationType;
// } PER_IO_DATA, * LPPER_IO_DATA;