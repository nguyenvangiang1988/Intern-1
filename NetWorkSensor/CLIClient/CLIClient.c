/*
 * CLIClient.c
 *
 *  Created on: Jul 5, 2019
 *      Author: lhungtam
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

int main(){
    int socketValue;
    struct sockaddr_in addressILS;
    char *cmd;

    socketValue=socket(AF_INET, SOCK_STREAM, 0);
    if (socketValue < 0){
        perror("socket");
        return 0;
    }

    addressILS.sin_family=AF_INET;
    addressILS.sin_port=htons(3501);
    inet_aton("192.168.81.136", &addressILS.sin_addr.s_addr);

    cmd=(char*)malloc(sizeof(char)*100);

    connect(socketValue, (struct sockaddr*)&addressILS, sizeof(addressILS));

    while (1){
        memset(cmd, '\0', sizeof(cmd));
        gets(cmd);
        send(socketValue, cmd, strlen(cmd), 0);
    }
}
