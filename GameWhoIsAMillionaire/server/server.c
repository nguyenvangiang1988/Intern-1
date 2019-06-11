#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define sizeOfListQuestion 10
#define maxListClient 20

struct question{
    char *content;
    char *answerA;
    char *answerB;
    char *answerC;
    char *answerD;
    char *result;
};
struct question listQuestion[sizeOfListQuestion];

struct client{
    int socket;
    char nickname[20];
    int number;
    int passing;
};
struct client listClient[maxListClient];

struct sockaddr_in addressClient;
char message[1024];
char answer[10];
fd_set listSocket;
int sizeFD;
int valueOfSocket;
int valueOfAccept;
int sizeListClient=2;//value random
int countQuestion;
FILE *fileQuestion;
FILE *fileAnswer;

void ReadFileQuestion(){
    countQuestion=0;
    fileQuestion=fopen("bocauhoi1.txt", "r");
    if (fileQuestion==NULL){
            perror("fileQuestion");
    }
    else{
        char buf[512];
        while (!feof(fileQuestion)){

            memset(buf, '\0', sizeof(buf));
            fgets(buf, sizeof(buf), fileQuestion);
            listQuestion[countQuestion].content=(char*)malloc(strlen(buf));
            strcpy(listQuestion[countQuestion].content, buf);

            memset(buf, '\0', sizeof(buf));
            fgets(buf, sizeof(buf), fileQuestion);
            listQuestion[countQuestion].answerA=(char*)malloc(strlen(buf));
            strcpy(listQuestion[countQuestion].answerA, buf);

            memset(buf, '\0', sizeof(buf));
            fgets(buf, sizeof(buf), fileQuestion);
            listQuestion[countQuestion].answerB=(char*)malloc(strlen(buf));
            strcpy(listQuestion[countQuestion].answerB, buf);

            memset(buf, '\0', sizeof(buf));
            fgets(buf, sizeof(buf), fileQuestion);
            listQuestion[countQuestion].answerC=(char*)malloc(strlen(buf));
            strcpy(listQuestion[countQuestion].answerC, buf);

            memset(buf, '\0', sizeof(buf));
            fgets(buf, sizeof(buf), fileQuestion);
            listQuestion[countQuestion].answerD=(char*)malloc(strlen(buf));
            strcpy(listQuestion[countQuestion].answerD, buf);

            fgets(buf, sizeof(buf), fileQuestion);

            countQuestion++;
        }
    }
    fclose(fileQuestion);
}

void ReadFileAnswer(){
    fileAnswer=fopen("bocautraloi1.txt","r");
    if (fileAnswer==NULL){
        perror("fileAnswer");
    }
    else{
        countQuestion=0;
        while (!feof(fileAnswer)){
            char buf[512];
            fgets(buf, sizeof(buf), fileAnswer);
            listQuestion[countQuestion].result=(char*)malloc(strlen(buf));
            strcpy(listQuestion[countQuestion].result, buf);
            countQuestion++;
            memset(buf, '\0', sizeof(buf));
        }
    }
    fclose(fileAnswer);
}

void InitListClient(){
    int i;
    for (i=0; i<20; i++){
        listClient[i].socket=0;
        listClient[i].number=i+1;
        listClient[i].passing=0;
        memset(&listClient[i].nickname, '\0', sizeof(listClient[i].nickname));
    }
}

int IsExitNickname(char nickname[]){
    int i;
    for (i=0; i<strlen(nickname); i++){
        if (strcmp(listClient[i].nickname, nickname)==0){
            return 1;
        }
    }
    return 0;
}

int CheckCharacterInNickname(char nickname[]){
    int i;
    for (i=0; i<strlen(nickname); i++){
        if (message[i]==10 || message[i]==13 || (message[i]>=48 && message[i]<=57) || (message[i]>=65 && message[i]<=90) || (message[i]>=97 && message[i]<=122)){
            continue;
        }
        else{
            return 1;
        }
    }
    return 0;
}

void RandomClient(int *sizeListcClient){
    (*sizeListcClient)=rand()%10;
}

