#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <MSWSock.h>
#include "../lib/FileSockTrans.h"
//#include "../SoursePackage/test.h"






// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
const  WCHAR* DEFAULT_FILE_PATH = L"C:\\Users\\Ò¦í²ê¿\\Videos\\¼²ËÙ×·É±\\John.Wick.¼²ËÙ×·É±.2014.ÖÐÓ¢×ÖÄ».BD.1080P-ÈËÈËÓ°ÊÓ.mp4";

int BufferPoint;
int BufferScaned;
char BUFFER[BUFFERSIZE];

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1"


int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
 

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
        
    send_Meg_Sf(ConnectSocket, DEFAULT_FILE_PATH);




 
#if 0
    //createfile and transmitfile
        freeaddrinfo(result);
        HANDLE FHandle = CreateFile(DEFAULT_FILE_PATH, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
        if (FHandle == INVALID_HANDLE_VALUE) {
            printf("error at %d", __LINE__);
        }
             


        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            return 1;

        }
        auto statu = TransmitFile(ConnectSocket,FHandle, 0, 0, NULL, NULL, TF_DISCONNECT);
        CloseHandle(FHandle);
        if (statu == FALSE)
        {
            printf("error at %s,%d", __FILE__, __LINE__);
            //exit(1);
        }
#endif 
        // Send an initial buffer
#if 0
        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        printf("Bytes Sent: %ld\n", iResult);
#endif

        // shutdown the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
#if 0



        // Receive until the peer closes the connection
        do {

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0)
                printf("Bytes received: %d\n", iResult);
            else if (iResult == 0)
                printf("Connection closed\n");
            else
                printf("recv failed with error: %d\n", WSAGetLastError());

        } while (iResult > 0);
#endif // 0
        // cleanup
        closesocket(ConnectSocket);
        WSACleanup();

        return 0;
    }
