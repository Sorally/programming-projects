/* 
 * For complete Winsock Server Code:
 * http://msdn.microsoft.com/en-us/library/ms738545%28v=vs.85%29.aspx
 *
 */

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0501	// for getaddrinfo, freeaddrinfo
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "hidtool.h"

// Need to link with Ws2_32.lib
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")


/******************************************************************************
 * msgbox()
 *
 * threaded procedure
 *****************************************************************************/
DWORD WINAPI msgbox(LPVOID iValue) {
	MessageBox(0, iValue, __FILE__, MB_OK);
	return 0;
}

/******************************************************************************
 * main()
 *
 *****************************************************************************/
int main(int argc, char* argv[]) {
	
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, hints;
	char recvbuf[DEFAULT_BUFLEN];
	char retnbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int open_connection = 1;
	SYSTEMTIME lt;
	int server_started = 0;

start:

	// Initialize Winsock
	memset(recvbuf, 0, DEFAULT_BUFLEN);
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	if (!server_started) {
		// Get local IP address
		char szHostName[DEFAULT_BUFLEN];
		struct hostent *host_entry;
		char * szLocalIP;

		gethostname(szHostName, DEFAULT_BUFLEN);
		host_entry = gethostbyname(szHostName);
		szLocalIP = inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);

		// print welcome message
		GetLocalTime(&lt);
		printf("[%02d.%02d.%02d %02d:%02d:%02d] server starting on %s:%s\n", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, szLocalIP, DEFAULT_PORT);
		server_started = 1;
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
		printf("socket failed with error: %ld\n", (long) WSAGetLastError());
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

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult <= 0) {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		//return 1;
		goto start;
	}

	GetLocalTime(&lt);
	memset(retnbuf, 0, DEFAULT_BUFLEN);
	printf("[%02d.%02d.%02d %02d:%02d:%02d] ", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);

	//printf("Bytes received: %d\n", iResult);

	if (recvbuf[0] == 'd' && recvbuf[1] == 'i' && recvbuf[2] == 'e') {
		Beep(900, 100);
		Beep(800, 100);
		Beep(700, 100);
		Beep(600, 100);
		Beep(500, 100);
		Beep(400, 100);

		printf("Received die command, so long old friend\n");
		open_connection = 0;
		goto cleanup;
	}

	if (strcmp(recvbuf, "temperature\n") == 0) {
		//int i = 1;
		//HANDLE hThread1 = CreateThread(NULL, 0, msgbox, &recvbuf, 0, 0);
		int current_temp;

		printf(recvbuf);
		char *args[3];
		args[0] = 0;
		args[1] = "read";
		args[2] = "2";
		current_temp = interface(3, args);
		//if (current_temp > 0) {
		itoa(current_temp, retnbuf, 10);
		//}
		//printf(">%s<", retnbuf); 
	} else if (strcmp(recvbuf, "airconON\n") == 0) {
		printf(recvbuf);
		char *args[3];
		args[0] = 0;
		args[1] = "write";
		args[2] = "3";
		interface(3, args);
	} else if (strcmp(recvbuf, "airconOFF\n") == 0) {
		printf(recvbuf);
		char *args[3];
		args[0] = 0;
		args[1] = "write";
		args[2] = "4";
		interface(3, args);	
	} else if (strcmp(recvbuf, "toggle light\n") == 0) {
		printf(recvbuf);
		char *args[3];
		args[0] = 0;
		args[1] = "write";
		args[2] = "5";
		interface(3, args);	
	} else if (strcmp(recvbuf, "switch light\n") == 0) {
		printf(recvbuf);
		char *args[3];
		args[0] = 0;
		args[1] = "write";
		args[2] = "6";
		interface(3, args);	
	} else {
		Beep(200, 600);
		printf("unimplemented:%s", recvbuf);
	}

	// Echo the buffer back to the sender
	iSendResult = send(ClientSocket, retnbuf, strlen(retnbuf), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		goto cleanup;
	}

	closesocket(ClientSocket);
	WSACleanup();
	//printf("Bytes sent: %d\n", iSendResult);

	goto start;

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		goto cleanup;
	}

cleanup:
	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}