void ReFreshListSocket(){
    FD_ZERO(&listSocket);
    FD_SET(valueOfSocket, &listSocket);

    sizeFD=valueOfSocket;

    for (int i=0; i<maxListClient; i++){
        if (listClient[i].socket > 0){
            FD_SET(listClient[i].socket, &listSocket);
        }
        if (sizeFD < listClient[i].socket){
            sizeFD=listClient[i].socket;
        }
    }
}

void concentrates(int position){
    if(position == sizeListClient-1)
        sizeListClient--;
    else{
        if(position < sizeListClient-1){
            int i;
            for (i=position; i<sizeListClient-1; i++){
                memset(listClient[i].nickname, '\0', sizeof(listClient[i].nickname));

                strcpy(listClient[i].nickname, listClient[i+1].nickname);
                listClient[i].number=listClient[i+1].number;
                listClient[i].passing=listClient[i+1].passing;
                listClient[i].socket=listClient[i+1].socket;
            }
            sizeListClient--;
        }
    }
}

void WaitAndRecieveInfoClient(int socket){
    select(socket+1, &listSocket, NULL, NULL, NULL);
    if(FD_ISSET(socket, &listSocket)){
        memset(&message, '\0', sizeof(message));
        recv(socket, message, sizeof(message), 0);
        puts(message);
        printf("from IP: %s\n",inet_ntoa(addressClient.sin_addr));
    }
}

void RecieveAnswerClient(int position, char *buf){
    memset(answer, '\0', sizeof(answer));
    recv(listClient[position].socket, answer, sizeof(answer), 0);
    while (!strcmp(answer, "")){
        send(listClient[position].socket, buf, strlen(buf), 0);
        recv(listClient[position].socket, answer, sizeof(answer), 0);
        sprintf(answer, "\n");
        puts(answer);
    }
}

