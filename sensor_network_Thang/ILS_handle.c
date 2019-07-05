#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT_USER 2000
#define PORT_SEND 5678
#define MAXIMUM_USER 10

char buffer[100];
struct sockaddr_in serv_addr;
socklen_t addrlen;
int SOCK_USER;
fd_set LIST_USER;
fd_set LIST_REQUEST;

struct REQUEST {
    int addRoom;
    int deleteRoom;
    int enable;
} ELEMENT [MAXIMUM_USER];



void *ConnectUser(){
    int acceptUser;
    while(1){
        select (SOCK_USER +1, &LIST_USER, NULL, NULL, NULL);
        if (FD_ISSET (SOCK_USER, &LIST_USER)){
            addrlen= sizeof(struct sockaddr_in);
            acceptUser= accept (SOCK_USER, (struct sockaddr*)&serv_addr, &addrlen);
            FD_SET (acceptUser, &LIST_REQUEST); 
            if (acceptUser< 0){ 
                perror("ACCEPT 1: ");
                exit(1);
            }  
            printf("there is a new connect\n");
        }

        memset (buffer, '\0', sizeof (buffer));
        select (acceptUser + 1, &LIST_REQUEST, NULL, NULL, NULL);
        while(strcmp (buffer, "y")){
            if (FD_ISSET (acceptUser, &LIST_REQUEST)){

                memset (buffer, '\0', sizeof (buffer));
                recv (acceptUser, buffer, sizeof (buffer), 0);
                puts (buffer);
            }
        }
    }
}


int main(int argc, char *argv){

    pthread_t threadUser;
    memset (&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = INADDR_ANY;
    serv_addr.sin_port          = htons (PORT_USER);

    SOCK_USER= socket (AF_INET, SOCK_STREAM, 0);   
    if (SOCK_USER < 0){
        perror("socket accept: ");
        exit(1);
    }   
    if (bind (SOCK_USER, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind accept :");
        exit(1);
    }
    
    if (listen (SOCK_USER, 2) <0){
        perror("listen accept: ");
        exit(1);
    }
//================================================================================

    FD_ZERO (&LIST_USER);
    FD_ZERO (&LIST_REQUEST);
    FD_SET (SOCK_USER, &LIST_USER);
    pthread_create (&threadUser, NULL, ConnectUser, NULL);

    while(1);
    return 0;
}

