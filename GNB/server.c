// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // For socket functions
#include <ws2tcpip.h> // For sockaddr_in
#include <time.h>

#define TOTAL_FRAMES 10
#define SERVER_PORT 8080
#define ACK_LOSS_PROBABILITY 0.3 // Probability of ACK loss

void gbn_server() {
    WSADATA wsaData;
    int sock;
    struct sockaddr_in server_address, client_address;
    int addr_len = sizeof(client_address);
    char frame; // Changed to char to match expected type

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    server_address.sin_port = htons(SERVER_PORT);
    
    // Bind the socket
    bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));

    int next_frame = 0;

    while (next_frame < TOTAL_FRAMES) {
        // Receive frame
        recvfrom(sock, &frame, sizeof(frame), 0,
                 (struct sockaddr *)&client_address, &addr_len);
        printf("Server: Receiving frame %d\n", frame);
        
        // Simulate ACK loss
        if ((rand() / (double)RAND_MAX) > ACK_LOSS_PROBABILITY) {
            printf("Server: Acknowledgment sent for frame %d\n", frame);
            // Send acknowledgment back to client
            sendto(sock, &frame, sizeof(frame), 0,
                   (struct sockaddr *)&client_address, addr_len);
            next_frame++;
        } else {
            printf("Server: Acknowledgment lost for frame %d\n", frame);
        }
    }

    closesocket(sock); // Close the socket
    WSACleanup(); // Cleanup Winsock
}

int main() {
    srand(time(0)); // Seed the random number generator
    gbn_server(); // Start the server simulation
    return 0;
}