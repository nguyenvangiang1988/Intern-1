#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>

#define BSIZE 1024
#define PORT 21996

void CleanStdInput(){
    while(fgetc(stdin) != 10);
}

enum DATA_TYPE{
    NUMBER, STRING, STUDENT_INFO
};


struct Student{
    char id[10];
    char name[100];
    struct {
        unsigned int day, month;
        unsigned int year;
    } birthday;
};


int main(){
    char buff[BSIZE];
    int stop = 0;
    int type;
    int cskid = socket(AF_INET, SOCK_STREAM, 0);
    int sbytes;
    struct Student student;

    int n;

    if(cskid == -1){
        perror("Create client socket failed");
        return 0x0;
    }

    struct sockaddr_in serveraddr = {
            AF_INET,
            htons(PORT),
            htonl(0x7f000001)
    };

    if(connect(cskid, (struct sockaddr*)&serveraddr, sizeof serveraddr) == -1){
        perror("Connect to server failed");
    }else{
        printf("Connect to server success!\n");
        while(!stop){
            printf("Type to send (%d. Integer Number, %d. String, %d. Student Info): ", NUMBER, STRING, STUDENT_INFO);

            scanf("%d", &type);

            switch(type){
            case NUMBER:
                sbytes = send(cskid, &type, sizeof type, 0);
                printf("Integer Number: ");
                scanf("%d", &n);
                sbytes = send(cskid, &n, 4, 0);
                if(sbytes != 4){
                    perror("Send error");
                    printf("Closing socket...\n");
                    close(cskid);
                    return 0x0;
                }
                printf("Sent integer number\n");
                break;
            case STRING:
                sbytes = send(cskid, &type, sizeof type, 0);
                printf("String: ");
                CleanStdInput();
                fgets(buff, BSIZE, stdin);
                sbytes = send(cskid, buff, strlen(buff), 0);
                if(strlen(buff) != sbytes){
                    perror("Send error");
                    printf("Closing socket...\n");
                    close(cskid);
                    return 0x0;
                }
                printf("Sent string\n");
                break;
            case STUDENT_INFO:
                sbytes = send(cskid, &type, sizeof type, 0);
                printf("Student ID: ");
                CleanStdInput();
                fgets(student.id, sizeof(student.id) / sizeof(char), stdin);
                printf("Student name: ");
                CleanStdInput();
                fgets(student.name, sizeof(student.name) / sizeof(char), stdin);
                printf("Day of birthday: ");
                CleanStdInput();
                scanf("%u", &student.birthday.day);
                printf("Month of birthday: ");
                CleanStdInput();
                scanf("%u", &student.birthday.month);
                printf("Year of birthday: ");
                CleanStdInput();
                scanf("%u", &student.birthday.year);
                sbytes = send(cskid, &student, sizeof student, 0);
                break;
            default:
                printf("Unknown type. Closing socket...\n");
                close(cskid);
                break;
            }

            printf("Do you want to stop (0: Continue, 1: Stop): ");
            scanf("%d", &stop);
        }
    }

    close(cskid);

    return 0x0;
}
