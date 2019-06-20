#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/ether.h>
#include<sys/socket.h>
#include<arpa/inet.h>

unsigned char FILE_DATA[5000];
int MAX_PROTOCOL=100;

int main() {

    FILE *fd= fopen( "my_data.txt", "w" );
    struct sockaddr_in sourceAddress, destAddress;
    int packetSize;
    unsigned char *buffer = (unsigned char *)malloc(65536);
    int sock = socket (AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sock == -1)
    {
    perror("Failed to create socket");
    exit(1);
    }
    int countProtocol=0;
    while(countProtocol<= MAX_PROTOCOL) {

        packetSize = recvfrom(sock , buffer , 65536 , 0 , NULL, NULL);
        if (packetSize == -1) {
        printf("Failed to get packets\n");
        return 1;
        }

        struct iphdr *ipPacket = (struct iphdr *) (buffer  + sizeof(struct ethhdr));
        fputs("Incoming Packet: \n", fd);

        switch (ipPacket->protocol) {
        case 1:
            fputs("> protocol: ICMP\n", fd );
            printf("> protocol: ICMP\n");
            break;

        case 2:
            fputs("> protocol: IGMP\n", fd);
            printf("> protocol: IGMP\n");
            break;

        case 6:
            fputs("> protocol: TCP\n", fd);
            printf("> protocol: TCP\n");
            break;

        case 17:
            fputs("> protocol: UDP\n", fd);
            printf("> protocol: UDP\n");
            break;

        default:
            fputs("> protocol: OTHER\n", fd);
            printf("> protocol: OTHER\n");
            break;
        }

        memset(&sourceAddress, 0, sizeof(sourceAddress));
        sourceAddress.sin_addr.s_addr = ipPacket->saddr;
        memset(&destAddress, 0, sizeof(destAddress));
        destAddress.sin_addr.s_addr = ipPacket->daddr;
        fprintf(fd, "> Version: %d\n", ipPacket->version);
        printf("> Version: %d\n", ipPacket->version);

        fprintf(fd, "> Tye of service: %d\n",ipPacket->tos);
        printf("> Tye of service: %d\n",ipPacket->tos);

        fprintf(fd, "> Packet Size (bytes): %d\n",ntohs(ipPacket->tot_len));
        printf("> Packet Size (bytes): %d\n",ntohs(ipPacket->tot_len));

        fprintf(fd, "> Source Address: %s\n", (char *)inet_ntoa(sourceAddress.sin_addr));
        printf("> Source Address: %s\n", (char *)inet_ntoa(sourceAddress.sin_addr));

        fprintf(fd, "> Destination Address: %s\n", (char *)inet_ntoa(destAddress.sin_addr));
        printf("> Destination Address: %s\n", (char *)inet_ntoa(destAddress.sin_addr));

        fprintf(fd, "> Identification: %d\n\n", ntohs(ipPacket->id));
        printf("> Identification: %d\n\n", ntohs(ipPacket->id));

        countProtocol++;

}
    fclose(fd);
    return 0;
}
