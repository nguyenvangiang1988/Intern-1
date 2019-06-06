#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "lib.c"

#define MAX_CONNECTIONS 100
#define PORT 21996
#define NICKNAME_LENGTH 32

typedef struct {
    pthread_t tid;
    //current nickname of node
    char c_nickname[NICKNAME_LENGTH];
    //to nickname of node
    char t_nickname[NICKNAME_LENGTH];
    //socket to interface with node
    int c_socket;
} client;

struct {
    client clients[MAX_CONNECTIONS];
    int length;
} peers;

void PeersInitialize(){
    peers.length = 0;
}

void PeersFree(){
    int i = 0;
    for ( ; i < peers.length; i++) {
        close(peers.clients[i].c_socket);
    }
}

int PeersAdd(char* nickname, int socket){
    if (peers.length == MAX_CONNECTIONS) {
        return -1;
    }

    client p;
    strncpy(p.c_nickname, nickname, NICKNAME_LENGTH);
    p.c_socket = socket;

    peers.clients[peers.length++] = p;
    return 0;
}

void PeersDeleteByNickname(char* nickname){
    int i = 0, j;
    while (i < peers.length) {
        if (strcmp(nickname, peers.clients[i].c_nickname) == 0) {
            for(j = i; j < peers.length - 1; j++) {
                peers.clients[j] = peers.clients[j + 1];
            }

            peers.length--;
        }
        else {
            i++;
        }
    }
}

int PeersGetLength(){
    return peers.length;
}

void PeersPrint(){
    int i = 0;
    for ( ; i < peers.length; i++) {
        printf("Nickname: %s (%d)\n", peers.clients[i].c_nickname, peers.clients[i].c_socket);
    }
}

void PeersDeleteByTID(pthread_t t){
    int i = 0, j;
    while (i < peers.length) {
        if (peers.clients[i].tid == t) {
            for (j = i; j < peers.length - 1; j++) {
                peers.clients[j] = peers.clients[j + 1];
            }

            peers.length--;
        }
        else {
            i++;
        }
    }
}

int PeersHaveNickname(char* nickname) {
    int i;

    for (i = 0; i < peers.length; i++) {
        if (strcmp(peers.clients[i].c_nickname, nickname) == 0) {
            return 1;
        }
    }

    return 0;
}

int CreateServerSocket(unsigned int ip, unsigned short port){
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        perror("create socket failed");
        return -1;
    }

    struct sockaddr_in addr = {
        AF_INET,
        htons(port),
        {
            htonl(ip)
        }
    };

    if (bind(server_socket, (struct sockaddr*)&addr, sizeof addr) == -1) {
        perror("bind failed");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        perror("listen failed");
        close(server_socket);
        return -1;
    }

    return server_socket;
}

int SendMessageToNickname(char* msg, char* fromnickname, char* nickname){
    char* message = (char*)malloc(strlen(fromnickname) + strlen(": ") + strlen(msg) + 1);
    int i;
    int sbytes;

    strcpy(message, "");
    strcat(message, fromnickname);
    strcat(message, ": ");
    strcat(message, msg);
    
    for (i = 0; i < peers.length; i++) {
        if (strcmp(peers.clients[i].c_nickname, nickname) == 0) {
            sbytes = SendLineToSocket(peers.clients[i].c_socket, message);
            if (sbytes == -1) {
                perror("sent message failed");
                return -1;
            }
        }
    }
    
    free(message);

    return 0;
}

