/*
 * server.c
 *
 *  Created on: May 31, 2019
 *      Author: lhungtam
 */


#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

int client[10], sizeMaxClient=10;

void ClearClient(){
    int i;
    for (i=0; i<sizeMaxClient; i++){
        client[i]=0;
    }
}

int main(){
    struct sockaddr_in addresServer;
    struct sockaddr_in addressClient;

    int valueOfSocket;
    int valueOfBind;
    int valueOfListen;
    int valueOfAccept;
    int valueOfShutdown;

    socklen_t addressClient_Len;

    fd_set listSocket;
    int sizeFD;

    char string[1024];

    valueOfSocket=socket(AF_INET, SOCK_STREAM, 0);
    if (valueOfSocket<0){
        printf("Error create server\n");
        return -1;
    }
    else{
        printf("Create server successful\n");
    }
    memset(&addresServer, '\0', sizeof(addresServer));

    addresServer.sin_port=htons(1234);
    addresServer.sin_family=AF_INET;
    inet_aton("192.168.81.11", &addresServer.sin_addr.s_addr);

    valueOfBind=bind(valueOfSocket,(struct sockaddr*)&addresServer, sizeof(addresServer));
    if (valueOfBind==0){
        printf("Bind is successful\n");
    }
    else {
        printf("Error bind\n");
        perror("BIND: \n");
        goto CLOSE;
    }

    valueOfListen=listen(valueOfSocket, 3);
    if (valueOfListen==0){
        printf("Listening...\n");
    }
    else {
        printf("false\n");
        goto CLOSE;
    }

    int i;
    ClearClient();
    while(1){

        FD_ZERO(&listSocket);
        FD_SET(valueOfSocket, &listSocket);
        sizeFD=valueOfSocket;

        for (i=0; i<sizeMaxClient; i++){

            if (client[i]!=0){
                FD_SET(client[i], &listSocket);
            }

            if (sizeFD < client[i]){
                sizeFD=client[i];
            }
        }

        select(sizeFD+1, &listSocket, NULL, NULL, NULL);

        if (FD_ISSET(valueOfSocket, &listSocket)){

            addressClient_Len=sizeof(struct sockaddr_in);
            valueOfAccept=accept(valueOfSocket, (struct sockaddr*)&addressClient, &addressClient_Len);

            if (valueOfAccept > 0){
                printf("Accept successfully IP: %s, Socket=%d\n", inet_ntoa(addressClient.sin_addr),valueOfAccept);
                int j;

                for (j=0; j<sizeMaxClient; j++){
                    if (client[j]==0){
                        client[j]=valueOfAccept;
                        break;
                    }
                }
            }
            else{
                printf("false\n");
            }
        }

        for (i=0; i<sizeMaxClient; i++){

            if (FD_ISSET(client[i],&listSocket)){

                memset(&string, '\0', sizeof(string));
                recv(client[i], string, sizeof(string), 0);
                puts(string);
                printf("socket=%d\n",client[i]);

                if (strcmp(string, "EXIT")==0){
                    client[i]=0;
                    close(client[i]);
                }
            }
        }
    }

    valueOfShutdown=shutdown(valueOfSocket, SHUT_RDWR);

    if (valueOfShutdown==0){
        printf("Shutdown successful\n");
    }
    else{
        printf("shutdown false\n");
    }
    CLOSE:
        close(valueOfSocket);
        return -1;
    return 0;
}
