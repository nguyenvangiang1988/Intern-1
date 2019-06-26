/*
 * SocketRaw_Lib.c
 *
 *  Created on: Jun 19, 2019
 *      Author: lhungtam
 */


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<netinet/ether.h>
#include<netinet/ip_icmp.h>
#include<net/if.h>
#include<unistd.h>
#include<time.h>

#define SIZE_PARAMETER 8
struct ParameterExecute{
    char *valueName;
    char *nameParameter;
    int flagExecute;
};
struct ParameterExecute arrayExecute[SIZE_PARAMETER];

void InitialParameterExecute(){
    int i;
    arrayExecute[0].nameParameter="-eth";
    arrayExecute[1].nameParameter="-proto";
    arrayExecute[2].nameParameter="-portsrc";
    arrayExecute[3].nameParameter="-portdst";
    arrayExecute[4].nameParameter="-srcaddr";
    arrayExecute[5].nameParameter="-dstaddr";
    arrayExecute[6].nameParameter="-f";
    arrayExecute[7].nameParameter="-size";
    for (i=0; i<SIZE_PARAMETER; i++){
        arrayExecute[i].flagExecute=0;
        arrayExecute[i].valueName=(char*)malloc(sizeof(char)*10);
        memset(arrayExecute[i].valueName, '\0', 10);
    }
}

void SetParameterExecute(int agrc, char *agrv[]){
    int i, j;
    for (i=1; i<agrc; i=i+2){
        for (j=0; j<SIZE_PARAMETER; j++){
            if (i==1 && strcmp(agrv[i], "-eth")==0){
                continue;
            }
            else{
                if (strcmp(agrv[i], arrayExecute[j].nameParameter)==0){
                    arrayExecute[j].flagExecute=1;
                    strcpy(arrayExecute[j].valueName, agrv[i+1]);
                }
            }
        }
    }
}

int GetProtocol(char *protocolName){
    if (strcmp("TCP", protocolName) == 0){
        return 6;
    }
    else{
        if (strcmp("UDP", protocolName) == 0){
            return 17;
        }
        else{
            if (strcmp("ICMP", protocolName) == 0){
                return 1;
            }
            else{
                return -1;
            }
        }
    }
}

int FilterProtocol(int flagExecute, unsigned char **buffer, int size, int getprotocol){//getprotocol ==> function GetProtocol
    if (flagExecute == 0){
        return 0;
    }
    else{
        struct iphdr *ipPacket;
        ipPacket=(struct iphdr*)(*buffer + sizeof(struct ethhdr));
        if (ipPacket->protocol==getprotocol){
            return 0;
        }
        else{
            return 1;
        }
    }
}

int FilterPortSrc(int flagExecute, unsigned char *buffer, int size, char *portNumberSrc, int flagSrc){
    if (flagExecute == 0){
        return 0;
    }
    else{
        if (flagSrc==6){
            struct tcphdr *tcpPacket;
            tcpPacket=(struct tcphdr*)(buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));

            if (ntohs(tcpPacket->th_sport) == strtol(portNumberSrc, 0, 10)){
                return 0;
            }
            else{
                return 1;
            }
        }
        else{
            if (flagSrc==17){
                struct udphdr *udpPacket;
                udpPacket=(struct udphdr*)(buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));

                if (ntohs(udpPacket->source) == strtol(portNumberSrc, 0, 10)){
                    return 0;
                }
                else{
                    return 1;
                }
            }
            else{
                return 0;
            }
        }
    }
}

int FilterPortDst(int flagExecute, unsigned char *buffer, int size, char *portNumberDst, int flagDst){
    if (flagExecute == 0){
        return 0;
    }
    else{
        if (flagDst==1){
            struct tcphdr *tcpPacket;
            tcpPacket=(struct tcphdr*)(buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));

            if (ntohs(tcpPacket->th_dport) == strtol(portNumberDst, 0, 10)){
                return 0;
            }
            else{
                return 1;
            }
        }
        else{
            struct udphdr *udpPacket;
            udpPacket=(struct udphdr*)(buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));

            if (ntohs(udpPacket->dest) == strtol(portNumberDst, 0, 10)){
                return 0;
            }
            else{
                return 1;
            }
        }
    }
}

