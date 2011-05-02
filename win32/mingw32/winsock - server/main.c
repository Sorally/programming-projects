/* 
 * Complete Winsock Server Code
 * http://msdn.microsoft.com/en-us/library/ms738545%28v=vs.85%29.aspx
 *
 */
 

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#define WINVER 0x0501		// for getaddrinfo, freeaddrinfo

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(void) {
	
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET,
           ClientSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;
		int open_connection = 1;

	do {

    // Initialize Winsock
    memset(recvbuf, 0, DEFAULT_BUFLEN);
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
 

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    // Accept a client socket (blocking)
    ClientSocket = accept(ListenSocket, NULL, NULL);
    
	    if (ClientSocket == INVALID_SOCKET) {
	        printf("accept failed with error: %d\n", WSAGetLastError());
	        closesocket(ListenSocket);
	        WSACleanup();
	        return 1;
	    }
	
	    // No longer need server socket
	    closesocket(ListenSocket);
	
	    // Receive until the peer shuts down the connection
	    do {
	
	        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	        if (iResult > 0) {
							//printf("Bytes received: %d\n", iResult);
							if (recvbuf[0] == 'd' && recvbuf[1] == 'i' && recvbuf[2] == 'e') {
								printf("Received die command, so long old friend\n");
								open_connection = 0;
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
							}
							printf("%s", recvbuf);
							// Echo the buffer back to the sender
							iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
	            if (iSendResult == SOCKET_ERROR) {
	                printf("send failed with error: %d\n", WSAGetLastError());
	                closesocket(ClientSocket);
	                WSACleanup();
	                return 1;
	            }
	            //printf("Bytes sent: %d\n", iSendResult);
	        }
	        else if (iResult == 0) {
	            //printf("Connection closing...\n");
	        } else {
	            printf("recv failed with error: %d\n", WSAGetLastError());
	            closesocket(ClientSocket);
	            WSACleanup();
	            //return 1;
	        }
	
	    } while (iResult > 0);
		} while (open_connection);
		
    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;

}