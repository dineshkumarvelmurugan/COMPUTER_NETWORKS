#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // For Windows socket programming
#include <windows.h> // For Sleep function   gcc -o client client.c -lws2_32


#define PORT 8080
#define TOTAL_FRAMES 10
#define WINDOW_SIZE 4

void send_frames(SOCKET sock, struct sockaddr_in server_addr) {
    int base = 0; // The lowest frame number in the window
    int next_frame = 0; // The next frame to send
    int ack;

    while (base < TOTAL_FRAMES) {
        // Send frames in the window
        while (next_frame < base + WINDOW_SIZE && next_frame < TOTAL_FRAMES) {
            printf("Sending frame %d\n", next_frame);
            sendto(sock, (char*)&next_frame, sizeof(next_frame), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            next_frame++;
        }

        // Wait for acknowledgment
        recvfrom(sock, (char*)&ack, sizeof(ack), 0, NULL, NULL);
        printf("Acknowledgment received for frame %d\n", ack);

        // Update the base for the next window
        if (ack >= base) {
            base = ack + 1; // Move the base up to the next unacknowledged frame
        }
        
        // Simulate waiting for a short time before sending the next frames
        Sleep(500);
    }
    printf("All frames sent successfully.\n");
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    // Create a socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    // Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost

    // Send frames to the server
    send_frames(sock, server_addr);

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