int FilterSrcAddr(int flagExecute, unsigned char *buffer, int size, char *ipSrcAdrr){
    if (flagExecute == 0){
        return 0;
    }
    else{
        struct iphdr *ipPacket;
        ipPacket=(struct iphdr*)(buffer + sizeof(struct ethhdr));

        struct sockaddr_in sourceSocketAddress;
        memset(&sourceSocketAddress, '\0', sizeof(struct sockaddr_in));
        sourceSocketAddress.sin_addr.s_addr=ipPacket->saddr;

        if (strcmp(ipSrcAdrr, inet_ntoa(sourceSocketAddress.sin_addr))==0){
            return 0;
        }
        else{
            return 1;
        }
    }
}

int FilterDstAddr(int flagExecute, unsigned char *buffer, int size, char *ipDstAdrr){
    if (flagExecute == 0){
        return 0;
    }
    else{
        struct iphdr *ipPacket;
        ipPacket=(struct iphdr*)(buffer + sizeof(struct ethhdr));

        struct sockaddr_in destSocketAddress;
        memset(&destSocketAddress, '\0', sizeof(struct sockaddr_in));
        destSocketAddress.sin_addr.s_addr=ipPacket->saddr;

        if (strcmp(ipDstAdrr, inet_ntoa(destSocketAddress.sin_addr))==0){
            return 0;
        }
        else{
            return 1;
        }
    }
}

int FilterSize(int flagExecute, unsigned char *buffer, int size, int sizeOfPacket, int flag){
    if (flagExecute == 0){
        return 0;
    }
    else{
        struct iphdr *ipPacket;
        ipPacket=(struct iphdr*)(buffer + sizeof(struct ethhdr));

        if (flag==1){//less than
            if (ipPacket->tot_len <= sizeOfPacket){
                return 0;
            }
            else{
                return 1;
            }
        }
        else{
            if (ipPacket->tot_len >= sizeOfPacket){
                return 0;
            }
            else{
                return 1;
            }
        }
    }
}

int FilterEthernet(int flagExecute, int valueOfSocket, char *nameEthernet){
    if (flagExecute == 0){
        return 0;
    }
    else{
        int valueOfSetSocketOpt;
        valueOfSetSocketOpt=setsockopt(valueOfSocket, SOL_SOCKET, SO_BINDTODEVICE, nameEthernet, strlen(nameEthernet));
        if (valueOfSetSocketOpt < 0){
            perror("");
            return -1;
        }
        else{
            return 1;
        }
    }
}

void WriteFile(int flagExecute, char *nameFile, unsigned char *buffer, int size){
    if (flagExecute == 0){
        return;
    }
    else{
        FILE *file;
        file=fopen(nameFile, "a+");
        if (file==NULL){
            perror("file:");
            return;
        }
        struct iphdr *ipPacket;
        ipPacket = (struct iphdr*)(buffer + sizeof(struct ethhdr));
        struct sockaddr_in sourceSocketAddress;
        struct sockaddr_in destSocketAddress;

        memset(&sourceSocketAddress, '\0', sizeof(struct sockaddr_in));
        memset(&destSocketAddress, '\0', sizeof(struct sockaddr_in));

        sourceSocketAddress.sin_addr.s_addr=ipPacket->saddr;
        destSocketAddress.sin_addr.s_addr=ipPacket->daddr;

        time_t now=time(0);
        char *timeLocal=ctime(&now);
        fprintf(file, "\n\n\n");
        fprintf(file, "--------------------------------------\n");
        fprintf(file, "%s", timeLocal);
        fprintf(file, "--------------------------------------\n");
        fprintf(file, "Packet size: %d\n", ipPacket->tot_len);
        fprintf(file, "Source address: %s\n", inet_ntoa(sourceSocketAddress.sin_addr));
        fprintf(file, "Destination address: %s\n", inet_ntoa(destSocketAddress.sin_addr));
        fprintf(file, "Protocol: %d\n", ipPacket->protocol);
        fprintf(file, "ToS: %d\n",ipPacket->tos);
        fprintf(file, "Version: %d\n", ipPacket->version);
        fprintf(file, "--------------------------------------\n");
        fclose(file);
    }
}

