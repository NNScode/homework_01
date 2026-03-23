#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

typedef struct {
    char student_id[20];
    char full_name[100];
    char dob[20];
    float gpa;
} Student;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Sử dụng: %s <cổng> <tên file log>\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);
    char *log_file = argv[2];

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Lỗi tạo socket.\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Lỗi bind.\n");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        printf("Lỗi listen.\n");
        close(server_socket);
        return 1;
    }

    printf("Server đang lắng nghe ở cổng %d...\n", server_port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_socket < 0) {
            continue;
        }

        Student received_student;
        int bytes_received = recv(client_socket, &received_student, sizeof(Student), 0);
        
        if (bytes_received == sizeof(Student)) {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

            time_t rawtime;
            struct tm *timeinfo;
            char time_buffer[80];
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

            char log_entry[512];
            snprintf(log_entry, sizeof(log_entry), "%s %s %s %s %s %.2f\n", 
                     client_ip, time_buffer, 
                     received_student.student_id, 
                     received_student.full_name, 
                     received_student.dob, 
                     received_student.gpa);

            printf("%s", log_entry);

            FILE *file = fopen(log_file, "a");
            if (file) {
                fprintf(file, "%s", log_entry);
                fclose(file);
            } else {
                printf("Lỗi mở file log.\n");
            }
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
