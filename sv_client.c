#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct {
    char student_id[20];
    char full_name[100];
    char dob[20];
    float gpa;
} Student;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Sử dụng: %s <địa chỉ IP> <cổng>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Lỗi tạo socket.\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Địa chỉ IP không hợp lệ!\n");
        close(client_socket);
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Kết nối đến server thất bại!\n");
        close(client_socket);
        return 1;
    }

    Student current_student;

    printf("Nhập mã số sinh viên: ");
    scanf("%19s", current_student.student_id);
    
    while (getchar() != '\n');

    printf("Nhập họ và tên (không dấu): ");
    fgets(current_student.full_name, sizeof(current_student.full_name), stdin);
    current_student.full_name[strcspn(current_student.full_name, "\n")] = 0;

    printf("Nhập ngày sinh (yyyy-mm-dd): ");
    scanf("%19s", current_student.dob);

    printf("Nhập điểm trung bình: ");
    scanf("%f", &current_student.gpa);

    if (send(client_socket, &current_student, sizeof(Student), 0) < 0) {
        printf("Gửi dữ liệu thất bại!\n");
    } else {
        printf("Đã gửi thông tin sinh viên thành công!\n");
    }

    close(client_socket);
    return 0;
}
