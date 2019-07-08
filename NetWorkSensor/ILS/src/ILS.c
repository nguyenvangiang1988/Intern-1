/*
 ============================================================================
 Name        : ILS.c
 Author      : lhungtam
 Version     :
 Copyright   : Your copyright notice
 Description : ILS in C, Ansi-style
 ============================================================================
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "QueueRoom.h"

#define CMD_SIZE 5

struct Queue *queue;
char *copyCMD;

void RecieveData(){
    struct sockaddr_in addressRecieve;
    int socketRecieve;
    int bindValueRecieve;
    int listenValueRecieve;
    int acceptValueRecieve;

    socketRecieve=socket(AF_INET, SOCK_STREAM, 0);
    if (socketRecieve < 0){
        perror("socketRecieve: ");
        return;
    }
    printf("Create succes\n");

    addressRecieve.sin_family=AF_INET;
    addressRecieve.sin_port=htons(3500);
    inet_aton("192.168.81.136", &addressRecieve.sin_addr.s_addr);

    bindValueRecieve=bind(socketRecieve, (struct sockaddr*)&addressRecieve, sizeof(addressRecieve));
    if (bindValueRecieve < 0){
        perror("bindRecieve");
        exit(socketRecieve);
        return;
    }
    printf("bind succsess\n");

    listenValueRecieve=listen(socketRecieve, 10);
    if (listenValueRecieve < 0){
        perror("listenRecieve");
        exit(socketRecieve);
        return;
    }
    printf("listenpthread...\n");

    acceptValueRecieve=accept(socketRecieve, NULL, NULL);
    if (acceptValueRecieve < 0){
        perror("acceptRecieve");
        exit(socketRecieve);
        return;
    }
    printf("Accept Successfully\n");

    while (1){
    	struct ThreadRoom *room;
    	room=(struct ThreadRoom*)malloc(sizeof(struct ThreadRoom));
        recv(acceptValueRecieve, room, sizeof(struct ThreadRoom), 0);
        enqueue(queue, room);
    }
}

void WriteLog(float degreeAverage){
	FILE *f;
	f=fopen("./writeLog", "a+");
	time_t now = time(0);
	char *timeLocal=ctime(&now);
	fprintf(f, "\n");
	fprintf(f, "===================================\n");
	fprintf(f, "time: %s", timeLocal);
	if (strlen(copyCMD)!=0){
		fprintf(f, "Command: %s\n", copyCMD);
		memset(copyCMD, '\0', sizeof(copyCMD));
	}
	fprintf(f, "Average Degree: %f\n", degreeAverage);
	fprintf(f, "===================================\n");
	fprintf(f, "\n");
	fclose(f);
}

void GetCoord(){
    int count;
    float average;
    while (1){
        average=0;
        count=0;
        while (1){
            while(isempty(*queue));
            struct ThreadRoom *room;
            room=delqueue(queue);
            if (room->nameRooom != -1){
                average = average + room->degree;
                count++;
                free(room);
            }
            else{
                if (count!=0){
                    average = average/count;
                    WriteLog(average);
                    break;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in addressSend;
    int socketSend;
    int bindValueSend;
    int listenValueSend;
    int acceptValueSend;

    struct sockaddr_in addressClient;
    int socketClient;
    int bindValueClient;
    int listenValueClient;
    int acceptValueClient;

    char *cmdSend, *cmd;
    cmdSend=(char*)malloc(sizeof(char)*100);
    cmd=(char*)malloc(sizeof(char)*100);
    copyCMD=(char*)malloc(sizeof(char)*100);

    fd_set setSockClient;

    char *setCommand[]={"add", "del", "une", "enb", "set_t"};

    pthread_t p_send;
    pthread_create(&p_send, NULL, (void*)&RecieveData, NULL);

    pthread_t p_get_coord;
    pthread_create(&p_get_coord, NULL, (void*)&GetCoord, NULL);

    queue=(struct Queue*)malloc(sizeof(struct Queue));
    init(queue);

    socketSend=socket(AF_INET, SOCK_STREAM, 0);
    if (socketSend < 0){
        perror("socketSend: ");
        return 0;
    }

    socketClient=socket(AF_INET, SOCK_STREAM, 0);
    if (socketClient < 0){
        perror("socketClient: ");
        return 0;
    }

    addressSend.sin_family=AF_INET;
    addressSend.sin_port=htons(3502);
    inet_aton("192.168.81.136", &addressSend.sin_addr.s_addr);

    addressClient.sin_family=AF_INET;
    addressClient.sin_port=htons(3501);
    inet_aton("192.168.81.136", &addressClient.sin_addr.s_addr);

    bindValueSend=bind(socketSend, (struct sockaddr*)&addressSend, sizeof(addressSend));
    if (bindValueSend < 0){
        perror("bindSend");
        exit(socketSend);
        exit(socketClient);
        return 0;
    }

    bindValueClient=bind(socketClient, (struct sockaddr*)&addressClient, sizeof(addressClient));
        if (bindValueClient < 0){
            perror("bindClient");
            exit(socketSend);
            exit(socketClient);
            return 0;
        }

    listenValueSend=listen(socketSend, 10);
    if (listenValueSend < 0){
        perror("listenSend");
        exit(socketSend);
        exit(socketClient);
        return 0;
    }

    listenValueClient=listen(socketClient, 10);
    if (listenValueClient < 0){
        perror("listenClient");
        exit(socketSend);
        exit(socketClient);
        return 0;
    }
    printf("listening...\n");

    acceptValueSend=accept(socketSend, NULL, NULL);
    if (acceptValueSend < 0){
        perror("acceptSend");
        exit(socketSend);
        exit(socketClient);
        return 0;
    }

    acceptValueClient=accept(socketClient, NULL, NULL);
    if (acceptValueClient < 0){
        perror("acceptClient");
        exit(socketSend);
        exit(socketClient);
        return 0;
    }
    printf("accept success\n");

    int size_FD;
    if (acceptValueClient < socketClient){
        size_FD=socketClient;
    }
    else{
        size_FD=acceptValueClient;
    }

    char *command, *argument;
    command=(char*)malloc(sizeof(char)*20);
    argument=(char*)malloc(sizeof(char)*80);
    while (1){
        FD_ZERO(&setSockClient);
        FD_SET(socketClient, &setSockClient);
        FD_SET(acceptValueClient, &setSockClient);
        select(size_FD+1, &setSockClient, NULL, NULL, NULL);
        if (FD_ISSET(acceptValueClient, &setSockClient)){
            memset(cmd, '\0', sizeof(cmd));
            recv(acceptValueClient, cmd, sizeof(cmd), 0);
            strcpy(copyCMD, cmd);

            memset(command, '\0', strlen(command));
            memset(argument, '\0', strlen(argument));

            strcpy(command, strtok(cmd, " "));
            strcpy(argument, strtok(NULL, "\0 \n"));

            printf("command: %s\n",command);
            printf("argument: %s\n", argument);

            int i;
            for (i=0; i<CMD_SIZE; i++){
                if (strcmp(command, setCommand[i])==0){
                    memset(cmdSend, '\0', strlen(cmdSend));
                    sprintf(cmdSend, "%d %s", i+1, argument);
                    int senval=send(acceptValueSend, cmdSend, 100, 0);
                    if (senval <= 0){
                        exit(socketSend);
                        exit(socketClient);
                        return 0;
                    }
                }
            }
        }
    }
	return EXIT_SUCCESS;
}
