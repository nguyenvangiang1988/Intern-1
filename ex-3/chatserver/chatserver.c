#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "lib.c"

#define MAX_CONNECTIONS 100
#define PORT 21996
#define NICKNAME_LENGTH 32

typedef struct {
    //current nickname of node
    char c_nickname[NICKNAME_LENGTH];
    int bfrom;
    //to nickname of node
    char t_nickname[NICKNAME_LENGTH];
    int bto;
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

void PeersDeleteBySocket(int sk){
    int i, j, end_length;
    for (i = 0; i < peers.length; i++) {
        if (peers.clients[i].c_socket == sk) {
            close(sk);
            end_length = peers.length - 1;
            for (j = i; j < end_length; j++) {
                peers.clients[j] = peers.clients[j + 1];
            }
            peers.length--;
            return;
        }
    }
}

int PeersAddClient(int sk){
    if (peers.length >= MAX_CONNECTIONS)
        return -1;

    int offset = peers.length++;
    peers.clients[offset].bfrom = peers.clients[offset].bto = 0;
    peers.clients[offset].c_socket = sk;
    strncpy(peers.clients[offset].c_nickname, "", NICKNAME_LENGTH);
    strncpy(peers.clients[offset].t_nickname, "", NICKNAME_LENGTH);

    return 0;
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

void PeersAddSocketToFDS(fd_set* fds){
    int i;
    for (i = 0; i < peers.length; i++) {
        FD_SET(peers.clients[i].c_socket, fds);
    }
}

int PeersMaxSocketInFDS(int server_socket){
    int max_fd = server_socket;
    int i;

    for (i = 0; i < peers.length; i++) {
        if (max_fd < peers.clients[i].c_socket) {
            max_fd = peers.clients[i].c_socket;
        }
    }

    return max_fd;
}

void PeersSetClientNickname(int sk, char* cnickname){
    int i = 0;
    for (; i < peers.length; i++) {
        if (peers.clients[i].c_socket == sk) {
            strncpy(peers.clients[i].c_nickname, cnickname, NICKNAME_LENGTH);
            peers.clients[i].bfrom = 1;
        }
    }
}

void PeersSetToNickname(int sk, char* tnickname){
    int i = 0;
    for (; i < peers.length; i++) {
        if (peers.clients[i].c_socket == sk) {
            strncpy(peers.clients[i].t_nickname, tnickname, NICKNAME_LENGTH);
            peers.clients[i].bto = 1;
        }
    }
}

int CreateServerSocket(unsigned int ip, unsigned short port){
    struct sockaddr_in addr = {
        AF_INET,
        htons(port),
        {
            htonl(ip)
        }
    };

    int reuse = 1;

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        perror("create socket failed");
        return -1;
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse) == -1) {
        perror("setsockopt failed");
        close(server_socket);
        return -1;
    }

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

int main(){
    int client_socket;
    struct sockaddr_in client_addr;
    int sizeofsockaddr;
    int running = 1;
    fd_set readfds;
    struct timeval timeout;
    int select_result;
    //iterator
    int i;
    client client_info;
    char* str;

    PeersInitialize();

    int server_socket = CreateServerSocket(INADDR_ANY, PORT);

    if (server_socket == -1) {
        perror("create socket failed");
        printf("closing...\n");
        fflush(stdout);
        return 0;
    }

    printf("listening on 0.0.0.0:%d\n", PORT);

    while (running) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        PeersAddSocketToFDS(&readfds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int max = PeersMaxSocketInFDS(server_socket);

        if (select_result = select(PeersMaxSocketInFDS(server_socket) + 1, &readfds, NULL, NULL, &timeout), select_result < 0) {
            perror("select failed");
        }
        else if (select_result) {
            //check server socket is ready?
            if (FD_ISSET(server_socket, &readfds)) {
                printf("server is accepting...\n");
                sizeofsockaddr = sizeof client_addr;
                client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &sizeofsockaddr);
                if (client_socket == -1) {
                    perror("accept failed");
                }
                else {
                    printf("server accepted client %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    PeersAddClient(client_socket);
                }
            }

            //check all client_socket is ready?
            for (i = 0; i < peers.length; i++) {
                if (FD_ISSET(peers.clients[i].c_socket, &readfds)) {
                    client_info = peers.clients[i];
                    if (client_info.bfrom && client_info.bto) {
                        //exchange message
                        str = GetLineFromSocket(client_info.c_socket);
                        if (str == NULL) {
                            printf("disconnect to client %d\n", client_info.c_socket);
                            PeersDeleteBySocket(client_info.c_socket);
                        }
                        else {
                            SendMessageToNickname(str, client_info.c_nickname, client_info.t_nickname);
                            printf("[%s => %s]: %s\n", client_info.c_nickname, client_info.t_nickname, str);
                            free(str);
                        }
                    }
                    else if (!client_info.bfrom) {
                        //exchange client nickname
                        str = GetLineFromSocket(client_info.c_socket);
                        if (str == NULL) {
                            printf("disconnect to client %d\n", client_info.c_socket);
                            PeersDeleteBySocket(client_info.c_socket);
                        }
                        else {
                            if (strlen(str) == 0) {
                                printf("Empty nickname is invalid, disconnect...\n");
                                PeersDeleteBySocket(client_info.c_socket);
                            }
                            else {
                                printf("[Nickname] OK %s\n", str);
                                PeersSetClientNickname(client_info.c_socket, str);
                            }
                            free(str);
                        }
                    }
                    else if (!client_info.bto) {
                        //exchange to nickname
                        str = GetLineFromSocket(client_info.c_socket);
                        if (str == NULL) {
                            printf("disconnect to client %d\n", client_info.c_socket);
                            PeersDeleteBySocket(client_info.c_socket);
                        }
                        else {
                            if (strlen(str) == 0) {
                                printf("Empty to nickname is invalid, disconnect...\n");
                                PeersDeleteBySocket(client_info.c_socket);
                            }
                            else {
                                printf("[To Nickname] OK %s\n", str);
                                PeersSetToNickname(client_info.c_socket, str);
                            }
                            free(str);
                        }
                    }
                }
            }
        }
    }

    printf("closing...");
    PeersFree();
    close(server_socket);

    return 0;
}