int main (){
    socklen_t addressClient_Len;

    int positionQuestion=0;

    struct sockaddr_in addressServer;

    int valueOfBind;
    int valueOfListen;

    int countClient=0;
    int waitFullClient=1;
    int sendOnlyOne=1;

    char notificationFalse[]="Nickname has exit or have special character. Enter new nickname please";

    valueOfSocket=socket(AF_INET, SOCK_STREAM, 0);
    if (valueOfSocket < 0){
        perror("socket");
        return -1;
    }
    else{
        printf("Create socket successfully\n");
    }

    memset(&addressServer, '\0', sizeof(struct sockaddr_in));

    addressServer.sin_port=htons(1234);
    addressServer.sin_family=AF_INET;
    inet_aton("192.168.81.128", &addressServer.sin_addr.s_addr);

    valueOfBind=bind(valueOfSocket, (struct sockaddr*)&addressServer, sizeof(addressServer));
    if (valueOfBind < 0){
        perror("bind");
        close(valueOfSocket);
    }
    else{
        printf("Bind successfully\n");
    }

    valueOfListen=listen(valueOfSocket, 10);
    if (valueOfListen < 0){
        perror("listen");
        close(valueOfSocket);
    }
    else{
        printf("Listening...");
    }

    InitListClient();
    ReadFileQuestion();
    ReadFileAnswer();
    addressClient_Len=sizeof(addressClient);

    while (1){
        ReFreshListSocket();

        while (waitFullClient){
            ReFreshListSocket();
            select(sizeFD+1, &listSocket, NULL, NULL, NULL);

            if (FD_ISSET(valueOfSocket, &listSocket)){
                valueOfAccept=accept(valueOfSocket, (struct sockaddr*)&addressClient, &addressClient_Len);

                if (valueOfAccept > 0){
                    FD_SET(valueOfAccept, &listSocket);
                    printf("Accept successfully from IP: %s\n",inet_ntoa(addressClient.sin_addr));
                    send(valueOfAccept, "Enter nickname: ", sizeof("Enter nickname: "), 0);
                    WaitAndRecieveInfoClient(valueOfAccept);

                    while (IsExitNickname(message) || CheckCharacterInNickname(message)){
                        send(valueOfAccept, notificationFalse, sizeof(notificationFalse), 0);
                        WaitAndRecieveInfoClient(valueOfAccept);
                    }
                    send(valueOfAccept, "Sign up nickname successful\n", sizeof("Sign up nickname successful\n"), 0);

                    for (int i=0; i<maxListClient; i++){//save nickname
                        if (listClient[i].socket == 0){
                            listClient[i].socket=valueOfAccept;
                            strcpy(listClient[i].nickname, message);
                            if (sizeFD < listClient[i].socket){
                                sizeFD=listClient[i].socket;
                            }
                            break;
                        }
                    }
                    countClient++;
                }
                else{
                    perror("accept");
                    close(valueOfSocket);
                }
            }

            for (int i=0; i<countClient; i++){//send all client
                send(listClient[i].socket, "The system will start when full client !!\n", strlen("The system will start when full client !!\n"), 0);
                printf("The system will start when full client !!\n");
            }

            if (countClient == sizeListClient){
                waitFullClient=0;
            }
        }

        while (sendOnlyOne){
            for (int i=0; i<sizeListClient; i++){
                char totals[60];
                memset(&totals, '\0', sizeof(totals));
                sprintf(totals, "Totals client: %d \n"
                                "Your position: %d \n"
                                "Totals question: %d\n",sizeListClient, i+1, sizeListClient*5);
                send(listClient[i].socket, totals, sizeof(totals), 0);
                printf("%s", totals);

                char start[]="We start !!\n";
                send(listClient[i].socket, start, strlen(start), 0);
                printf("%s", start);
            }
            sendOnlyOne=0;
        }

        for (int i=0; i<sizeListClient; i++){//send 1 answer all client
            ReFreshListSocket();
            char *buf;
            buf=(char*)malloc(sizeof(char)*1024);
            if (FD_ISSET(listClient[i].socket, &listSocket)){

                memset(buf, '\0', 1024);
                strcat(buf, listQuestion[positionQuestion].content);
                strcat(buf, listQuestion[positionQuestion].answerA);
                strcat(buf, listQuestion[positionQuestion].answerB);
                strcat(buf, listQuestion[positionQuestion].answerC);
                strcat(buf, listQuestion[positionQuestion].answerD);
                strcat(buf, "\nNote: you can choose ignore but it only one!(next: q)\n");

                send(listClient[i].socket, buf, strlen(buf), 0);

                printf("%s",listQuestion[positionQuestion].content);
                printf("%s",listQuestion[positionQuestion].answerA);
                printf("%s",listQuestion[positionQuestion].answerB);
                printf("%s",listQuestion[positionQuestion].answerC);
                printf("%s",listQuestion[positionQuestion].answerD);
                printf("To IP: %s\n",inet_ntoa(addressClient.sin_addr));
            }
            select(sizeFD+1, &listSocket, NULL, NULL, NULL);

            if (FD_ISSET(listClient[i].socket, &listSocket)){
                RecieveAnswerClient(i, buf);
                puts(answer);
                printf("\nFrom IP: %s\n", inet_ntoa(addressClient.sin_addr));
                if (strcmp("q\n", answer)==0){
                    if (listClient[i].passing==1){
                        send(listClient[i].socket, "You can't choose please answer!\n", sizeof("You can't choose please answer!\n"), 0);

                        select(sizeFD+1, &listSocket, NULL, NULL, NULL);

                        if (FD_ISSET(listClient[i].socket, &listSocket)){
                            RecieveAnswerClient(i, buf);
                        }
                    }
                    else{
                        listClient[i].passing=1;
                        continue;
                    }
                }
                if (strcmp(listQuestion[positionQuestion].result, answer)!=0){
                    send(listClient[i].socket, "Your answer is false\n", sizeof("Your answer is false\n"), 0);
                    listClient[i].socket=0;
                    printf("answer of %s false\n",listClient[i].nickname);
                    concentrates(i);
                    printf("client[%d] has deleted\n", i);
                }
                else{
                    send(listClient[i].socket, "Your answer is true\n", sizeof("Your answer is true\n"), 0);
                    printf("answer of %s true\n",listClient[i].nickname);
                }
            }
            if (sizeListClient == 1){
                printf("The winner is %s\n",listClient[0].nickname);
                send(listClient[0].socket, "You are winner\n", sizeof("You are winner\n"), 0);
                shutdown(valueOfSocket, SHUT_RDWR);
                break;
            }
        }
        positionQuestion++;
    }
    return 0;
}

























