#pragma once
#include <windows.h>

namespace IOCP
{
    constexpr auto DATA_BUFSIZE = 8192;
    inline constexpr auto OVERLAPPED_TYPES=3;
    enum OVERLAPPED_TYPE { RECV,SEND,CONNECT};
}



// typedef struct
// {
//     OVERLAPPED  Overlapped;
//     char        Buffer[DATA_BUFSIZE];
//     int         BufferLen;
//     int         OperationType;
// } PER_IO_DATA, * LPPER_IO_DATA;