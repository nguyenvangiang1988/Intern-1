#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/ether.h>
#include<sys/socket.h>
#include<arpa/inet.h>

struct FILTER{
    char *nameProtocol;
    char choose[5];
    int check;
    int version;
    int typeOfService;
    int packetSize;
    char *sourceAddress;
    char *destAddress;
    int indentifycation;
};

struct FILTER PROTOCOL[5];

void InitProtocol(){
    for(int i=0; i<5; i++){

        PROTOCOL[i].version= 0;
        PROTOCOL[i].typeOfService= 0;
        PROTOCOL[i].packetSize= 0;
        PROTOCOL[i].check= 0;
         
    }         
}

int main() {
    char input[5][100]=
    {
        "1. TCP (y/n): ",
        "2. UDP (y/n): ",
        "3. ICMP (y/n): ",
        "4. IGMP (y/n): ",
        "5. Other (y/n): "
    };

    struct sockaddr_in sourceAddress, destAddress;
    unsigned char *buffer = (unsigned char *)malloc(65536);
    int sock = socket (AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sock == -1){
        perror("Failed to create socket");
        exit(1);
    }
    printf("Choose packet will be fillter (yes/no): \n");
    for(int i=0; i<5; i++){
        memset(PROTOCOL[i].choose, '\0', sizeof(PROTOCOL[i].choose));
        puts(input[i]);
        printf("Input: ");
        gets(PROTOCOL[i].choose);
        fflush(stdin);
    }
    printf(">> Please wait detect a new comming packet....\n");
    int countProtocol=0;
    int num= 0;

    while(1) {

        int packet = recvfrom(sock , buffer , 65536 , 0 , NULL, NULL);
        if (packet == -1) {
            printf("Failed to get packets\n");
            return 1;
        }
        struct iphdr *ipPacket = (struct iphdr *) (buffer  + sizeof(struct ethhdr));
        memset(&sourceAddress, 0, sizeof(sourceAddress));
        sourceAddress.sin_addr.s_addr = ipPacket->saddr;
        memset(&destAddress, 0, sizeof(destAddress));
        destAddress.sin_addr.s_addr = ipPacket->daddr;
        InitProtocol();

        switch (ipPacket->protocol) {
        case 1:
            PROTOCOL[0].nameProtocol= "TCP";
            PROTOCOL[0].check= 1;
            num= 0;
            break;

        case 2:
            PROTOCOL[1].nameProtocol= "UDP";
            PROTOCOL[1].check= 1;
            num= 1;
            break;

        case 6:
            PROTOCOL[2].nameProtocol= "ICMP";
            PROTOCOL[2].check= 1;
            num= 2;
            break;

        case 17:
            PROTOCOL[3].nameProtocol= "IGMP";
            PROTOCOL[3].check= 1;
            num= 3;
            break;

        default:
            PROTOCOL[4].nameProtocol= "Other";
            PROTOCOL[4].check= 1;
            num= 4; 
            break;
        }
       
        PROTOCOL[num].version= ipPacket->version;
        PROTOCOL[num].typeOfService= ipPacket->tos;
        PROTOCOL[num].packetSize= ntohs(ipPacket->tot_len);
        PROTOCOL[num].sourceAddress= (char *)inet_ntoa(sourceAddress.sin_addr);
        PROTOCOL[num].destAddress= (char *)inet_ntoa(destAddress.sin_addr);
        PROTOCOL[num].indentifycation= ntohs(ipPacket->id);

        for (int i=0; i<5; i++){
            if ( PROTOCOL[i].check == 1 &&  strcmp( PROTOCOL[i].choose, "y")==0){
                FILE *fd= fopen( "my_data.txt", "a+" );
                fprintf(fd, "\n\n------------Protocol: %s------------------\n", PROTOCOL[i].nameProtocol);
                fprintf(fd, ">> Version: %d\n", PROTOCOL[i].version);
                fprintf(fd, ">> Type of services: %d\n", PROTOCOL[i].typeOfService);
                fprintf(fd, ">> Packet Size (byte): %d\n", PROTOCOL[i].packetSize);
                fprintf(fd, ">> Source Address: %s\n", PROTOCOL[i].sourceAddress);
                fprintf(fd, ">> Destination Address: %s\n", PROTOCOL[i].destAddress);
                fprintf(fd, ">> Identification: %d\n", PROTOCOL[i].indentifycation);

                printf("\n\n------------Protocol: %s------------------\n", PROTOCOL[i].nameProtocol);
                printf(">> Version: %d\n", PROTOCOL[i].version);
                printf(">> Type of services: %d\n", PROTOCOL[i].typeOfService);
                printf(">> Packet Size (byte): %d\n", PROTOCOL[i].packetSize);
                printf(">> Source Address: %s\n", PROTOCOL[i].sourceAddress);
                printf(">> Destination Address: %s\n", PROTOCOL[i].destAddress);
                printf(">> Identification: %d\n", PROTOCOL[i].indentifycation);
                printf(">> Please wait detect a new comming packet....\n");
                PROTOCOL[i].check = 0;
                fclose(fd);
                
            }

        }

        
    }
    return 0;
}

