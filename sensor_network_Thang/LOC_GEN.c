/*
 ============================================================================
 Name        : c.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT_RECV 5678
#define PORT_SEND 1234
#define TIME_SEND 3
#define MAXIMUM_USER 10
#define FILE_NAME "LOC_GEN.txt"

int SOCK_SEND = 0;
int SOCK_RECV = 0;
fd_set EVEN;
fd_set NEXT;


struct THREAD_SON {
    int num;
    int totalThread;
    int disable;
    int timeRandom;
    
} ELEMENT [MAXIMUM_USER];



void InitThreadSon(){
    for (int i=0; i< MAXIMUM_USER; i++){
        ELEMENT [i].num          = 0;
        ELEMENT [i].totalThread  = 0;
        ELEMENT [i].disable      = 0;
        ELEMENT [i].timeRandom   = 0;
    }
}

int CountRoom(){
    int count= 0;
    for (int i=0; i< MAXIMUM_USER; i++){
        if (ELEMENT [i].num != 0){
            count ++;
        }
    }
    return count;
}

//================================================================
// threading send to server
void *SendMessage() {

    while (1) {
        int countSend =0;
        int temp= 0;
        for (int i=0; i< MAXIMUM_USER; i++){

            ELEMENT [i].totalThread= CountRoom();
            ELEMENT [i].timeRandom= 16 + rand() % 17;

            if (ELEMENT [i].num !=0 && ELEMENT [i].disable == 0){
                char Message[1000];  
                srand (time(NULL));
                sprintf (Message, ">> Number of room: %d\n"
                                  ">> Total room: %d\n"
                                  ">> Temperature at present: %d °C\n"
                                  "================================\n\n",
                                  ELEMENT [i].num, ELEMENT [i].totalThread, ELEMENT [i].timeRandom);     

                printf ("%s", Message);
                FILE *file= fopen (FILE_NAME, "a+");

                if (!file){
                    perror ("ERROR ");
                    exit (1);
                }
                fputs (Message, file);
                fclose (file);
                char *sendTemperture;
                sprintf (sendTemperture, "%d", ELEMENT [i].timeRandom);
                send (SOCK_SEND, sendTemperture, strlen(sendTemperture), 0);
                sleep(2);
                countSend ++;
            }
        }
        for (int i=0; i<MAXIMUM_USER; i++){
            if (ELEMENT [i].num != 0){
                temp ++;
            }
        }
        if (countSend == temp && temp != 0){
            char *notify= "All room is sended data";
            send (SOCK_SEND, notify, strlen (notify), 0);
            temp = 0;
            countSend= 0;
        }
        sleep (TIME_SEND);
    }

}

void *recvMessage(){
    while(1){
        FD_ZERO (&EVEN);
        FD_SET (SOCK_RECV, &EVEN);
        select (SOCK_RECV + 1, &EVEN, NULL, NULL, NULL);
        if (FD_ISSET (SOCK_RECV, &EVEN)){
            char Message[1000];
            memset(Message, '\0', sizeof(Message));
            recv (SOCK_RECV, Message, sizeof(Message), 0);
            puts(Message);
            if (strcmp (Message, "add")==0){
                for(int i=0; i< MAXIMUM_USER; i++){
                    if (ELEMENT [i].num == 0){
                        ELEMENT [i].num= i+1;
                        break;
                    }
                }
            }
            if ( strcmp (Message, "delete") == 0){
                select (SOCK_RECV + 1, &EVEN, NULL, NULL, NULL);
                if (FD_ISSET (SOCK_RECV, &EVEN)){
                    memset(Message, '\0', sizeof(Message));
                    recv (SOCK_RECV, Message, sizeof(Message), 0);
                    int del= strtol (Message, 0, 10);
                    ELEMENT [del-1].num= 0;

                }
            }
            if ( strcmp (Message, "disable") == 0){
                select (SOCK_RECV + 1, &EVEN, NULL, NULL, NULL);
                if (FD_ISSET (SOCK_RECV, &EVEN)){
                    memset(Message, '\0', sizeof(Message));
                    recv (SOCK_RECV, Message, sizeof(Message), 0);
                    int x= strtol (Message, 0, 10);
                    ELEMENT [x-1].disable= 1;

                }
            }
            if ( strcmp (Message, "enable") == 0){
                select (SOCK_RECV + 1, &EVEN, NULL, NULL, NULL);
                if (FD_ISSET (SOCK_RECV, &EVEN)){
                    memset(Message, '\0', sizeof(Message));
                    recv (SOCK_RECV, Message, sizeof(Message), 0);
                    int x= strtol (Message, 0, 10);
                    ELEMENT [x-1].disable= 0;

                }
            }
        }
    }
}

int main()
{
    InitThreadSon();
    struct sockaddr_in serv_addr;
    pthread_t send_thread, receive_thread;
    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_RECV);
    serv_addr.sin_addr.s_addr   = INADDR_ANY;

    if ((SOCK_RECV = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sock");
        exit(1);
    }
    int connectRecv= connect (SOCK_RECV, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if ( connectRecv < 0)
    {
       perror("connect");
       exit(1);
    }
//=======================================================port 2
    serv_addr.sin_port = htons(PORT_SEND);    
    if ((SOCK_SEND = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sock");
        exit(1);
    }
    int connectSend= connect(SOCK_SEND, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if ( connectSend < 0)
    {
       perror("connect");
       exit(1);
    }
    FD_ZERO (&NEXT);
    FD_SET (connectSend, &NEXT);

    pthread_create(&send_thread, NULL, SendMessage, NULL);
    pthread_create(&receive_thread, NULL, recvMessage, NULL);

    while(1){};
    return 0;
}
