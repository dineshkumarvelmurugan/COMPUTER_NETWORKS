#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib gcc -o client client.c -lws2_32 gcc -o server server.c -lws2_32

#define PORT 43452
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    int n;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter the number of Fibonacci terms: ");
    scanf("%d", &n);
    sprintf(buffer, "%d", n); // Convert number to string

    // Send the number to the server
    sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Receive the Fibonacci series from the server
    int serverAddrSize = sizeof(serverAddr);
    if (recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrSize) > 0) {
        buffer[strlen(buffer)] = '\0'; // Null-terminate the received data
        printf("Fibonacci series received from server: %s\n", buffer);
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
