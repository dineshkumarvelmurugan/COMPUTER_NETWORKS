#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib") 

#define PORT 8080
#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10

typedef struct {
    int frame_no;
    bool received;
} Frame;

bool is_frame_received() {
    return rand() % 2; 
}

void selective_repeat_receiver(SOCKET new_socket) {
    Frame window[WINDOW_SIZE] = {0};
    int next_expected_frame = 0;
    int frame_no;

    while (next_expected_frame < TOTAL_FRAMES) {
        if (recv(new_socket, (char*)&frame_no, sizeof(frame_no), 0) > 0) {
            printf("Server: Received Frame %d\n", frame_no);

            if (is_frame_received()) {
                printf("Server: Frame %d received correctly.\n", frame_no);
                send(new_socket, (char*)&frame_no, sizeof(frame_no), 0);  
            } else {
                printf("Server: Frame %d corrupted or lost.\n", frame_no);
            }
        }
        Sleep(1000);  
    }

    printf("Server: All frames received!\n");
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    srand(time(0));

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server: Waiting for a connection...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server: Client connected.\n");

    selective_repeat_receiver(new_socket);

    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
