#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT_USER 2000
#define PORT_SEND 5678
#define PORT_RECV 1234

char buffer[100];
struct sockaddr_in serv_addr;
socklen_t addrlen;
int SOCK_USER;
int SOCK_SEND;
int SOCK_RECV;
int acceptSend;
int START_SEND= 0;
fd_set LIST_USER;
fd_set LIST_REQUEST;
fd_set LIST_DATA;

struct REQUEST {
    int addRoom;
    int delete;
    int delRoom;
    int disable;
    int disRoom;
    int enable;
    int enRoom;
} ELEMENT;

void InitConfig(){
    ELEMENT.addRoom    = 0;
    ELEMENT.delete     = 0;
    ELEMENT.disable    = 0;
    ELEMENT.enable     = 0;
    ELEMENT.delRoom    = -1;
    ELEMENT.disRoom    = -1;
    ELEMENT.enRoom     = -1;
    
}

void *ConnectUser(){
    int acceptUser;

    while(1){

        FD_ZERO (&LIST_USER);
        FD_SET (SOCK_USER, &LIST_USER);
        select (SOCK_USER +1, &LIST_USER, NULL, NULL, NULL);
        if (FD_ISSET (SOCK_USER, &LIST_USER)){

            addrlen= sizeof(struct sockaddr_in);
            acceptUser= accept (SOCK_USER, (struct sockaddr*)&serv_addr, &addrlen);
            FD_SET (acceptUser, &LIST_USER); 
            if (acceptUser< 0){ 

                perror("ACCEPT 1 ");
                exit(1);
            }  
            printf(">> there is a USER connect\n");
        }
        memset (buffer, '\0', sizeof (buffer));
        while(1){

            select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
            if (FD_ISSET (acceptUser, &LIST_USER)){

                memset (buffer, '\0', sizeof (buffer));
                recv (acceptUser, buffer, sizeof (buffer), 0);
                if (strcmp (buffer, "n")== 0){
                    START_SEND= 1;
                    break;
                }
                if (strcmp (buffer, "add") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.addRoom = strtol (buffer, 0, 10);
                        ELEMENT.delete= 0;
                    } 
                }
                if (strcmp (buffer, "delete") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.delete = strtol (buffer, 0, 10);
                        select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                        if (FD_ISSET (acceptUser, &LIST_USER)){

                            memset (buffer, '\0', sizeof (buffer));
                            recv (acceptUser, buffer, sizeof (buffer), 0);
                            ELEMENT.delRoom = strtol (buffer, 0, 10);
                        } 
                        ELEMENT.addRoom= 0;
                    } 
                }
                if (strcmp (buffer, "disable") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.disable = strtol (buffer, 0, 10);
                        printf ("recv flag dis int : %d\n", ELEMENT.disable);
                        select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                        if (FD_ISSET (acceptUser, &LIST_USER)){

                            memset (buffer, '\0', sizeof (buffer));
                            recv (acceptUser, buffer, sizeof (buffer), 0);
                            ELEMENT.disRoom = strtol (buffer, 0, 10);
                            printf ("recv room num int : %d\n", ELEMENT.disRoom);
                        }
                    } 
                }
                if (strcmp (buffer, "enable") ==0){

                    select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                    if (FD_ISSET (acceptUser, &LIST_USER)){

                        memset (buffer, '\0', sizeof (buffer));
                        recv (acceptUser, buffer, sizeof (buffer), 0);
                        ELEMENT.enable = strtol (buffer, 0, 10);
                        printf ("recv flag dis int : %d\n", ELEMENT.disable);
                        select (acceptUser + 1, &LIST_USER, NULL, NULL, NULL);
                        if (FD_ISSET (acceptUser, &LIST_USER)){

                            memset (buffer, '\0', sizeof (buffer));
                            recv (acceptUser, buffer, sizeof (buffer), 0);
                            ELEMENT.enRoom = strtol (buffer, 0, 10);
                            printf ("recv room num int : %d\n", ELEMENT.enRoom);
                        }
                    } 
                }

            }
        }

    }
}

