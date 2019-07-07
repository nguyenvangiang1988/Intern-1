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

#define PORT 2000
int sock = 0;
int valueOfShutdown;
char Message[2000];
char start[10];


struct REQUEST {
    int enable; 
    int add;
    int delete;
    char nameRequest [100];
} USER;

void Init(){
    USER.enable= 0;
    USER.add = 0;
    USER.delete =0;
}

void CommandUser(){
    memset(Message, '\0', sizeof(Message));
    gets(Message);
    fflush(stdin);
    if (strcmp (Message, "1") != 0 && strcmp (Message,"0") != 0){
        printf (">> Error command\n");
    }
    else{
        send (sock, Message, strlen(Message), 0);
        
    }
}

int main()
{
    Init();
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr.s_addr);

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
    else {
        printf (">> Connect server complete ! \n");
    }

    memset(start, '\0', sizeof(start));
    while(1){
        while (strcmp (start, "n")) {
            char arrayString [500];
            memset (USER.nameRequest, '\0', sizeof (USER.nameRequest));
            sprintf ( arrayString, "\n>> Please input config: \n"
                                    "1. Add a new room \n"
                                    "2. Delete a room \n"
                                    "3. Enable room's operate \n"
                                    "4. EXIT\n"
                                    "=========================\n");
            puts (arrayString);
            memset(Message, '\0', sizeof(Message));
            printf("%s","> ");
            gets(Message);
            fflush(stdin);
            int message= strtol (Message, 0, 10);
            switch (message){
                case 1:
                    printf (">> Do you want to add a new room (0: no / 1: yes): ");
                    strcpy (USER.nameRequest, "add");
                    send (sock, USER.nameRequest, strlen (USER.nameRequest), 0);
                    CommandUser();
                    USER.add= strtol (Message, 0, 10);
                    break;
                case 2:
                    printf (">> Do you want to delete a room (0: no / 1: yes): ");
                    strcpy (USER.nameRequest, "delete");
                    send (sock, USER.nameRequest, strlen (USER.nameRequest), 0);
                    CommandUser();
                    USER.delete= strtol (Message, 0, 10);

                    break;   
                case 3:
                    printf (">> Do you want to enable room's operate (0: no / 1: yes): ");
                    strcpy (USER.nameRequest, "enable");
                    send (sock, USER.nameRequest, strlen (USER.nameRequest), 0);
                    CommandUser();
                    USER.enable= strtol (Message, 0, 10);
                    break;   
                case 4:
                    printf (">> config complete! Do you want to config continue? (y/n):  ");
                    gets(start);
                    fflush(stdin);
                    if (strcmp (start, "n")==0){
                        printf(">> OK config successfully\n");
                        puts (start);
                        send (sock, start, strlen (start), 0);
                    }
                    exit (1);
                    break;
                default:
                    printf (">> Error comand, please input again \n");
                    break;                      
           }
        }
    }
    
    return 0;
}
