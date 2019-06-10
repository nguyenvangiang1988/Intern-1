#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "lib.c"

#define NICKNAME_LENGTH 32
#define MESSAGE_LENGTH 1000

#define SERVER_IP 0x7f000001
#define SERVER_PORT 21996

//global variables, client_socket for main thread and sub thread
int client_socket;

int ConnectToServer(unsigned int ip, unsigned short port){
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(ip);

    int client_socket = socket(PF_INET, SOCK_STREAM, 0);

    if (client_socket == -1) {
        perror("cannot create client socket");
        return -1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof server_addr) == -1) {
        perror("cannot connect to server");
        close(client_socket);
        return -1;
    }

    return client_socket;
}

//thread to receive is sub thread
void* ClientRecvMessageThread(void* args){
    int client_socket = *((int*)args);
    char* message;

    while (message = GetLineFromSocket(client_socket), message != 0) {
        printf("[RECV] %s\n", message);
        free(message);
    }

    printf("Down client recevice message Thread\n");
    close(client_socket);
    exit(0);
}

void CleanStdInputStream(){
    while (fgetc(stdin) != '\n');
}

int main(int argc, char** args){
    int stop = 0;
    int sresult;
    char message[MESSAGE_LENGTH];
    char nickname[NICKNAME_LENGTH];
    client_socket = ConnectToServer(SERVER_IP, SERVER_PORT);
    pthread_t tid;

    if (client_socket == -1) {
        perror("create client socket failed");
        return 0;
    }

    printf("connect to server successfully\n");

    //thread to receive message from server
    if (pthread_create(&tid, 0, ClientRecvMessageThread, &client_socket)) {
        printf("cannot create thread to receive message\n");
        close(client_socket);
        return 0;
    }

    //thread to send is main thread
    //exchange client nickname to server
    printf("Your nickname: ");
    gets(nickname);
    sresult = SendLineToSocket(client_socket, nickname);
    if (sresult == -1) {
        perror("send nickname failed");
        close(client_socket);
        return 0;
    }

    //exchange to nickname to server
    printf("What nickname do you want to chat: ");
    gets(nickname);
    sresult = SendLineToSocket(client_socket, nickname);
    if (sresult == -1) {
        perror("send partner nickname failed");
        close(client_socket);
        return 0;
    }

    while (!stop) {
        printf("MESSAGE TO SENT TO %s: ", nickname);
        gets(message);
        sresult = SendLineToSocket(client_socket, message);
        if (sresult == -1) {
            perror("send message to partner failed");
            close(client_socket);
            return 0;
        }
        printf("Stop (1. Stop, 0. Continue): ");
        scanf("%d", &stop);
        CleanStdInputStream();
    }

    printf("closing socket...\n");
    close(client_socket);

    return 0;
}
