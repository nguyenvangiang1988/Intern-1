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

void GetStringFromStdInput(char* s, unsigned int length){
	fgets(s, length, stdin);
	*(s + strlen(s) - 1) = 0;
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

    int number;

    if (cskid == -1){
        perror("Create client socket failed");
        return 0x0;
    }

    struct sockaddr_in serveraddr = {
            AF_INET,
            htons(PORT),
            htonl(0x7f000001)
    };

    if (connect(cskid, (struct sockaddr*)&serveraddr, sizeof serveraddr) == -1){
        perror("Connect to server failed");
    }else{
        printf("Connect to server success!\n");
        while (!stop){
            printf("Type to send (%d. Integer Number, %d. String, %d. Student Info): ", NUMBER, STRING, STUDENT_INFO);

            scanf("%d", &type);

            switch (type){
				case NUMBER:
					sbytes = send(cskid, &type, sizeof type, 0);
					printf("Integer Number: ");
					scanf("%d", &number);
					sbytes = send(cskid, &number, sizeof number, 0);
					if(sbytes != sizeof number){
						perror("Send error");
						printf("Closing socket...\n");
						close(cskid);
						return 0x0;
					}
					printf("Sent integer number %d bytes\n", sbytes);
					break;
				case STRING:
					sbytes = send(cskid, &type, sizeof type, 0);
					printf("String: ");
					CleanStdInput();
					GetStringFromStdInput(buff, BSIZE);
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
					GetStringFromStdInput(student.id, sizeof(student.id) / sizeof(char));
					printf("Student name: ");
					GetStringFromStdInput(student.name, sizeof(student.name) / sizeof(char));
					printf("Day of birthday: ");
					scanf("%u", &student.birthday.day);
					printf("Month of birthday: ");
					scanf("%u", &student.birthday.month);
					printf("Year of birthday: ");
					scanf("%u", &student.birthday.year);
					sbytes = send(cskid, &student, sizeof student, 0);
					break;
				default:
					printf("Unknown type. Closing socket...\n");
					close(cskid);
					return 0x0;
					break;
            }

            printf("Do you want to stop (0: Continue, 1: Stop): ");
            scanf("%d", &stop);
        }
    }

    printf("Closing socket...\n");
    close(cskid);

    return 0x0;
}
