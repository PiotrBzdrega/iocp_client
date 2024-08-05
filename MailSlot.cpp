#include "MailSlot.h"
#include "WSAErrors.h"


#include <cstdio>
#include <stdexcept>
#include <array>


namespace MailSlot
{
    Receiver::Receiver(std::function<void(std::string_view)> callback, const wchar_t* ms_name)
    {
        _handle = init(ms_name);

        /* create thread with function member*/
        read_thr=std::thread(&Receiver::read,this,callback);
    }

    Receiver::~Receiver()
    {
        join();
    }

    void Receiver::join()
    {
        if (_handle)
        {
            //TODO: make sure that this will stop thread otherwise create boolFlag
            CloseHandle(_handle);
        }

        if(read_thr.joinable())
        {
            read_thr.join();
        }
    }

    inline HANDLE Receiver::init(const wchar_t* ms_name)
    {
        // Create a mailslot
        HANDLE hMailslot = CreateMailslotW(ms_name,
                                    0, // no limitation of maximum message size
                                    MAILSLOT_WAIT_FOREVER, // no time-out
                                    NULL); // default security attributes

        if (INVALID_HANDLE_VALUE != hMailslot) 
        {
            std::printf("\nCreateMailslot() was successful.");
            return hMailslot;
        }
        else
        {
            throw std::runtime_error("CreateMailslotW failed: "+  WSA::ErrToString(GetLastError()));
        }

        return nullptr;  //Error
    }

    void Receiver::read(std::function<void(std::string_view)> callback)
    {
        /* The maximum message size, in bytes, allowed for this mailslot. */
        DWORD maxMessageSize; 
        /* The size of the next message, in bytes; MAILSLOT_NO_MESSAGE ((DWORD)-1) There is no next message. */
        DWORD nextSize;
        /* The total number of messages waiting to be read, when the function returns */
        DWORD messageCount;
        /* The amount of time, in milliseconds, a read operation can wait for a message to be written to the mailslot before a time-out occurs */
        DWORD readTimeout;
        /* A pointer to the variable that receives the number of bytes read when using a synchronous hFile parameter */
        DWORD bytesRead;

        /* Retrieves information about the specified mailslot */
        if(!GetMailslotInfo(_handle,&maxMessageSize,&nextSize,&messageCount,&messageCount))
        {
            //error occurs
            return;
        }

        /* not valid */
        if (maxMessageSize<=0)
        {
            /* code */
        }

        /* buffer for ReadFile */
        std::array<char,READ_BUFFER_SIZE> buffer;


        while (1)
        {
            if(ReadFile(_handle, &buffer, buffer.size(), &bytesRead, nullptr))
            {
                // Null-terminate the string
                buffer[bytesRead]='\0'; 

                /* pass string_view */
                callback(std::string_view(buffer.data(),bytesRead+1));
            }
            else
            {
                return;
            }
        }
        
    }

    Sender::Sender(const wchar_t*  ms_name)
    {
        _handle = init(ms_name);

    }

    inline HANDLE Sender::init(const wchar_t* ms_name)
    {

        // /* wchar_t* to char* */
        // size_t len = std::wcstombs(nullptr, ms_name, 0);


        // Open a handle to the mailslot
        HANDLE hMailslot = CreateFileW(
            ms_name, // Mailslot name
            GENERIC_WRITE,                       // Write access
            FILE_SHARE_READ,                     // Shared read access
            nullptr,                             // Default security attributes
            OPEN_EXISTING,                       // Opens an existing mailslot
            FILE_ATTRIBUTE_NORMAL,               // Normal file attributes
            nullptr                              // No template file
        );

        if (INVALID_HANDLE_VALUE != hMailslot) 
        {
            std::printf("\nCreateFile() was successful.\n");
            return hMailslot;
        }
        else
        {
            throw std::runtime_error("CreateFileW failed: "+  WSA::ErrToString(GetLastError()));
        }

        return nullptr;  //Error
    }



    int Sender::write(const std::string &message)
    {

        DWORD bytesWritten;
        if (!WriteFile(_handle, message.c_str(), message.size(), &bytesWritten, nullptr)) 
        {
            throw std::runtime_error("WriteFile failed: "+  WSA::ErrToString(GetLastError()));
        }
        return 0;
    }
}

