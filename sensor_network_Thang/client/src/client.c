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
    int disable; 
    int add;
    int delete;
    int enable;
    char nameRequest [100];
} USER;

void Init(){
    USER.disable= 0;
    USER.add = 0;
    USER.delete =0;
    USER.enable =0;
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
        printf ("ok: %s\n", Message);
        
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
                                    "3. Disable room's operate \n"
                                    "4. Enable room's operate\n"
                                    "5. EXIT\n"
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

                    printf (">> Input room's number which want to delete: ");
                    char delRoom [10];
                    gets (delRoom);
                    fflush(stdin);
                    send (sock, delRoom, strlen (delRoom), 0);
                    
                    USER.delete= strtol (Message, 0, 10);

                    break;   
                case 3:
                    printf (">> Do you want to disable room's operate (0: no / 1: yes): ");
                    strcpy (USER.nameRequest, "disable");
                    send (sock, USER.nameRequest, strlen (USER.nameRequest), 0);
                    printf ("send: %s\n", USER.nameRequest);
                    CommandUser();

                    printf (">> choice room want to disable send data: ");
                    char disableRoom [10];
                    gets (disableRoom);
                    fflush(stdin);
                    send (sock, disableRoom, strlen (disableRoom), 0);
                    printf ("dis room: %s\n", disableRoom);
                    USER.disable= strtol (Message, 0, 10);
                    break;   
               case 4:
                    printf (">> Do you want to enable room's operate (0: no / 1: yes): ");
                    strcpy (USER.nameRequest, "enable");
                    send (sock, USER.nameRequest, strlen (USER.nameRequest), 0);
                    printf ("send: %s\n", USER.nameRequest);
                    CommandUser();

                    printf (">> choice room want to enale send data: ");
                    char enableRoom [10];
                    gets (enableRoom);
                    fflush(stdin);
                    send (sock, enableRoom, strlen (enableRoom), 0);
                    printf ("en room: %s\n", enableRoom);
                    USER.enable= strtol (Message, 0, 10);
                    break; 
                case 5:
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
