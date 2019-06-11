/*
 * client.c
 *
 *  Created on: Jun 6, 2019
 *      Author: lhungtam
 */


#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define PORT 1234

int main()
{

    struct sockaddr_in serverAddress;

    int valueOfSocket;
    int valueOfShutdown;\
    int check=1;

    fd_set socketlist;

    memset(&serverAddress, '\0', sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    inet_aton("192.168.81.128", &serverAddress.sin_addr.s_addr);

    valueOfSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (valueOfSocket < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    FD_ZERO(&socketlist);
    FD_SET(valueOfSocket, &socketlist);

    int ValueConnect=connect(valueOfSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if ( ValueConnect < 0)
    {
       printf("\nConnection Failed \n");
       return -1;
    }
    while (check){//input nickname
        select(valueOfSocket+1, &socketlist, NULL, NULL, NULL);
        if(FD_ISSET(valueOfSocket, &socketlist)){
            char ServerReply[2000];
            memset(&ServerReply, '\0', sizeof(ServerReply));
            recv(valueOfSocket, ServerReply, sizeof(ServerReply),0);
            puts(ServerReply);
            if(strcmp("Sign up nickname successful\n", ServerReply)==0){
                check=0;
                continue;
            }
        }

        char Message[2000];
        printf("%s","> ");
        gets(Message);
        fflush(stdin);
        send(valueOfSocket, Message, strlen(Message), 0);
        memset(&Message, '\0', sizeof(Message));
    }

    check=1;
    while (check){//notification from server
        select(valueOfSocket+1, &socketlist, NULL, NULL, NULL);
        if(FD_ISSET(valueOfSocket, &socketlist)){
            char ServerReply[2000];
            memset(&ServerReply, '\0', sizeof(ServerReply));
            recv(valueOfSocket, ServerReply, sizeof(ServerReply),0);
            puts(ServerReply);
            if (strcmp(ServerReply, "We start !!\n")==0){
                check=0;
            }
        }
    }

    while (1){//answer
        printf("?????????????????\n");
        select(valueOfSocket+1, &socketlist, NULL, NULL, NULL);
        if(FD_ISSET(valueOfSocket, &socketlist)){
            char ServerReply[1024];
            memset(&ServerReply, '\0', sizeof(ServerReply));
            recv(valueOfSocket, ServerReply, sizeof(ServerReply),0);
            printf("%s",ServerReply);
	    if (strcmp(ServerReply, "Your answer is false\n")==0){
		shutdown(valueOfSocket, SHUT_RDWR);
		return -1;
	    }
	    if (strcmp(ServerReply, "Your answer is true\n")==0){
		continue;
	    }
        }
        char Message[2000];
        printf("%s","> ");
        gets(Message);
	strcat(Message, "\n");
        fflush(stdin);
        send(valueOfSocket, Message, strlen(Message), 0);
        memset(&Message, '\0', sizeof(Message));
	
	/*select(valueOfSocket+1, &socketlist, NULL, NULL, NULL);
	if(FD_ISSET(valueOfSocket, &socketlist)){
	    char ServerReply[1024];
            memset(&ServerReply, '\0', sizeof(ServerReply));
            recv(valueOfSocket, ServerReply, sizeof(ServerReply),0);
            printf("%s",ServerReply);
	    
	}*/
    }
    valueOfShutdown=shutdown(valueOfSocket, SHUT_RDWR);
    if(valueOfShutdown==0)
        printf("Shutdown successful");
    close(valueOfSocket);
    return 0;
}