void* ClientCommunicationThread(void* args){
    client* clientinfo = (client*)args;
    int clone_socket = clientinfo->c_socket;
    char clone_c_nickname[NICKNAME_LENGTH], clone_t_nickname[NICKNAME_LENGTH];
    pthread_t clone_tid;
    int sbytes;
    char* rstr;
    int b_received_nickname = 0;
    char* message;

    rstr = GetLineFromSocket(clone_socket);

    //clone thread id, can replace tid by id set manually
    clone_tid = clientinfo->tid;

    if (rstr == 0) {
        perror("get line from socket failed");
        printf("delete TID: 0x%X from Peers array\n", clone_tid);
        PeersDeleteByTID(clone_tid);
        printf("closing client socket...\n");
        close(clone_socket);
        return 0;
    }
    else {
        strncpy(clientinfo->c_nickname, rstr, NICKNAME_LENGTH);
        strncpy(clone_c_nickname, rstr, NICKNAME_LENGTH);
        free(rstr);
        if (strlen(clone_c_nickname) != 0) {
            printf("Nickname received: %s\n", clone_c_nickname);
        }
        else {
            printf("Nickname invalid!\n");
            SendLineToSocket(clone_socket, "Nickname invalid!");

            printf("Disconnect this client\n");
            close(clone_socket);

            printf("delete TID: 0x%X from Peers array\n", clone_tid);
            PeersDeleteByTID(clone_tid);

            return 0;
        }
    }

    rstr = GetLineFromSocket(clone_socket);
    if (rstr == 0) {
        perror("get line from socket failed");
        printf("delete TID: 0x%X from Peers array\n", clone_tid);
        PeersDeleteByTID(clone_tid);
        printf("closing client socket...\n");
        close(clone_socket);
        return 0;
    }
    else {
        strncpy(clientinfo->t_nickname, rstr, NICKNAME_LENGTH);
        strncpy(clone_t_nickname, rstr, NICKNAME_LENGTH);
        free(rstr);
        if (strlen(clone_t_nickname) != 0 && strcmp(clone_c_nickname, clone_t_nickname) && PeersHaveNickname(clone_t_nickname)) {
            printf("To nickname received: %s\n", clone_t_nickname);
        }
        else {
            if (strlen(clone_t_nickname) == 0) {
                SendLineToSocket(clone_socket, "To nickname invalid!");
                printf("To nickname invalid!\n");
            }

            if (!PeersHaveNickname(clone_t_nickname)) {
                SendLineToSocket(clone_socket, "To nickname not found");
                printf("To nickname not found\n");
            }

            if (strcmp(clone_c_nickname, clone_t_nickname) == 0) {
                SendLineToSocket(clone_socket, "You cannot chat yourself");
                printf("You cannot chat yourself\n");
            }

            printf("Disconnect this client\n");
            close(clone_socket);

            printf("delete TID: 0x%X from Peers array\n", clone_tid);
            PeersDeleteByTID(clone_tid);

            return 0;
        }
    }

    printf("OK this connect can communicate, between %s and %s\n", clone_c_nickname, clone_t_nickname);
    
    while (message = GetLineFromSocket(clone_socket), message != 0) {
        printf("[%s -> %s]: %s\n", clone_c_nickname, clone_t_nickname, message);
        SendMessageToNickname(message, clone_c_nickname, clone_t_nickname);
        free(message);
    }
    
    printf("Disconnect this client\n");
    close(clone_socket);
    
    printf("delete TID: %d from Peers array\n", clone_tid);
    PeersDeleteByTID(clone_tid);

    return 0;
}

int main(){
    int server_socket = CreateServerSocket(INADDR_ANY, PORT);
    int client_socket;
    pthread_t tid;
    client* clientinfo;
    struct sockaddr_in client_addr;
    int sizeofsockaddr = sizeof client_addr;

    PeersInitialize();

    if (server_socket == -1) {
        printf("closing...");
        fflush(stdout);
        return 0;
    }

    while (client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &sizeofsockaddr), client_socket != -1) {
        PeersAdd("", client_socket);
        pthread_create(&tid, 0, ClientCommunicationThread, peers.clients + peers.length - 1);
        peers.clients[peers.length - 1].tid = tid;
        printf("Client IP: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("%d connections\n", PeersGetLength());
    }

    perror("accept failed");

    printf("closing...");
    PeersFree();
    close(server_socket);

    return 0;
}