void ShowData(int valueOfSocket){
    int packetSize, check;
    struct iphdr *ipPacket;
    unsigned char *buffer = (unsigned char *)malloc(50000);
    memset(buffer, '\0', 50000);

    ipPacket = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in sourceSocketAddress;
    struct sockaddr_in destSocketAddress;

    while(1) {
        packetSize = recvfrom(valueOfSocket, buffer, 50000, 0, NULL, NULL);
        memset(&sourceSocketAddress, '\0', sizeof(struct sockaddr_in));
        memset(&destSocketAddress, '\0', sizeof(struct sockaddr_in));

        sourceSocketAddress.sin_addr.s_addr=ipPacket->saddr;
        destSocketAddress.sin_addr.s_addr=ipPacket->daddr;

        if (packetSize != -1){

            FilterEthernet(arrayExecute[0].flagExecute, valueOfSocket, arrayExecute[0].valueName);

            check=FilterProtocol(arrayExecute[1].flagExecute, &buffer, sizeof(buffer), GetProtocol(arrayExecute[1].valueName));
            if (check==1){
                continue;
            }

            check=FilterPortSrc(arrayExecute[2].flagExecute, buffer, sizeof(buffer),arrayExecute[2].valueName, GetProtocol(arrayExecute[1].valueName));
            if (check==1){
                continue;
            }

            check=FilterPortDst(arrayExecute[3].flagExecute, buffer, sizeof(buffer),arrayExecute[3].valueName, GetProtocol(arrayExecute[1].valueName));
            if (check==1){
                continue;
            }

            check=FilterSrcAddr(arrayExecute[4].flagExecute, buffer, sizeof(buffer), arrayExecute[4].valueName);
            if (check==1){
                continue;
            }

            check=FilterDstAddr(arrayExecute[5].flagExecute, buffer, sizeof(buffer), arrayExecute[5].valueName);
            if (check==1){
                continue;
            }

            WriteFile(arrayExecute[6].flagExecute, arrayExecute[6].valueName, buffer, sizeof(buffer));


            check=FilterSize(arrayExecute[7].flagExecute, buffer, sizeof(buffer), strtol(arrayExecute[7].valueName, 0, 10), 1);
            if (check==1){
                continue;
            }

            time_t now=time(0);
            char *timeLocal=ctime(&now);
            printf("\n\n\n");
            printf("--------------------------------------\n");
            printf("%s", timeLocal);
            printf("--------------------------------------\n");
            printf("Packet size: %d\n", ipPacket->tot_len);
            printf("Source address: %s\n", inet_ntoa(sourceSocketAddress.sin_addr));
            printf("Destination address: %s\n", inet_ntoa(destSocketAddress.sin_addr));
            printf("Protocol: %d\n", ipPacket->protocol);
            printf("ToS: %d\n",ipPacket->tos);
            printf("Version: %d\n", ipPacket->version);
            printf("--------------------------------------\n");
        }
        else{
            perror("");
            break;
        }
    }
}

int main(int agrc, char *agrv[]) {
    int valueOfSocket;
    int valueReturnOfSetSocketOpt;

    valueOfSocket = socket (PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if(valueOfSocket == -1)
    {
        perror("Failed to create socket");
        close(valueOfSocket);
        return -1;
    }

    InitialParameterExecute();
    SetParameterExecute(agrc, agrv);

    if (agrc == 1){
        ShowData(valueOfSocket);
    }
    else{
        if (agrc == 3 && strcmp("-eth", agrv[1])==0){
            valueReturnOfSetSocketOpt=FilterEthernet(1, valueOfSocket, agrv[2]);
            if (valueReturnOfSetSocketOpt < 0){
                close(valueOfSocket);
            }
            else{
                ShowData(valueOfSocket);
            }
        }
        else{
            valueReturnOfSetSocketOpt=FilterEthernet(1, valueOfSocket, agrv[2]);
            if (valueReturnOfSetSocketOpt < 0){
                exit(1);
            }
            else{
                ShowData(valueOfSocket);
            }
            ShowData(valueOfSocket);
        }
    }
    close(valueOfSocket);
    return 0;
}