void *SendData(){
    addrlen= sizeof(struct sockaddr_in);
    int acceptSend;    
    while(1){

        FD_ZERO (&LIST_REQUEST);
        FD_SET (SOCK_SEND, &LIST_REQUEST);
        if (FD_ISSET (SOCK_SEND, &LIST_REQUEST)){      

            acceptSend= accept (SOCK_SEND, (struct sockaddr*)&serv_addr, &addrlen);
            if (acceptSend < 0 ){
                perror ("ACCEPT SEND ");
                exit (1);
            }

            else{
                printf (">> Thread program is connected\n");
            }

        }
        while(1){

            if (START_SEND == 1){

                printf ("start send\n");
                if (ELEMENT.addRoom == 1){

                    char *addRequest= "add";
                    send (acceptSend, addRequest, strlen (addRequest), 0);
                    ELEMENT.addRoom = 0; 
                }
                if (ELEMENT.delete == 1){
                    
                    char *delRequest= "delete";
                    send (acceptSend, delRequest, strlen (delRequest), 0);
                    ELEMENT.delete = 0; 
                }               
                if (ELEMENT.delRoom > 0){

                    char del[5];
                    sprintf (del, "%d", ELEMENT.delRoom);
                    send (acceptSend, del, strlen (del), 0);
                    ELEMENT.delRoom= -1;
                }

                
                if (ELEMENT.disable == 1){
                    
                    char *enableRequest= "disable";
                    send (acceptSend, enableRequest, strlen (enableRequest), 0);
                    printf ("send name request str: %s\n", enableRequest);
                    ELEMENT.disable = 0; 
                }
                if (ELEMENT.disRoom > 0){

                    char dis[5];
                    sprintf (dis, "%d", ELEMENT.disRoom);
                    send (acceptSend, dis, strlen (dis), 0);
                    printf ("send num room want to dis string: %s\n", dis );
                    ELEMENT.disRoom= -  1;
                }
                if (ELEMENT.enable == 1){
                    
                    char *enableRequest= "enable";
                    send (acceptSend, enableRequest, strlen (enableRequest), 0);
                    printf ("send name request str: %s\n", enableRequest);
                    ELEMENT.disable = 0; 
                }
                if (ELEMENT.enRoom > 0){

                    char en[5];
                    sprintf (en, "%d", ELEMENT.enRoom);
                    send (acceptSend, en, strlen (en), 0);
                    printf ("send num room want to en string: %s\n", en );
                    ELEMENT.enRoom= -  1;
                }

                perror ("send");
                START_SEND= 0;
            }
        }
    }
}

void *RecvData(){
    addrlen= sizeof(struct sockaddr_in);
    int acceptRecv;
    while(1){
        
        FD_ZERO (&LIST_DATA);
        FD_SET (SOCK_RECV, &LIST_DATA);
        select (SOCK_RECV + 1, &LIST_DATA, NULL, NULL, NULL);
        if (FD_ISSET (SOCK_RECV, &LIST_DATA)){
            
            acceptRecv= accept (SOCK_RECV, (struct sockaddr *)&serv_addr, &addrlen);
            if (acceptRecv < 0){
                
                perror ("Accept Recv");
                exit (1);
            }
            else
                printf (">> Ready recived data\n");
                FD_SET (acceptRecv, &LIST_DATA);
            while(1){
               
                int arrayList [10];
                int count= 0, sum= 0;
                int countSum= 0;
                char arrayString [1024];
                memset (arrayList, 0, sizeof (arrayList));
                while(1){
                    select (acceptRecv + 1, &LIST_DATA, NULL, NULL, NULL);
                    if (FD_ISSET (acceptRecv, &LIST_DATA)){
                        memset (arrayString, '\0', sizeof (arrayString));
                        recv (acceptRecv, arrayString, sizeof (arrayString), 0);

                        if (strcmp (arrayString, "All room is sended data")==0){
                            
                            for (int i=0; i< 10; i++){
                                if (arrayList[i] != 0){
                                    sum += arrayList[i];
                                    countSum ++;
                                }
                            }
                            int sumTB= sum/countSum;
                            printf (">> SUM TB= %d\n", sumTB);
                            printf ("reset ok\n\n");          
                            break;
                        }
                        else{
                            arrayList [count]= strtol (arrayString, 0, 10);
                            printf ("array list %d: %d\n", count, arrayList [count]);
                            count ++;
                        }
                    }
                }
                
            }
        }
    }    
}

int main(int argc, char *argv){

    InitConfig();
    pthread_t threadUser, threadSend, threadRecv;
    memset (&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = INADDR_ANY;

    serv_addr.sin_port          = htons (PORT_USER);
    SOCK_USER= socket (AF_INET, SOCK_STREAM, 0);   
    if (SOCK_USER < 0){
        perror("socket accept ");
        exit(1);
    }   
    if (bind (SOCK_USER, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind accept ");
        exit(1);
    }
///=====================================port send
    serv_addr.sin_port          = htons (PORT_SEND);
    SOCK_SEND= socket (AF_INET, SOCK_STREAM, 0);   
    if (SOCK_SEND < 0){
        perror("socket accept ");
        exit(1);
    }   
    if (bind (SOCK_SEND, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind accept ");
        exit(1);
    }

    if (listen (SOCK_USER, 5) <0){
        perror("listen accept ");
        exit(1);
    }
    
    if (listen (SOCK_SEND, 5) <0){
        perror("listen accept ");
        exit(1);
    }
//=======================================port recv
    serv_addr.sin_port          = htons (PORT_RECV);
    SOCK_RECV= socket (AF_INET, SOCK_STREAM, 0);   
    if (SOCK_RECV < 0){
        perror("socket accept ");
        exit(1);
    }   
    if (bind (SOCK_RECV, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind accept ");
        exit(1);
    }

    if (listen (SOCK_RECV, 5) <0){
        perror("listen accept ");
        exit(1);
    }




//================================================================================

    pthread_create (&threadUser, NULL, ConnectUser, NULL);
    pthread_create (&threadSend, NULL, SendData, NULL);
    pthread_create (&threadRecv, NULL, RecvData, NULL);

    while(1);


    return 0;
}
