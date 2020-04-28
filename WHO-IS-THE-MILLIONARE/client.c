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

#define PORT 5000
int sock = 0;
int valueOfShutdown;


//================================================================
// threading send to server
void *SendMessage() {
    while (1) {
        char Message[2000];
        printf("%s","> ");
        gets(Message);
        fflush(stdin);
        send(sock, Message, strlen(Message), 0);
        if(strcmp(Message,"EXIT")==0)
            break;
        memset(&Message, '\0', sizeof(Message));
    }

}

//================================================================
// threading recived from server
void *ReceiveMessage() {
    while (1) {
        char ServerReply[2000];
        recv(sock, ServerReply, sizeof(ServerReply),0);
        puts(ServerReply);
        if(strcmp(ServerReply,"Wrong! You are fail. Good bye!")==0 || strcmp(ServerReply,"You win! CONGRATULATE")==0)
            exit(0);
        memset(&ServerReply, '\0', sizeof(ServerReply));
    }
}



//===========================================================
int main()
{

    struct sockaddr_in serv_addr;
    pthread_t send_thread, receive_thread;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("192.168.81.12", &serv_addr.sin_addr.s_addr);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        close(sock);
        return -1;
    }
    int ValueConnect=connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if ( ValueConnect < 0)
    {
       printf("\nConnection Failed \n");
       close(sock);
       return -1;
    }
    int PthreadSend=  pthread_create(&send_thread, NULL, SendMessage, NULL);
    if( PthreadSend<0){
	perror("Pthread send");
	return -1;
    }
    int PthreadRecv= pthread_create(&receive_thread, NULL, ReceiveMessage, NULL);
    if(PthreadRecv<0){
	perror("Pthread Recieve");
	return -1;
    }
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);
    valueOfShutdown=shutdown(sock, SHUT_RDWR);
    if(valueOfShutdown==0)
        printf("Shutdown successful");
    else
        close(sock);
    close(ValueConnect);
    close(sock);
    return 0;
}
