#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")  

#define PORT 8080
#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10
#define TIMEOUT 3

typedef struct {
    int frame_no;
    bool ack;
} Frame;

void send_frame(SOCKET sockfd, Frame *frame) {
    printf("Client: Sending Frame %d\n", frame->frame_no);
    send(sockfd, (char*)&frame->frame_no, sizeof(frame->frame_no), 0);
}

bool receive_ack(SOCKET sockfd, int *ack_no) {
    int recv_status = recv(sockfd, (char*)ack_no, sizeof(*ack_no), 0);
    return recv_status > 0;
}

void selective_repeat_sender(SOCKET sockfd) {
    Frame window[WINDOW_SIZE];
    int next_frame_to_send = 0;
    int last_acknowledged = -1;
    int frames_sent = 0;
    int ack_no;

    while (frames_sent < TOTAL_FRAMES) {
        for (int i = 0; i < WINDOW_SIZE && next_frame_to_send < TOTAL_FRAMES; i++) {
            if (window[i].ack || window[i].frame_no > last_acknowledged) {
                window[i].frame_no = next_frame_to_send;
                window[i].ack = false;
                send_frame(sockfd, &window[i]);
                next_frame_to_send++;
            }
        }

        for (int i = 0; i < WINDOW_SIZE; i++) {
            if (receive_ack(sockfd, &ack_no)) {
                printf("Client: Received ACK for Frame %d\n", ack_no);
                for (int j = 0; j < WINDOW_SIZE; j++) {
                    if (window[j].frame_no == ack_no) {
                        window[j].ack = true;
                        last_acknowledged = ack_no;
                        frames_sent++;
                        break;
                    }
                }
            } else {
                printf("Client: No ACK for Frame %d, retransmitting...\n", window[i].frame_no);
                send_frame(sockfd, &window[i]);
            }
        }

        printf("Client: Sliding window - Next frame to send: %d\n", next_frame_to_send);
        Sleep(TIMEOUT * 1000); 
    }

    printf("Client: All frames sent and acknowledged!\n");
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    selective_repeat_sender(sockfd);

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
