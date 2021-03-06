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
    int cskid;
    int number;
    struct Student student;
    char buff[BSIZE];
    int rbytes;
    enum DATA_TYPE type;
    int err_indicate = 0;

    int skid = socket(AF_INET, SOCK_STREAM, 0);

    if (skid == -1){
        perror("Error initialize server socket");
        return 0;
    }

    struct sockaddr_in addrport = {
        AF_INET,
        htons(PORT),
        htonl(INADDR_ANY)
    };

    if (bind(skid, (struct sockaddr*)&addrport, sizeof addrport) == -1){
        perror("Bind address to socket failed");
        close(skid);
        return 0;
    }
    else{
        printf("Bind port %d success\n", PORT);
    }

    if (listen(skid, MAXPENDING) == -1){
        perror("Listening failed");
        close(skid);
        return 0;
    }
    else{
        printf("OK listening\n");
    }

    cskid = accept(skid, 0, 0);

    if (cskid != -1){
        printf("OK client connected!\n");

        while ((rbytes = recv(cskid, &type, sizeof type, 0)) > 0){
            if (rbytes == sizeof type){
                switch (type){
                    case STRING:
                        rbytes = recv(cskid, buff, BSIZE, 0);
                        if (rbytes > 0){
                            char* s = (char*)malloc(rbytes + 1);
                            strncpy(s, buff, rbytes);
                            s[rbytes] = 0;
                            printf("Received string: %s\n", s);
                            free(s);
                        }
                        else{
                            err_indicate = 1;
                        }
                        break;
                    case NUMBER:
                        rbytes = recv(cskid, &number, sizeof number, 0);
                        if (rbytes == sizeof number){
                            printf("Received integer number: %d\n", number);
                        }
                        else{
                            err_indicate = 1;
                        }
                        break;
                    case STUDENT_INFO:
                        rbytes = recv(cskid, &student, sizeof student, 0);
                        if (rbytes == sizeof student){
                            printf("Received Student Structure:\nID: %s\nName: %s\nBirthday: %u/%u/%u\n", student.id, student.name, student.birthday.day, student.birthday.month, student.birthday.year);
                        }
                        else{
                            err_indicate = 1;
                        }
                        break;
                    default:
                        printf("Out of type! Closing socket...\n");
                        err_indicate = 1;
                        break;
                }

                if (err_indicate){
                    break;
                }
            }
            else{
                printf("Size of type byte incorrect!\n");
                break;
            }
        }

        close(cskid);
    }
    else{
        perror("accept failed");
    }


    printf("Closing socket...\n");

    close(skid);

    return 0;
}
