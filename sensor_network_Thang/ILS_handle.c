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

char buffer[100];
struct sockaddr_in serv_addr, other;
socklen_t addrlen;
int SOCK_USER;
int SOCK_SEND;
int acceptSend;
int START_SEND= 0;
fd_set LIST_USER;
fd_set LIST_REQUEST;

struct REQUEST {
    int addRoom;
    int delete;
    int enable;
} ELEMENT;

void InitConfig(){
    ELEMENT.addRoom    = 0;
    ELEMENT.delete     = 0;
    ELEMENT.enable     = 0;
    
}

void *ConnectUser(){
    int acceptUser;

    while(1){

        FD_ZERO (&LIST_USER);
        FD_SET (SOCK_USER, &LIST_USER);
        select (SOCK_USER +1, &LIST_USER, NULL, NULL, NULL);
        if (FD_ISSET (SOCK_USER, &LIST_USER)){

            addrlen= sizeof(struct sockaddr_in);
            acceptUser= accept (SOCK_USER, (struct sockaddr*)&serv_addr, &addrlen);
            FD_SET (acceptUser, &LIST_USER); 
            if (acceptUser< 0){ 

                perror("ACCEPT 1 ");
                exit(1);
            }  
            printf(">> there is a USER connect\n");
        }
        memset (buffer, '\0', sizeof (buffer));
        while(1){

            select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
            if (FD_ISSET (acceptUser, &LIST_USER)){

                memset (buffer, '\0', sizeof (buffer));
                recv (acceptUser, buffer, sizeof (buffer), 0);
                puts (buffer);
                if (strcmp (buffer, "n")== 0){

                    printf("exit\n");
                    START_SEND= 1;
                    break;
                }
                if (strcmp (buffer, "add") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.addRoom = strtol (buffer, 0, 10);
                        ELEMENT.delete= 0;
                    } 
                }
                if (strcmp (buffer, "delete") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.delete = strtol (buffer, 0, 10);
                        ELEMENT.addRoom= 0;
                    } 
                }
                if (strcmp (buffer, "enable") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.enable = strtol (buffer, 0, 10);
                    } 
                }

            }
        }

    }
}

void *SendData(){
    addrlen= sizeof(struct sockaddr_in);
    int acceptSend;    
    while(1){

        FD_ZERO (&LIST_REQUEST);
        FD_SET (SOCK_SEND, &LIST_REQUEST);
        if (FD_ISSET (SOCK_SEND, &LIST_REQUEST)){      

            acceptSend= accept (SOCK_SEND, (struct sockaddr*)&other, &addrlen);
            if (acceptSend < 0 ){
                perror ("ACCEPT SEND ");
                exit (1);
            }

            else{
                printf (">> Thread program is connected\n");
            }

        }
        while(1){

            if (START_SEND == 1){

                printf ("start send\n");
                if (ELEMENT.addRoom == 1){

                    char *addRequest= "add";
                    send (acceptSend, addRequest, strlen (addRequest), 0);
                    ELEMENT.addRoom = 0; 
                }
                if (ELEMENT.delete == 1){
                    
                    char *delRequest= "delete";
                    send (acceptSend, delRequest, strlen (delRequest), 0);
                    ELEMENT.delete = 0; 
                }
                if (ELEMENT.enable == 1){
                    
                    char *enableRequest= "enable";
                    send (acceptSend, enableRequest, strlen (enableRequest), 0);
                    ELEMENT.enable = 0; 
                }
                perror ("send");
                START_SEND= 0;
            }
        }
    }
}

int main(int argc, char *argv){

    InitConfig();
    pthread_t threadUser, threadSend;
    memset (&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = INADDR_ANY;

    serv_addr.sin_port          = htons (PORT_USER);
    SOCK_USER= socket (AF_INET, SOCK_STREAM, 0);   
    if (SOCK_USER < 0){
        perror("socket accept ");
        exit(1);
    }   
    if (bind (SOCK_USER, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind accept ");
        exit(1);
    }
///=====================================port send
    serv_addr.sin_port          = htons (PORT_SEND);
    SOCK_SEND= socket (AF_INET, SOCK_STREAM, 0);   
    if (SOCK_SEND < 0){
        perror("socket accept ");
        exit(1);
    }   
    if (bind (SOCK_SEND, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind accept ");
        exit(1);
    }

    if (listen (SOCK_USER, 2) <0){
        perror("listen accept ");
        exit(1);
    }
    
    if (listen (SOCK_SEND, 2) <0){
        perror("listen accept ");
        exit(1);
    }



//================================================================================

    pthread_create (&threadUser, NULL, ConnectUser, NULL);
    pthread_create (&threadSend, NULL, SendData, NULL);

    while(1);


    return 0;
}
