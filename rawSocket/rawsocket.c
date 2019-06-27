#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<netinet/ether.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int i=0, p= 0, src= 0, dst= 0, s= 0, o= 0, srcp=0, dstp=0;
int CheckPort= 0;

struct FILLTER{
    char *protocol;
    int protocolID;
    char *srcAddr;
    char *dstAddr;
    int size;
    char *interface;
    char *fileName;
    int srcPort;
    int dstPort;
};
struct FILLTER filter;

void CheckProtocol(char *initProtocol){
        if(strcmp(initProtocol, "TCP")==0){
           filter.protocol= initProtocol;
           filter.protocolID= 6;
           CheckPort= 1;
        }
        else if(strcmp(initProtocol, "UDP")==0){
           filter.protocol= initProtocol;
           filter.protocolID= 17;
           CheckPort= 1;
        }
        else if(strcmp(initProtocol, "ICMP")==0){
           filter.protocol= initProtocol;
           filter.protocolID= 1;
           CheckPort= 0;
        }
        else if(strcmp(initProtocol, "IGMP")==0){
           filter.protocol= initProtocol;
           filter.protocolID= 2;
           CheckPort= 0;
        }

        else{
            printf("Unknown interface\n");
            exit(1);
        }

}

void CheckFilter(int size, char *init[]){

    for(int j=0; j<size; j++){
        if(j%2 != 0){
           if( strcmp(init[j], "-i")==0){
                filter.interface= init[j+1];
                i=1;
            }
            else if(strcmp(init[j], "-p")==0){
                CheckProtocol(init[j+1]);
                p= 1;
            }
            else if(strcmp(init[j], "-src")==0){
                filter.srcAddr= init[j+1];
                src= 1;
            }
            else if(strcmp(init[j], "-dst")==0){
                filter.dstAddr= init[j+1];
                dst= 1;
            }
            else if(strcmp(init[j], "-s")==0){
                filter.size= strtol( init[j+1], 0, 10);
                s=1;

            }
            else if(strcmp(init[j], "-o")==0){
                filter.fileName= init[j+1];
                o=1;

            }
            else if(strcmp(init[j], "-srcp")==0 && CheckPort==1){
                filter.srcPort= strtol(init[j+1], 0, 10);
                srcp= 1;
            }
            else if(strcmp(init[j], "-dstp")==0 && CheckPort==1){
                filter.dstPort= strtol(init[j+1], 0, 10);
                dstp=1;
            }
            else{
                printf("Error command\n");
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[]) {

    struct sockaddr_in sourceAddress, destAddress;
    int packetSize;
    unsigned char *buffer = (unsigned char *)malloc(65536);
    int sock = socket (AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sock == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }

    CheckFilter(argc, argv);

    if(i==1){
        if(setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, filter.interface, sizeof(filter.interface))<0){
            perror("Unkown interface ");
            exit(1);
        }
    }
    while(1) {

        packetSize = recvfrom(sock , buffer , 65536 , 0 , NULL, NULL);
        if (packetSize == -1) {
            printf("Failed to get packets\n");
            return 1;
        }

        struct iphdr *ipPacket = (struct iphdr *) (buffer  + sizeof(struct ethhdr));
        struct tcphdr *tcpPacket=  (struct tcphdr *) (buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));
        struct udphdr *udpPacket=  (struct udphdr *) (buffer  + sizeof(struct iphdr) + sizeof(struct ethhdr));
        memset(&sourceAddress, 0, sizeof(sourceAddress));
        sourceAddress.sin_addr.s_addr = ipPacket->saddr;
        memset(&destAddress, 0, sizeof(destAddress));
        destAddress.sin_addr.s_addr = ipPacket->daddr;

        char *stringScrAddr= (char *)inet_ntoa(sourceAddress.sin_addr);
        char *stringDstAddr=  (char *)inet_ntoa(destAddress.sin_addr);

        int packetSize= ntohs(ipPacket->tot_len);
        int SrcPortNum;
        int DstPortNum;

        int Start= 0;

        switch (ipPacket->protocol) {
        case 6:
            filter.protocol= "TCP";
            break;

        case 17:
            filter.protocol= "UDP";
            break;

        case 1:
            filter.protocol= "ICMP";
            break;

        case 2:
            filter.protocol= "IGMP";
            break;

        default:
            filter.protocol= "other";
            break;
        }

        if(i==1){
            Start ++;

        }
        if(p==1){
            if(ipPacket -> protocol== filter.protocolID){
                Start ++;
            }
            else
                Start= 0;
        }
        if(src==1){
            if(strcmp(stringScrAddr, filter.srcAddr)==0){
                Start ++;
            }
            else
                Start= 0;
        }
        if(dst==1){
            if(strcmp(stringDstAddr, filter.dstAddr)==0){
                Start ++;
            }
            else
                Start= 0;
        }
        if(s==1){
            if(filter.size < packetSize){
                Start ++;
            }
            else
                Start= 0;
        }
        if(o==1){
            Start ++;
        }
        if(dstp==1 && strcmp(filter.protocol, "TCP")==0 && filter.dstPort == tcpPacket -> th_dport){
            DstPortNum= tcpPacket -> th_dport;
            Start ++;
        }
        if(srcp==1 && strcmp(filter.protocol, "TCP")==0 && filter.srcPort == tcpPacket -> th_sport){
            SrcPortNum= tcpPacket -> th_sport;
            Start ++;
        }
        if(dstp==1 && strcmp(filter.protocol, "UDP")==0 && filter.dstPort == udpPacket -> uh_dport){
            DstPortNum= udpPacket -> uh_dport;
            Start ++;
        }
        if(srcp==1 && strcmp(filter.protocol, "UDP")==0 && filter.srcPort == udpPacket -> uh_sport){
            SrcPortNum= udpPacket -> uh_sport;
            Start ++;
        }
        if(Start==(argc-1)/2){
            if(o==1){
                FILE *fd= fopen("hello.txt", "a+" );
                if(!fd){
                    perror("Write file\n");
                    exit(1);
                }
                else{
                    fprintf(fd, "\n\n================PROTOCOL: %s =================\n",filter.protocol);
                    fprintf(fd, ">> Version: %d\n", ipPacket->version);
                    fprintf(fd, ">> Source address: %s\n", (char *)inet_ntoa(sourceAddress.sin_addr));
                    fprintf(fd, ">> Destination address: %s\n", (char *)inet_ntoa(destAddress.sin_addr));
                    fprintf(fd, ">> Type of services: %d\n", ipPacket->tos);
                    fprintf(fd, ">> Packet size: %d\n", ntohs(ipPacket->tot_len));

                    fclose(fd);
                }
            }

            printf("\n\n================PROTOCOL: %s =================\n",filter.protocol);
            printf(">> Version: %d\n", ipPacket->version);
            printf(">> Source address: %s\n", (char *)inet_ntoa(sourceAddress.sin_addr));
            printf(">> Destination address: %s\n", (char *)inet_ntoa(destAddress.sin_addr));
            printf(">> Type of services: %d\n", ipPacket->tos);
            printf(">> Packet size: %d\n", ntohs(ipPacket->tot_len));
        }
    }
    return 0;
}
