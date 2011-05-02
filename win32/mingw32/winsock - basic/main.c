#include <stdio.h>
#include "winsock2.h"
#include <windows.h>
//#pragma comment(lib, "wininet.lib")


int main() {

  //----------------------
  // Initialize Winsock
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != NO_ERROR)
    printf("Error at WSAStartup()\n");

  //----------------------
  // Create a SOCKET for listening for
  // incoming connection requests.
  SOCKET ListenSocket;
  ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (ListenSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    WSACleanup();
    return 0;
  }

  //----------------------
  // The sockaddr_in structure specifies the address family,
  // IP address, and port for the socket that is being bound.
  struct sockaddr_in service;
  service.sin_family = AF_INET;
  service.sin_addr.s_addr = inet_addr("127.0.0.1");
  service.sin_port = htons(27015);

  if (bind( ListenSocket, 
    (SOCKADDR*) &service, 
    sizeof(service)) == SOCKET_ERROR) {
    printf("bind() failed.\n");
    closesocket(ListenSocket);
    return 0;
  }

  //----------------------
  // Listen for incoming connection requests 
  // on the created socket
  if (listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR)
    printf("Error listening on socket.\n");

  printf("Listening on socket...\n");
  WSACleanup();
  return 0;
}