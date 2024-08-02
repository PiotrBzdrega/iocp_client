#include <iostream>
#include <cstring> //std::memset
#include <vector>

#include "WinSockInit.h"
#include "DataContext.h"
#include "ServInfo.h"
#include <mswsock.h> 
// #include "ServerContext.h"
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



2024.07.28
It is not possible to return WSAConnect result in GetQueuedCompletionStatus so i will try to use CONNECTEX with 0bytes

*/

int main()
{
    std::cout<<"Kitos build: "<<__DATE__<<"  " << __TIME__<<"\n";

    /* Initialize WinSock*/
    auto& winsock = WinSock::instance();

    /* IOCP singlton instance */
    // auto& iocp = IOCP::IOCP::instance(); //TODO: find best design 

    /* Creates an input/output (I/O) completion port w/o reference to socket */
    HANDLE IOCP_handle = CreateIoCompletionPort( INVALID_HANDLE_VALUE,NULL, 0, 0);
    if (!IOCP_handle)
    {
        std::cout<<"CreateIoCompletionPort: "<<WSA::ErrToString(GetLastError())<<"\n";
        return EXIT_FAILURE;
    }

    // Create a socket
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
        return EXIT_FAILURE;
    }

    // /* create socket for client */
    // SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // if (clientSocket == INVALID_SOCKET)
    // {
    //     std::cout<<"socket:"<<WSA::ErrToString(WSAGetLastError())<<"\n";
    //     closesocket(clientSocket);
    //     return EXIT_FAILURE;
    // }

    auto on = 1;
    if(setsockopt(clientSocket,SOL_SOCKET,SO_EXCLUSIVEADDRUSE,(char *)&on,sizeof(on))) /* The SO_EXCLUSIVEADDRUSE option prevents other sockets from being forcibly bound to the same address and port */
    {
        std::cout<<"setsockopt: "<<WSA::ErrToString(WSAGetLastError())<<"\n";
        closesocket(clientSocket);
        /* release completion port */
        return EXIT_FAILURE;
    }

    DataContext *clientContext = new DataContext(clientSocket);

	DWORD dwRetBytes = 0;
	GUID guid = WSAID_CONNECTEX;
    LPFN_CONNECTEX ConnectEx = nullptr;

    /* Appends reference to socket in completion port */
    IOCP_handle = CreateIoCompletionPort((HANDLE)clientContext->socket, IOCP_handle, 0, 0);
    if (!IOCP_handle)
    {
        std::cout<<"CreateIoCompletionPort: "<<WSA::ErrToString(GetLastError())<<"\n";
        closesocket(clientSocket);
        return EXIT_FAILURE;
    }

   // Load ConnectEx function
    auto ret=WSAIoctl(clientContext->socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid),&ConnectEx, sizeof(ConnectEx), &dwRetBytes, NULL, NULL);

    if (ConnectEx == nullptr || ret) 
    {
        std::cerr << "WSAIoctl failed: "<<WSA::ErrToString(WSAGetLastError())<<"\n";
        closesocket(clientContext->socket);
        return EXIT_FAILURE;
    }   

    // SOCKADDR_IN SockAddr{0};
	// SockAddr.sin_family = AF_INET;
	// SockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// SockAddr.sin_port = htons(1234);

    /* to ensure all fields are set to default values (0 or NULL for pointers) */    
    struct addrinfo hints = {0}; 
    {
        hints.ai_family = AF_INET,//AF_UNSPEC;     /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* TCP */
        hints.ai_protocol = IPPROTO_TCP; //TODO: not sure if needed for Windows , for X can be omited
        // hints.ai_flags = AI_PASSIVE; //TODO: not sure if needed for Windows , for X can be omited
    } 

    Servinfo servinfo;
    /* Translate name of a service location and/or a service name to set of socket addresses*/
    if(getaddrinfo(  "localhost",//"127.0.0.1", //"172.22.64.1",  //"localhost", /* e.g. "www.example.com" or IP */
                            "5234", /* e.g. "http" or port number  */
                            &hints, /* prepared socket address structure*/
                            &servinfo) /* pointer to sockaddr structure suitable for connecting, sending, binding to an IP/port pair*/
    )
    {
        std::cout<<"getaddrinfo:"<<WSA::ErrToString(GetLastError())<<"\n";
        closesocket(clientSocket);
        CloseHandle(IOCP_handle);
    }


    struct sockaddr_in sa;
    // Zero out the sockaddr_in structure
    std::memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;  // IPv4
    sa.sin_port = htons(1234);

    /*  ConnectEx() requires that the socket be bound to an address
	    with bind() before using, otherwise it will fail.
        connect does it automatically */
    ret = bind(clientContext->socket, (struct sockaddr*)&sa, sizeof(sa));

    sockaddr_storage addr = {0};

    for (addrinfo *p = servinfo; p != 0; p = p->ai_next)
    {
        memcpy_s(&addr, sizeof(addr), p->ai_addr, p->ai_addrlen);
        
		break;
    }

    if (!ConnectEx(clientContext->socket, (sockaddr*)&addr, sizeof(sockaddr_storage), NULL, 0, NULL, &clientContext->overlapped)) 
    {
        int err;
        if ( (err = WSAGetLastError()) != ERROR_IO_PENDING) 
        {
            std::cerr << "ConnectEx failed: "<<WSA::ErrToString(err)<<"\n";
            closesocket(clientContext->socket);
            delete clientContext;
            CloseHandle(IOCP_handle);
            return EXIT_FAILURE;
        }
    }


    // /* to ensure all fields are set to default values (0 or NULL for pointers) */    
    // struct addrinfo hints = {0}; 
    // {
    //     hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    //     hints.ai_socktype = SOCK_STREAM; /* TCP */
    //     hints.ai_protocol = IPPROTO_TCP; //TODO: not sure if needed for Windows , for X can be omited
    //     // hints.ai_flags = AI_PASSIVE; //TODO: not sure if needed for Windows , for X can be omited
    // } 

    // Servinfo servinfo;
    // /* Translate name of a service location and/or a service name to set of socket addresses*/
    // if(getaddrinfo(  "127.0.0.1", //"172.22.64.1",  //"localhost", /* e.g. "www.example.com" or IP */
    //                         "1234", /* e.g. "http" or port number  */
    //                         &hints, /* prepared socket address structure*/
    //                         &servinfo) /* pointer to sockaddr structure suitable for connecting, sending, binding to an IP/port pair*/
    // )
    // {
    //     std::cout<<"getaddrinfo:"<<WSA::ErrToString(GetLastError())<<"\n";
    //     closesocket(clientSocket);
    //     CloseHandle(IOCP_handle);
    // }

    // struct addrinfo *ptr = servinfo;

    // // Initiate a non-blocking connect operation
    // if (WSAConnect(clientContext->socket, ptr->ai_addr, (int)ptr->ai_addrlen, NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
    //     if (WSAGetLastError() != WSA_IO_PENDING) {
    //         std::cerr << "WSAConnect failed.\n";
    //         closesocket(clientContext->socket);
    //         CloseHandle(IOCP_handle);
    //         return EXIT_FAILURE;
    //     }
    // }

    // SOCKADDR_IN SockAddr{0};
	// SockAddr.sin_family = AF_INET;
	// SockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// SockAddr.sin_port = htons(1234);

    // /* connect to server on given address */
    // if (connect(clientSocket,(sockaddr*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR &&
	// 	WSAGetLastError() != WSAEWOULDBLOCK)
    // {
    //     std::cout<<"connect:"<<WSA::ErrToString(WSAGetLastError())<<"\n";
    //     closesocket(clientSocket);
    //     /* release completion port */
    //     return EXIT_FAILURE;
    // }


    // // Initialize the WSABUF structure
    // clientContext->wsabuf.buf = clientContext->buffer;
    // clientContext->wsabuf.len = sizeof(clientContext->buffer);

    // // Post an asynchronous read operation
    // DWORD flags = 0;
    // if (WSARecv(clientContext->socket, &clientContext->wsabuf, 1, &clientContext->bytesReceived, &flags, &clientContext->overlapped, NULL) == SOCKET_ERROR) 
    // {
    //     if (WSAGetLastError() != WSA_IO_PENDING) {
    //         std::cerr << "WSARecv failed: " <<WSA::ErrToString(WSAGetLastError()) << "\n";
    //         closesocket(clientSocket);
    //         /* release completion port */
    //         CloseHandle(IOCP_handle);
    //         return EXIT_FAILURE;
    //     }
    // }

    DWORD BytesTransferred{};  
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
        auto complStatus=GetQueuedCompletionStatus(IOCP_handle, &BytesTransferred,&CompletionKey/*(PULONG_PTR)&PerHandleData*/, &lpOverlapped /*(LPOVERLAPPED *) &PerIoData*/ , INFINITE);


        if(complStatus == 0 )
        {
            if (lpOverlapped == nullptr)
            {
                /* This usually indicates an error in the parameters to GetQueuedCompletionStatus. */
                std::cout<<"GetQueuedCompletionStatus: "<<WSA::ErrToString(GetLastError())<<"\n";
                closesocket(clientSocket);
                /* release completion port */
                CloseHandle(IOCP_handle);
                return EXIT_FAILURE;
            }
            else
            {
                /*The thread must deal with the data (possibly freeing any associated buffers), 
                but there is an error condition on the underlying HANDLE. 
                Usually seen when the other end of a network connection has been forcibly closed, 
                but there's still data in the send or receive queue.*/
                std::cout<<"GetQueuedCompletionStatus: "<<WSA::ErrToString(GetLastError())<<"\n";
                closesocket(clientSocket);
                /* release completion port */
                // CloseHandle(IOCP_handle); //do not stop 
                return EXIT_FAILURE;
            }
        }
        if (&clientContext->overlapped==lpOverlapped)
        {
           std::cout<<"connected\n";

        }
        
        context=(DataContext*)(lpOverlapped);
        std::cout<<std::string(context->buffer,BytesTransferred)<<"\n";



        std::memset(context->buffer,0, sizeof(context->buffer));
        // Post an asynchronous read operation
        DWORD flags = 0;
        if (WSARecv(context->socket, &context->wsabuf, 1, &context->bytesReceived, &flags, &context->overlapped, NULL) == SOCKET_ERROR) 
        {
            int err;
            if ( (err = WSAGetLastError()) != WSA_IO_PENDING) {
                std::cerr << "WSARecv failed: " << WSA::ErrToString(err) << "\n";
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