#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Sử dụng: %s <cổng> <tệp tin chứa câu chào> <tệp tin lưu nội dung client gửi đến>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *greeting_file = argv[2];
    char *save_file = argv[3];

    FILE *f_greeting = fopen(greeting_file, "r");
    if (!f_greeting) {
        printf("Không thể mở tệp tin lời chào.\n");
        return 1;
    }
    
    char greeting[2048] = {0};
    fread(greeting, 1, sizeof(greeting) - 1, f_greeting);
    fclose(f_greeting);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("Lỗi tạo socket.\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Lỗi bind.\n");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        printf("Lỗi listen.\n");
        return 1;
    }

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            continue;
        }

        send(client_fd, greeting, strlen(greeting), 0);

        FILE *f_save = fopen(save_file, "a");
        if (f_save) {
            char buffer[1024];
            int bytes_received;
            while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
                fwrite(buffer, 1, bytes_received, f_save);
                fflush(f_save);
            }
            fclose(f_save);
        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
