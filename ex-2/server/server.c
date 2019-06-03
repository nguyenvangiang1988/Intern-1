#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>

#define PORT 21996
#define MAXPENDING 100
#define BSIZE 1024

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
    int skid = socket(AF_INET, SOCK_STREAM, 0);
    int cskid;
    int number;
    struct Student student;
    char buff[BSIZE];
    int rbytes;
    enum DATA_TYPE type;

    if(skid == -1){
        perror("Error initialize server socket");
    }

    struct sockaddr_in addrport = {
        AF_INET,
        htons(PORT),
        htonl(INADDR_ANY)
    };

    if(bind(skid, (struct sockaddr*)&addrport, sizeof addrport) == -1){
        perror("Bind address to socket failed");
        return 0x0;
    }else{
        printf("Bind port %d success\n", PORT);
    }

    if(listen(skid, MAXPENDING) == -1){
        perror("Listening failed");
        close(skid);
        return 0x0;
    }else{
        printf("OK listening\n");
    }

    cskid = accept(skid, 0, 0);

    if(cskid != -1){
        printf("OK client connected!\n");

        while((rbytes = recv(cskid, &type, sizeof type, 0)) > 0){
            if(rbytes == sizeof type){
                printf("type %d\n", type);

                switch(type){
                    case STRING:
                        rbytes = recv(skid, buff, BSIZE, 0);
                        if(rbytes > 0){
                            char* s = (char*)malloc(rbytes);
                            strncpy(s, buff, rbytes);
                            printf("Received string: %s", s);
                            free(s);
                        }else{
                            printf("Closing socket...\n");
                            close(cskid);
                            close(skid);
                            return 0x0;
                        }
                        break;
                    case NUMBER:
                        rbytes = recv(skid, &number, sizeof number, 0);
                        if(rbytes == sizeof number){
                            printf("Received integer number: %d", number);
                        }else{
                            printf("Closing socket...\n");
                            close(cskid);
                            close(skid);
                            return 0x0;
                        }
                        break;
                    case STUDENT_INFO:
                        rbytes = recv(skid, &student, sizeof student, 0);
                        if(rbytes == sizeof student){
                            printf("Receive Student Structure:\nID: %s\nName: %s\nBirthday: %u/%u/%u\n", student.id, student.name, student.birthday.day, student.birthday.month, student.birthday.year);
                        }else{
                            printf("Closing socket...\n");
                            close(cskid);
                            close(skid);
                            return 0x0;
                        }
                        break;
                    default:
                        printf("Default");
                        printf("Out of type! Closing socket...\n");
                        close(cskid);
                        close(skid);
                        return 0x0;
                        break;
                }
            }else{
                printf("Size of type byte incorrect!\n");
                break;
            }
        }
    }else{
        perror("accept failed");
    }


    printf("Closing...\n");

    int result = close(skid);

    if(result){
        perror("ERROR CLOSE SOCKET");
    }

    return 0x0;
}
