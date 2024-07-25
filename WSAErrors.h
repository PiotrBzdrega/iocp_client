#pragma once

#include <windows.h>
#include <string>

namespace WSA
{
    inline std::string ErrToString(int errCode)
    {
        LPVOID messageBuffer = nullptr;
        // Use FormatMessage to get the error message string
        size_t messageSize = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&messageBuffer,
            0,
            NULL
        );

        std::string msg((LPTSTR)messageBuffer, messageSize);

        // Free the buffer allocated by FormatMessage
        LocalFree(messageBuffer);

        return msg;
    }
}


