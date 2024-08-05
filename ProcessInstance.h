#pragma once

#ifdef  __linux__
#include <sys/file.h> //flock, open
#include <unistd.h> //close, read, write
    using HANDLE = int;
    constexpr auto lockName="/var/run/client.pid";
#elif _WIN32
#include "windows.h"
#include "WSAErrors.h"
    constexpr auto lockName=L"FirstInstance";
#endif


#include <string>
#include <stdexcept> //throw

namespace COM
{
    
    class ProcessInstance
    {
    private:
        HANDLE _handle;
        bool isParent;
    public:
        ProcessInstance() : isParent(false)
        {
#ifdef  __linux__
            /* create if do not exist */
            _handle = open(lockName, O_CREAT | O_RDWR, 0666);
            if (_handle == -1) {
                throw std::runtime_error("open "+ std::string(lockName));
            }
            /*The fcntl() system call provides record
            locking, allowing processes to place multiple read and write locks on different
            regions of the same file.*/

            struct flock fl;

            // Initialize the flock structure
            fl.l_type = F_WRLCK;   // Exclusive lock
            fl.l_whence = SEEK_SET; //Seek from beginning of file
            fl.l_start = 0;
            fl.l_len = 0; //whole file if 0

            // Attempt to acquire the lock
            int rc = fcntl(_handle, F_SETLK, &fl); //Set record locking info (non-blocking)

            if (rc && EWOULDBLOCK == errno)
            {
                /* Another instance, file already locked */
                // rc = fcntl(_handle, F_GETLK, &fl); //Get record locking info
                // std::cout<<"\033[0;91m Process ID of the process that holds the blocking lock "<<fl.l_pid<<"\033[0m\n";               
            }
            else if (rc)
            {
                throw std::runtime_error("fcntl "+ std::string(lockName));
            }
            
            isParent = (rc == 0);    
#elif _WIN32

            // Open the file
            _handle = CreateFileW(
            lockName,                           // file to open
            GENERIC_READ | GENERIC_WRITE,       // open for reading and writing
            0,                                  // do not share
            NULL,                               // default security
            CREATE_NEW,                         // creates a new file, only if it does not already exist.
            FILE_ATTRIBUTE_HIDDEN,              // normal file
            NULL);                              // no attr. template

            if (_handle == INVALID_HANDLE_VALUE) 
            {
                throw std::runtime_error("CreateFile " + WSA::ErrToString(GetLastError()) );
            }

            // // Lock the file
            // bResult = LockFileEx(
            // hFile,                      // handle to file
            // LOCKFILE_EXCLUSIVE_LOCK,    // exclusive access
            // 0,                          // reserved, must be zero
            // MAXDWORD,                   // number of bytes to lock (low order)
            // MAXDWORD,                   // number of bytes to lock (high order)
            // &ol);                       // overlapped structure

            // _handle=CreateMutexW(NULL,TRUE,lockName);
            isParent=_handle != nullptr;
#endif
        }
        ~ProcessInstance()
        {
            if (_handle)
            {
#ifdef __linux__
                close(_handle);
#elif _WIN32
                CloseHandle(_handle);
#endif
            }
        }
        bool operator()()
        {
            return isParent;
        }
    };
}

