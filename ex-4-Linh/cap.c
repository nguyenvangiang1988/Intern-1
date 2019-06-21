#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>

#define ETH_PROTOCOL_IP4 0x0800

#define IP_PROTOCOL_TCP 6
#define IP_PROTOCOL_ICMP 1
#define IP_PROTOCOL_IGMP 2
#define IP_PROTOCOL_UDP 17

#define MAX_FRAME_SIZE 100000

char* IPToString(void* i){
    unsigned char* ip = (unsigned char*)i;
    char* str = (char*)malloc(16);
    
    sprintf(str, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    
    return str;
}

char* MACToString(void* m){
    unsigned char* mac = (unsigned char*)m;
    char* str = malloc(18);
    
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    return str;
}

char* EtherGetProtocolString(unsigned short type){
    type = ntohs(type);
    switch (type) {
        case 0x0800:
            return "IPv4";
        case 0x0806:
            return "ARP";
        case 0x0842:
            return "Wake on LAN";
        case 0x22f3:
            return "IETF TRILL Protocol";
        case 0x22ea:
            return "Stream Reservation Protocol";
        case 0x6002:
            return "DEC MOP RC";
        case 0x6003:
            return "DECnet Pharse IV, DNA Routing";
        case 0x8035:
            return "RARP";
        case 0x809b:
            return "AppleTalk (Ethertalk)";
        case 0x80f3:
            return "AppleTalk Address Resolution Protocol (AARP)";
        case 0x8100:
            return "VLAN-tagged frame (IEEE 802.1Q) and Shortest Path Bridging IEEE 802.1aq with NNI compatibility";
        case 0x8102:
            return "Simple Loop Prevention Protocol (SLPP)";
        case 0x8137:
            return "IPX";
        case 0x8204:
            return "QNX Qnet";
        case 0x86dd:
            return "IPv6";
        case 0x8808:
            return "Ethernet flow control";
        case 0x8809:
            return "Ethernet Slow Protocols[11] such as the Link Aggregation Control Protocol";
        case 0x8819:
            return "CobraNet";
        case 0x8847:
            return "MPLS unicast";
        case 0x8848:
            return "MPLS multicast";
        case 0x8863:
            return "PPPoE Discovery Stage";
        case 0x8864:
            return "PPPoE Session Stage";
        case 0x886d:
            return "Intel Advanced Networking Services";
        case 0x8870:
            return "Jumbo Frames (Obsoleted draft-ietf-isis-ext-eth-01)";
        case 0x887b:
            return "HomePlug 1.0 MME";
        case 0x888e:
            return "EAP over LAN (IEEE 802.1X)";
        case 0x8892:
            return "PROFINET Protocol";
        case 0x889a:
            return "HyperSCSI (SCSI over Ethernet)";
        case 0x88a2:
            return "ATA over Ethernet";
        case 0x88a4:
            return "EtherCAT Protocol";
        case 0x88a8:
            return "Provider Bridging (IEEE 802.1ad) & Shortest Path Bridging IEEE 802.1aq";
        case 0x88ab:
            return "Ethernet Powerlink";
        case 0x88b8:
            return "GOOSE (Generic Object Oriented Substation event)";
        case 0x88b9:
            return "GSE (Generic Substation Events) Management Services";
        case 0x88ba:
            return "SV (Sampled Value Transmission)";
        case 0x88cc:
            return "Link Layer Discovery Protocol (LLDP)";
        case 0x88cd:
            return "SERCOS III";
        case 0x88dc:
            return "WSMP, WAVE Short Message Protocol";
        case 0x88e1:
            return "HomePlug AV MME";
        case 0x88e3:
            return "Media Redundancy Protocol (IEC62439-2)";
        case 0x88e5:
            return "MAC security (IEEE 802.1AE)";
        case 0x88e7:
            return "Provider Backbone Bridges (PBB) (IEEE 802.1ah)";
        case 0x88f7:
            return "Precision Time Protocol (PTP) over Ethernet (IEEE 1588)";
        case 0x88f8:
            return "NC-SI";
        case 0x88fb:
            return "Parallel Redundancy Protocol (PRP)";
        case 0x8902:
            return "IEEE 802.1ag Connectivity Fault Management (CFM) Protocol / ITU-T Recommendation Y.1731 (OAM)";
        case 0x8906:
            return "Fibre Channel over Ethernet (FCoE)";
        case 0x8914:
            return "FCoE Initialization Protocol";
        case 0x8915:
            return "RDMA over Converged Ethernet (RoCE)";
        case 0x891d:
            return "TTEthernet Protocol Control Frame (TTE)";
        case 0x892f:
            return "High-availability Seamless Redundancy (HSR)";
        case 0x9000:
            return "Ethernet Configuration Testing Protocol";
        case 0x9100:
            return "VLAN-tagged (IEEE 802.1Q) frame with double tagging";
        case 0xCAFE:
            return "Veritas Technologies Low Latency Transport (LLT)";
        default:
            return "Unknown Protocol";
    }
}

char* IPGetProtocolString(unsigned char type){
    switch (type) {
        case IP_PROTOCOL_ICMP:
            return "ICMP";
        case IP_PROTOCOL_IGMP:
            return "IGMP";
        case IP_PROTOCOL_TCP:
            return "TCP";
        case IP_PROTOCOL_UDP:
            return "UDP";
        default:
            return "Unknown Protocol";
    }
}

int RawSocketFilterInterface(int sk, char* ifn){
    struct ifreq request;
    struct sockaddr_ll addr;
    
    if (strlen(ifn) >= IFNAMSIZ) {
        return -1;
    }
    
    strncpy(request.ifr_ifrn.ifrn_name, ifn, IFNAMSIZ);
    
    if (ioctl(sk, SIOCGIFINDEX, &request) == -1) {
        perror("Cannot get interface index");
        return -1;
    }
    
    addr.sll_family = PF_PACKET;
    addr.sll_ifindex = request.ifr_ifru.ifru_ivalue;
    addr.sll_protocol = htons(ETH_P_ALL);
    
    if (bind(sk, (struct sockaddr*)&addr, sizeof addr) == -1) {
        perror("Bind to filter interface failed");
        return -1;
    }
    
    return 0;
}

void PrintHexData(void* d, unsigned length){
    int i = 0;
    int mark = -1;
    unsigned char* data = (unsigned char*)d;
    
    for (; i < length; i++) {
        printf("%02x ", (unsigned)data[i]);
        mark = (mark + 1) % 16;
        if (mark == 15) {
            printf("\n");
        }
    }
    
    printf("\n");
}

void PrintCharData(void* d, unsigned length){
    int i = 0;
    unsigned char* data = (unsigned char*)d;
    
    for (; i < length; i++) {
        printf("%c", (char)data[i]);
    }
    
    printf("\n");
}

int InStringArray(char** sarray, int length, char* str){
    int i = 0;
    for (; i < length; i++) {
        if (strcmp(sarray[i], str) == 0) {
            return 1;
        }
    }
    
    return 0;
}

int KeysFindOffset(char** keys, int length, char* str){
    int i = 0;
    for(; i < length; i++) {
        if (strcmp(keys[i], str) == 0) {
            return i;
        }
    }
    
    return -1;
}

int main(int argsTotal, char** args) {
    unsigned char frame[MAX_FRAME_SIZE];
    int sk;
    int rbytes;
    char *srcip, *destip;
    char *srcmac, *destmac;
    
    struct iphdr* ip4header;
    struct ethhdr* ethHeader;
    struct tcphdr* tcpHeader;
    struct udphdr* udpHeader;
    struct icmphdr* icmpHeader;
    
    int length = 0;
    unsigned int index = 1;
    int i;
    
    int keyOffset;
    int kvpLength = argsTotal / 2;
    char* argsStringArray[] = {
        "-i", //interface
        "-p", //protocol: IP, ICMP, TCP, UDP
        "-src", //source IP
        "-dest" //destination IP
    };
    char **keys, **values;
    
    
    if (argsTotal % 2 == 0) {
        printf("Invalid argument!\n");
        return 0;
    }
    
    if (kvpLength) {
        keys = (char**)malloc(sizeof(char*) * kvpLength);
        values = (char**)malloc(sizeof(char*) * kvpLength);

        for (i = 0; i < kvpLength; i++) {
            keys[i] = args[i * 2 + 1];
            values[i] = args[i * 2 + 2];
            if (!InStringArray(argsStringArray, sizeof argsStringArray / sizeof(char*), keys[i])) {
                printf("Invalid parameter: %s\n", keys[i]);
                return 0;
            }
        }
    }
    
    //Create socket for listen all packet from ETHERNET
    if (sk = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)), sk == -1) {
        perror("create socket failed");
        return 0;
    }
    
    if (keyOffset = KeysFindOffset(keys, kvpLength, "-i"), keyOffset != -1) {
        if (RawSocketFilterInterface(sk, values[keyOffset]) == -1) {
            return 0;
        }
        else {
            printf("Listening %s\n", values[keyOffset]);
        }
    }
    
    while (rbytes = read(sk, frame, MAX_FRAME_SIZE), rbytes > 0) {
        ethHeader = (struct ethhdr*)frame;
        srcmac = MACToString(ethHeader->h_source);
        destmac = MACToString(ethHeader->h_dest);
        
        printf("#%u\n", index++);
        printf("********************************************************************\n");
        printf("Ethernet Header\n");
        printf("\t\t|%-30s: %s\n", "Source MAC" ,srcmac);
        printf("\t\t|%-30s: %s\n", "Destination MAC", destmac);
        printf("\t\t|%-30s: %s\n", "Protocol", EtherGetProtocolString(ethHeader->h_proto));
        printf("\t\t|%-30s: %d bytes\n", "Length", rbytes);
        printf("\n");
        
        if (htons(ethHeader->h_proto) == ETH_PROTOCOL_IP4) {
            //Common IPv4 Header
            ip4header = (struct iphdr*)(frame + sizeof(struct ethhdr));
            srcip = IPToString(&ip4header->saddr);
            destip = IPToString(&ip4header->daddr);
            
            printf("IP Header\n");
            printf("\t\t|%-30s: %u\n", "Version", ip4header->version);
            printf("\t\t|%-30s: %u bytes\n", "Internet Header Length", ip4header->ihl * 4);
            printf("\t\t|%-30s: %u\n", "Type of Service", (unsigned)ip4header->tos);
            printf("\t\t|%-30s: %u bytes\n", "Total Length", (unsigned)ntohs(ip4header->tot_len));
            printf("\t\t|%-30s: %u\n", "ID of fragment", (unsigned)ntohs(ip4header->id));
            printf("\t\t|%-30s: %d\n", "DF (Do Not Fragment bit)", ntohs(ip4header->frag_off) & 0x4000 ? 1 : 0);
            printf("\t\t|%-30s: %d\n", "MF (More Fragments bit)", ntohs(ip4header->frag_off) & 0x2000 ? 1 : 0);
            printf("\t\t|%-30s: %u\n", "Fragment Offset", (unsigned)ntohs(ip4header->frag_off) & 0x1fff);
            printf("\t\t|%-30s: %u\n", "TTL", (unsigned)ip4header->ttl);
            printf("\t\t|%-30s: %s\n", "Protocol", IPGetProtocolString(ip4header->protocol));
            printf("\t\t|%-30s: 0x%04x\n", "Header checksum", ntohs(ip4header->check));
            printf("\t\t|%-30s: %s\n", "Source IP", srcip);
            printf("\t\t|%-30s: %s\n", "Destination IP", destip);
            
            if (ip4header->protocol == IP_PROTOCOL_TCP) {
                //TCP Header
                tcpHeader = (struct tcphdr*)(frame + sizeof(struct ethhdr) + ip4header->ihl * 4);
                printf("TCP Header\n");
                printf("\t\t|%-30s: %u\n", "Source Port", ntohs(tcpHeader->source));
                printf("\t\t|%-30s: %u\n", "Destination Port", ntohs(tcpHeader->dest));
                printf("\t\t|%-30s: %u\n", "Sequence number", ntohl(tcpHeader->seq));
                printf("\t\t|%-30s: %u\n", "ACK number", ntohl(tcpHeader->ack_seq));
                printf("\t\t|%-30s: %u\n", "Header length", tcpHeader->doff << 2);
                printf("\t\t\t\t|%s: %s\n", "URG", tcpHeader->urg ? "Set" : "Not Set");
                printf("\t\t\t\t|%s: %s\n", "ACK", tcpHeader->ack ? "Set" : "Not Set");
                printf("\t\t\t\t|%s: %s\n", "PSH", tcpHeader->psh ? "Set" : "Not Set");
                printf("\t\t\t\t|%s: %s\n", "RST", tcpHeader->rst ? "Set" : "Not Set");
                printf("\t\t\t\t|%s: %s\n", "SYN", tcpHeader->syn ? "Set" : "Not Set");
                printf("\t\t\t\t|%s: %s\n", "FIN", tcpHeader->fin ? "Set" : "Not Set");
                
                printf("\t\t|%-30s: %u\n", "Window", ntohs(tcpHeader->window));
                printf("\t\t|%-30s: 0x%x\n", "Checksum", ntohs(tcpHeader->check));
                printf("\t\t|%-30s: %u\n", "URG Pointer", ntohs(tcpHeader->urg_ptr));
                
                length = ntohs(ip4header->tot_len) - ip4header->ihl * 4 - tcpHeader->doff * 4;
                printf("Data: %u bytes\n", length);
                PrintHexData(frame + sizeof(struct ethhdr) + ip4header->ihl * 4 + tcpHeader->doff * 4, length);
                PrintCharData(frame + sizeof(struct ethhdr) + ip4header->ihl * 4 + tcpHeader->doff * 4, length);
            }
            
            if (ip4header->protocol == IP_PROTOCOL_UDP) {
                //UDP Header
                udpHeader = (struct udphdr*)(frame + sizeof(struct ethhdr) + ip4header->ihl * 4);
                printf("UDP Header\n");
                printf("\t\t|%-30s: %u\n", "Source Port", ntohs(udpHeader->source));
                printf("\t\t|%-30s: %u\n", "Destination Port", ntohs(udpHeader->dest));
                printf("\t\t|%-30s: %u\n", "Length", ntohs(udpHeader->len));
                printf("\t\t|%-30s: 0x%u\n", "Checksum", ntohs(udpHeader->check));
                
                length = ntohs(udpHeader->len) - sizeof(struct udphdr);
                printf("Data: %u bytes\n", length);
                PrintHexData(frame + sizeof(struct ethhdr) + ip4header->ihl * 4 + sizeof(struct udphdr), length);
                PrintCharData(frame + sizeof(struct ethhdr) + ip4header->ihl * 4 + sizeof(struct udphdr), length);
            }
            
            if (ip4header->protocol == IP_PROTOCOL_ICMP) {
                //ICMP Header
                icmpHeader = (struct icmphdr*)(frame + sizeof(struct ethhdr) + ip4header->ihl * 4);
                printf("ICMP Header\n");
                printf("\t\t|%-30s: %d\n", "Type", icmpHeader->type);
                printf("\t\t|%-30s: %d\n", "Code", icmpHeader->code);
                printf("\t\t|%-30s: 0x%04x\n", "Checksum", ntohs(icmpHeader->checksum));
            }
        }
        
        printf("\n\n");
        
        free(srcmac);
        free(destmac);
    }
    
    printf("Finishing...");
    close(sk);
    
    return 0;
}
