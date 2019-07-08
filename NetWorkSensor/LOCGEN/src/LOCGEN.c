/*
 ============================================================================
 Name        : LOCGEN.c
 Author      : lhungtam
 Version     :
 Copyright   : Your copyright notice
 Description : LOCGEN in C, Ansi-style
 ============================================================================
 */

#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "StructThreadRoom.h"
#include "StructThreadCLI.h"
#include "StructInfor.h"

int main(int argc, char *argv[]) {
	int socketRecieve;
	int socketSend;
	struct sockaddr_in addressRecieve;
	struct sockaddr_in addressSend;
    fd_set setSocket;

    pthread_t p_send;

    char *buffer;
    buffer=(char *)malloc(sizeof(char)*100);

    struct Infor *infor;
    infor=(struct Infor*)malloc(sizeof(struct Infor));

    struct ThreadCLI threadCLI;
    initThreadCLI(&threadCLI);

	socketRecieve=socket(AF_INET, SOCK_STREAM, 0);
	if (socketRecieve < 0){
		perror("socketRecieve");
		return 0;
	}
	socketSend=socket(AF_INET, SOCK_STREAM, 0);
	if (socketSend < 0){
	    perror("socketSend");
	    return 0;
	}

	addressRecieve.sin_family=AF_INET;
	addressRecieve.sin_port=htons(3502);
	inet_aton("192.168.81.136", &addressRecieve.sin_addr.s_addr);

	addressSend.sin_family=AF_INET;
	addressSend.sin_port=htons(3500);
	inet_aton("192.168.81.136", &addressSend.sin_addr.s_addr);

    infor->addressRecieve=&addressRecieve;
    infor->socketRecieve=&socketRecieve;
    infor->addressSend=&addressSend;
    infor->socketSend=&socketSend;
    infor->threadCLI=&threadCLI;

	connect(socketRecieve, (struct sockaddr*)&addressRecieve, sizeof(addressRecieve));
	printf("connect success\n");
	pthread_create(&p_send, NULL, (void*)&sendData, (void*)infor);
	perror("create pthread..\n");
    while (1){
        FD_ZERO(&setSocket);
        FD_SET(socketRecieve, &setSocket);
        printf("wait...\n");
        select(socketRecieve+1, &setSocket, NULL, NULL, NULL);
        if (FD_ISSET(socketRecieve, &setSocket)){
            int recvVal=recv(socketRecieve, buffer, 100, 0);
            if (recvVal<=0){
                continue;
            }
            else{
                puts(buffer);
                char *cmd, *arg;
                cmd=(char*)malloc(sizeof(char)*20);
                arg=(char*)malloc(sizeof(char)*20);
                strcpy(cmd, strtok(buffer, " "));
                strcpy(arg, strtok(NULL, "\0"));
                int i_cmd=atoi(cmd);
                int i_arg=atoi(arg);
                switch (i_cmd){
                    case 1:
                        add(infor);
                        writeFileConfig(&threadCLI);
                        break;
                    case 2:
                        delete(&threadCLI, i_arg);
                        writeFileConfig(&threadCLI);
                        break;
                    case 3:
                        stop(threadCLI, i_arg);
                        writeFileConfig(&threadCLI);
                        break;
                    case 4:
                        start(threadCLI, i_arg);
                        writeFileConfig(&threadCLI);
                        break;
                    case 5:
                        setTimeSend(&threadCLI, i_arg);
                        writeFileConfig(&threadCLI);
                        break;
                }
            }
        }
    }
	return EXIT_SUCCESS;
}
