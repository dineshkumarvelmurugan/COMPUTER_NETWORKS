#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define PORT 43451
#define BUFFER_SIZE 1024

void chatWithServer(SOCKET serverSocket);

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.137.180");

    // Connect to server
    if (connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connect failed. Error Code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    // Chat with server
    chatWithServer(serverSocket);

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

void chatWithServer(SOCKET serverSocket) {
    char buffer[BUFFER_SIZE];
    int n, result;

    printf("Enter a number to calculate its factorial: ");
    scanf("%d", &n);
    sprintf(buffer, "%d", n); // Prepare the message to send
    send(serverSocket, buffer, strlen(buffer), 0); // Send the number to the server

    // Receive the result from the server
    if (recv(serverSocket, buffer, BUFFER_SIZE - 1, 0) > 0) {
        buffer[strlen(buffer)] = '\0'; // Null-terminate the received data
        sscanf(buffer, "%d", &result); // Parse the result
        printf("Factorial of %d received from server: %d\n", n, result);
    }
}
