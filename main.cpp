#include <iostream>
#include <cstring> //std::memset
#include "WinSockInit.h"
#include "DataContext.h"
// #include "IOCP.h"

/* https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5i.html */
/* https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5j.html */
/* https://www.codeproject.com/Articles/10330/A-simple-IOCP-Server-Client-Class */
/* https://www.codeproject.com/Articles/14462/Build-your-own-cryptographically-safe-server-clien */
/*goal https://famellee.wordpress.com/2013/02/20/use-openssl-with-io-completion-port-and-certificate-signing/ */


/*
GLOSSARY
[*] idea
[!] info

https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5.html#:~:text=%C2%A0%C2%A0%C2%A0%C2%A0%C2%A0%C2%A0%C2%A0%20%7D-,SetEvent(hEvent)%3B,-%7D
[*] Create different thread for Reading and Processing data,
if enough data will be provided in Reading -> SetEvent(hEvent).
Processing will be -> WaitForSingleObject(hEvent)


https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5.html#:~:text=Table%205%2D2%20WSAEWOULDBLOCK%20Errors%20on%20Non%2Dblocking%20Sockets
[!] Set socket non-blocking
calls like: 
WSAAccept(), accept(), closesocket(), WSAConnect(), 
connect(), WSARecv(), recv(), WSARecvFrom(), recvfrom(),
WSASend(), send(), WSASendTo(), and sendto()
fail with the error WSAEWOULDBLOCK
nRet = ioctlsocket(s, FIONBIO, (unsigned long *) &ul);

https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5.html#:~:text=an%20asynchronous%20fashion.-,Socket%20I/O%20Models,-Essentially%2C%20six%20types
[!]Socket I/O Models
1.blocking
2.select()
3.WSAAsyncSelect()
4.WSAEventSelect()
5.overlapped, and
6.completion port

https://www.codeproject.com/Articles/10330/A-simple-IOCP-Server-Client-Class
WSACreateEvent()
WSAEventSelect(FD_ACCEPT)
WSAWaitForMultipleEvents()
WSAEnumNetworkEvents()
WSAAccept()
*/


int main()
{
    std::cout<<"Kitos build: "<<__DATE__<<"  " << __TIME__<<"\n";

    /* Initialize WinSock*/
    auto& winsock = WinSock::instance();

    /* Creates an input/output (I/O) completion port w/o reference to socket */
    HANDLE IOCP_handle = CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL, 0, 0);
    if (!IOCP_handle)
    {
        std::cout<<"CreateIoCompletionPort:"<<WSA::ErrToString(GetLastError())<<"\n";
        return EXIT_FAILURE;
    }

    /* create socket for client */
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout<<"socket:"<<WSA::ErrToString(WSAGetLastError())<<"\n";
        closesocket(clientSocket);
        return EXIT_FAILURE;
    }

    auto on = 1;
    if(setsockopt(clientSocket,SOL_SOCKET,SO_EXCLUSIVEADDRUSE,(char *)&on,sizeof(on))) /* The SO_EXCLUSIVEADDRUSE option prevents other sockets from being forcibly bound to the same address and port */
    {
        std::cout<<"setsockopt:"<<WSA::ErrToString(WSAGetLastError())<<"\n";
        closesocket(clientSocket);
        /* release completion port */
        return EXIT_FAILURE;
    }

    SOCKADDR_IN SockAddr{0};
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	SockAddr.sin_port = htons(1234);

    /* connect to server on given address */
    if (connect(clientSocket,(sockaddr*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR &&
		WSAGetLastError() != WSAEWOULDBLOCK)
    {
        std::cout<<"connect:"<<WSA::ErrToString(WSAGetLastError())<<"\n";
        closesocket(clientSocket);
        /* release completion port */
        return EXIT_FAILURE;
    }

    DataContext *clientContext = new DataContext(clientSocket);

    /* Appends reference to socket in completion port */
    IOCP_handle = CreateIoCompletionPort((HANDLE)clientContext->socket, IOCP_handle, 0, 0);
    if (!IOCP_handle)
    {
        std::cout<<"CreateIoCompletionPort:"<<WSA::ErrToString(GetLastError())<<"\n";
        closesocket(clientSocket);
        return EXIT_FAILURE;
    }

    // Initialize the WSABUF structure
    clientContext->wsabuf.buf = clientContext->buffer;
    clientContext->wsabuf.len = sizeof(clientContext->buffer);

    // Post an asynchronous read operation
    DWORD flags = 0;
    if (WSARecv(clientContext->socket, &clientContext->wsabuf, 1, &clientContext->bytesReceived, &flags, &clientContext->overlapped, NULL) == SOCKET_ERROR) 
    {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            std::cerr << "WSARecv failed: " <<WSA::ErrToString(WSAGetLastError()) << "\n";
            closesocket(clientSocket);
            /* release completion port */
            CloseHandle(IOCP_handle);
            return EXIT_FAILURE;
        }
    }


    DWORD BytesTransferred;  
    DataContext PerHandleData(clientSocket);
    // LPPER_IO_DATA PerIoData;
    LPOVERLAPPED lpOverlapped{nullptr};
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    ULONG_PTR CompletionKey{0};
    DataContext *context;

    while (1)
    {
        /*  Can be related to many threads */
        if(!GetQueuedCompletionStatus(IOCP_handle, &BytesTransferred,&CompletionKey/*(PULONG_PTR)&PerHandleData*/, &lpOverlapped /*(LPOVERLAPPED *) &PerIoData*/ , INFINITE))
        {
            std::cout<<"GetQueuedCompletionStatus:"<<WSA::ErrToString(GetLastError())<<"\n";
            closesocket(clientSocket);
            /* release completion port */
            CloseHandle(IOCP_handle);
            return EXIT_FAILURE;
        }
        context=(DataContext*)(lpOverlapped);
        std::cout<<std::string(context->buffer,BytesTransferred)<<"\n";
        int a=8;
        a=a;

        std::memset(context->buffer,0, sizeof(context->buffer));
        // Post an asynchronous read operation
        DWORD flags = 0;
        if (WSARecv(context->socket, &context->wsabuf, 1, &context->bytesReceived, &flags, &context->overlapped, NULL) == SOCKET_ERROR) 
        {
            if (WSAGetLastError() != WSA_IO_PENDING) {
                std::cerr << "WSARecv failed: " << WSA::ErrToString(WSAGetLastError()) << "\n";
                closesocket(clientSocket);
                /* release completion port */
                CloseHandle(IOCP_handle);
                return EXIT_FAILURE;
            }
        }

    }

    closesocket(clientSocket);

    /* release completion port */
    CloseHandle(IOCP_handle);

    return 0;
}