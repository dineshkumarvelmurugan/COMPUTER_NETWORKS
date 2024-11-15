#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib gcc -o client client.c -lws2_32 gcc -o server server.c -lws2_32

#define PORT 43452
#define BUFFER_SIZE 1024

void fibonacci(int n, char *result);

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    int num;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr =INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for incoming data...\n");

    while (1) {
        // Receive number from client
        if (recvfrom(serverSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&clientAddr, &clientAddrSize) > 0) {
            num = atoi(buffer); // Convert received data to integer

            char result[BUFFER_SIZE];
            fibonacci(num, result); // Generate Fibonacci series

            // Send Fibonacci series back to the client
            sendto(serverSocket, result, strlen(result), 0, (struct sockaddr *)&clientAddr, clientAddrSize);
        }
    }

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

void fibonacci(int n, char *result) {
    int a = 0, b = 1, next;
    char temp[BUFFER_SIZE];

    if (n <= 0) {
        strcpy(result, "Invalid input.");
        return;
    }

    // Generate Fibonacci series
    sprintf(result, "%d", a); // Add first number to result
    for (int i = 1; i < n; ++i) {
        sprintf(temp, ", %d", b); // Append next number to result
        strcat(result, temp);
        next = a + b;
        a = b;
        b = next;
    }
}
