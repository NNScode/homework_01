#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Sai cú pháp! Hướng dẫn sử dụng: %s <địa chỉ IP> <cổng>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Không thể tạo socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Địa chỉ IP không hợp lệ hoặc không được hỗ trợ\n");
        close(client_socket);
        return 1;
    }

    printf("Đang kết nối đến server %s:%d...\n", server_ip, server_port);
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Kết nối đến server thất bại");
        close(client_socket);
        return 1;
    }

    char server_reply[2048] = {0};
    int bytes_received = recv(client_socket, server_reply, sizeof(server_reply) - 1, 0);
    if (bytes_received > 0) {
        printf("\n[Server gửi]: %s\n", server_reply);
    }

    printf("Kết nối thành công! Nhập dữ liệu để gửi đi (gõ 'exit' để thoát):\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("> ");

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) {
            printf("Đóng kết nối.\n");
            break;
        }

        strcat(buffer, "\n");

        int bytes_sent = send(client_socket, buffer, strlen(buffer), 0);
        if (bytes_sent < 0) {
            perror("Không thể gửi dữ liệu");
            break;
        }
    }

    close(client_socket);
    return 0;
}
