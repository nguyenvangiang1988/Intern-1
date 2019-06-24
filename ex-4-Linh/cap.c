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

#define ANY_CHAR '*'

int IsInteger(char* s){
    int i = -1;
    
    while (s[++i] != 0) {
        if (s[i] < '0' || s[i] > '9')
            return 0;
    }
    
    return 1;
}

int IsIPAddress(char* ip){
    unsigned int digit[4] = {0, 0, 0, 0};
    int i = -1;
    int length = 0;
    int newDigit = 1;
    
    if (ip[0] == '\0') {
        return 0;
    }
    
    while (ip[++i]) {
        if (ip[i] >= '0' && ip[i] <= '9') {
            digit[length] *= 10;
            digit[length] += ip[i] - '0';
            if (digit[length] > 255) {
                return 0;
            }
            newDigit = 0;
        }
        else if (ip[i] == '.') {
            if (newDigit) {
                return 0;
            }
            
            length++;
            newDigit = 1;
            
            if (length >= 4) {
                return 0;
            }
        }
        else {
            return 0;
        }
    }
    
    if (length == 3){
        return 1;
    }
    else {
        return 0;
    }
}

int CompareIPAddress(char* a, char* b){
    int ia = 0, ib = 0;
    
    while (a[ia] && b[ib]) {
        if (a[ia] == ANY_CHAR || b[ib] == ANY_CHAR) {
            if (a[ia] == ANY_CHAR && b[ib] == ANY_CHAR) {
                ia += 1, ib += 1;
            }
            else if (a[ia] == ANY_CHAR) {
                ia += 1;
                while (b[++ib] != '.');
            }
            else if (b[ib] == ANY_CHAR) {
                ib += 1;
                while (a[++ia] != '.');
            }
        }
        else {
            if (a[ia] != b[ib]) {
                return 1;
            }
            
            ia++, ib++;
        }
    }
    
    return 0;
}

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

//common structure 
struct Package {
    unsigned char* pPackage;
    unsigned short length;
};

//Ethernet Protocol
void EthernetShowInfo(struct Package* package){
    struct ethhdr* header = (struct ethhdr*)package->pPackage;
    char* srcMAC = MACToString(header->h_source);
    char* destMAC = MACToString(header->h_dest);
    
    printf("Ethernet Header\n");
    printf("\t\t|%-30s: %s\n", "Source MAC" ,srcMAC);
    printf("\t\t|%-30s: %s\n", "Destination MAC", destMAC);
    printf("\t\t|%-30s: %s\n", "Protocol", EtherGetProtocolString(header->h_proto));
    printf("\t\t|%-30s: %hu bytes\n", "Length", package->length);
    
    free(srcMAC);
    free(destMAC);
}

struct Package EthernetGetPayload(struct Package* package){
    struct Package result;
    
    result.length = package->length - sizeof(struct ethhdr);
    result.pPackage = package->pPackage + package->length - result.length;
    
    return result;
}

unsigned short EthernetGetProtocol(struct Package* package){
    struct ethhdr* header = (struct ethhdr*)package->pPackage;
    
    return ntohs(header->h_proto);
}

//IP Protocol
void IPShowInfo(struct Package* package){
    struct iphdr* header = (struct iphdr*)package->pPackage;
    char* srcIP = IPToString(&header->saddr);
    char* destIP = IPToString(&header->daddr);
    
    printf("IP Header\n");
    printf("\t\t|%-30s: %u\n", "Version", header->version);
    printf("\t\t|%-30s: %u bytes\n", "Internet Header Length", header->ihl * 4);
    printf("\t\t|%-30s: %hhu\n", "Type of Service", header->tos);
    printf("\t\t|%-30s: %hu bytes\n", "Total Length", ntohs(header->tot_len));
    printf("\t\t|%-30s: %hu\n", "ID of fragment", ntohs(header->id));
    printf("\t\t|%-30s: %d\n", "DF (Do Not Fragment bit)", ntohs(header->frag_off) & 0x4000 ? 1 : 0);
    printf("\t\t|%-30s: %d\n", "MF (More Fragments bit)", ntohs(header->frag_off) & 0x2000 ? 1 : 0);
    printf("\t\t|%-30s: %u\n", "Fragment Offset", ntohs(header->frag_off) & 0x1fff);
    printf("\t\t|%-30s: %hhu\n", "TTL", header->ttl);
    printf("\t\t|%-30s: %s\n", "Protocol", IPGetProtocolString(header->protocol));
    printf("\t\t|%-30s: 0x%04hx\n", "Header checksum", ntohs(header->check));
    printf("\t\t|%-30s: %s\n", "Source IP", srcIP);
    printf("\t\t|%-30s: %s\n", "Destination IP", destIP);
    
    free(srcIP);
    free(destIP);
}

struct Package IPGetPayload(struct Package* package){
    struct iphdr* header = (struct iphdr*)package->pPackage;
    struct Package result;
    
    result.length = package->length - (header->ihl << 2);
    result.pPackage = package->pPackage + package->length - result.length;
    //option
    //result.length = header->tot_len - (header->ihl << 2);
    return result;
}

unsigned char IPGetProtocol(struct Package* package){
    struct iphdr* header = (struct iphdr*)package->pPackage;
    
    return (header->protocol);
}

char* IPGetSourceIPString(struct Package* package){
    struct iphdr* header = (struct iphdr*)package->pPackage;
    
    return IPToString(&header->saddr);
}

char* IPGetDestinationString(struct Package* package){
    struct iphdr* header = (struct iphdr*)package->pPackage;
    
    return IPToString(&header->daddr);
}

//TCP Protocol
struct Package TCPGetPayload(struct Package* package){
    struct tcphdr* header = (struct tcphdr*)package->pPackage;
    struct Package result;
        
    result.length = package->length - (header->doff << 2);
    result.pPackage = package->pPackage + package->length - result.length;
    
    return result;
}

void TCPShowInfo(struct Package* package){
    struct tcphdr* header = (struct tcphdr*)package->pPackage;
    struct Package message = TCPGetPayload(package);
    
    printf("TCP Header\n");
    printf("\t\t|%-30s: %hu\n", "Source Port", ntohs(header->source));
    printf("\t\t|%-30s: %hu\n", "Destination Port", ntohs(header->dest));
    printf("\t\t|%-30s: %u\n", "Sequence number", ntohl(header->seq));
    printf("\t\t|%-30s: %u\n", "ACK number", ntohl(header->ack_seq));
    printf("\t\t|%-30s: %u\n", "Header length", header->doff << 2);
    printf("\t\t\t\t|%s: %s\n", "URG", header->urg ? "Set" : "Not Set");
    printf("\t\t\t\t|%s: %s\n", "ACK", header->ack ? "Set" : "Not Set");
    printf("\t\t\t\t|%s: %s\n", "PSH", header->psh ? "Set" : "Not Set");
    printf("\t\t\t\t|%s: %s\n", "RST", header->rst ? "Set" : "Not Set");
    printf("\t\t\t\t|%s: %s\n", "SYN", header->syn ? "Set" : "Not Set");
    printf("\t\t\t\t|%s: %s\n", "FIN", header->fin ? "Set" : "Not Set");
    printf("\t\t|%-30s: %hu\n", "Window", ntohs(header->window));
    printf("\t\t|%-30s: 0x%04hx\n", "Checksum", ntohs(header->check));
    printf("\t\t|%-30s: %hu\n", "URG Pointer", ntohs(header->urg_ptr));
    
    printf("Data: %hu bytes\n", message.length);
    PrintHexData(message.pPackage, message.length);
    printf("\n");
    PrintCharData(message.pPackage, message.length);
}

unsigned short TCPGetSourcePort(struct Package* package){
    struct tcphdr* header = (struct tcphdr*)package->pPackage;
    
    return ntohs(header->source);
}

unsigned short TCPGetDestinationPort(struct Package* package){
    struct tcphdr* header = (struct tcphdr*)package->pPackage;
    
    return ntohs(header->dest);
}

//UDP Protocol 
struct Package UDPGetPayload(struct Package* package){
    struct udphdr* header = (struct udphdr*)package->pPackage;
    struct Package result;
    
    result.length = package->length - sizeof(struct udphdr);
    result.pPackage = package->pPackage + package->length - result.length;
    
    return result;
}

void UDPShowInfo(struct Package* package){
    struct udphdr* header = (struct udphdr*)package->pPackage;
    struct Package message = UDPGetPayload(package);
    
    printf("UDP Header\n");
    printf("\t\t|%-30s: %hu\n", "Source Port", ntohs(header->source));
    printf("\t\t|%-30s: %hu\n", "Destination Port", ntohs(header->dest));
    printf("\t\t|%-30s: %hu\n", "Length", ntohs(header->len));
    printf("\t\t|%-30s: 0x%04hu\n", "Checksum", ntohs(header->check));
    
    printf("Data: %hu bytes\n", message.length);
    PrintHexData(message.pPackage, message.length);
    printf("\n");
    PrintCharData(message.pPackage, message.length);
}

unsigned short UDPGetSourcePort(struct Package* package){
    struct udphdr* header = (struct udphdr*)package->pPackage;
    
    return ntohs(header->source);
}

unsigned short UDPGetDestinationPort(struct Package* package){
    struct udphdr* header = (struct udphdr*)package->pPackage;
    
    return ntohs(header->dest);
}

//ICMP Protocol
void ICMPShowInfo(struct Package* package){
    struct icmphdr* header = (struct icmphdr*)package->pPackage;
    
    printf("ICMP Header\n");
    printf("\t\t|%-30s: %hhu\n", "Type", header->type);
    printf("\t\t|%-30s: %hhu\n", "Code", header->code);
    printf("\t\t|%-30s: 0x%04hx\n", "Checksum", ntohs(header->checksum));
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
    int argPairsLength = argsTotal / 2;
    char* argsStringArray[] = {
        "-i", //interface
        "-p", //protocol: IP, ICMP, TCP, UDP
        "-src", //source IP
        "-dest", //destination IP
        "-sp", //source Port
        "-dp", //destination Port
        "-minsize", //minimum size
        "-maxsize", //maximum size
    };
    char **argKeys, **argValues;
    char *supportedProtocolsStringArray[] = {
        "ICMP",
        "TCP",
        "UDP"
    };
    int bTransportProtocolFilter = 0;
    unsigned char nTransportProtocolFilter;
    int bTransportSrcPortFilter = 0, bTransportDestPortFilter = 0;
    unsigned short nTransportSrcPort, nTransportDestPort;
    int bIPSourceAddressFilter = 0, bIPDestinationAddressFilter = 0;
    char *sIPSourceAddressFilter, *sIPDestinationAddressFilter;
    unsigned int minsize = 0, maxsize = MAX_FRAME_SIZE;
    
    struct Package ethPackage, ipPackage, icmpPackage, tcpPackage, udpPackage;
    char* ipTmp;
    
    if (argsTotal % 2 == 0) {
        printf("Invalid argument!\n");
        return 0;
    }
    
    if (argPairsLength) {
        argKeys = (char**)malloc(sizeof(char*) * argPairsLength);
        argValues = (char**)malloc(sizeof(char*) * argPairsLength);

        for (i = 0; i < argPairsLength; i++) {
            argKeys[i] = args[i * 2 + 1];
            argValues[i] = args[i * 2 + 2];
            if (!InStringArray(argsStringArray, sizeof argsStringArray / sizeof(char*), argKeys[i])) {
                printf("Invalid parameter: %s\n", argKeys[i]);
                return 0;
            }
        }
    }
    
    //Create socket for listen all packet from ETHERNET
    if (sk = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)), sk == -1) {
        perror("create socket failed");
        return 0;
    }
    
    //Filter by interface
    if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-i"), keyOffset != -1) {
        if (RawSocketFilterInterface(sk, argValues[keyOffset]) == -1) {
            return 0;
        }
        else {
            printf("Listening %s\n", argValues[keyOffset]);
        }
    }
    
    //Create Source and Destination IP Filter
    if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-src"), keyOffset != -1) {
        bIPSourceAddressFilter = 1;
        sIPSourceAddressFilter = argValues[keyOffset];
    }
    
    if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-dest"), keyOffset != -1) {
        bIPDestinationAddressFilter = 1;
        sIPDestinationAddressFilter = argValues[keyOffset];
    }
    
    //Create Protocol Filter
    if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-p"), keyOffset != -1) {
        if (InStringArray(supportedProtocolsStringArray, sizeof supportedProtocolsStringArray / sizeof(char*), argValues[keyOffset])) {
            bTransportProtocolFilter = 1;
            
            if (strcmp(argValues[keyOffset], "TCP") == 0) {
                nTransportProtocolFilter = IP_PROTOCOL_TCP;
            }
            else if (strcmp(argValues[keyOffset], "UDP") == 0) {
                nTransportProtocolFilter = IP_PROTOCOL_UDP;
            }
            else if (strcmp(argValues[keyOffset], "ICMP") == 0) {
                nTransportProtocolFilter = IP_PROTOCOL_ICMP;
            }
            
            if (nTransportProtocolFilter == IP_PROTOCOL_TCP || nTransportProtocolFilter == IP_PROTOCOL_UDP) {
                if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-sp"), keyOffset != -1) {
                    if (!IsInteger(argValues[keyOffset])) {
                        printf("Source Port invalid!\n");
                        return 0;
                    }
                    bTransportSrcPortFilter = 1;
                    nTransportSrcPort = atoi(argValues[keyOffset]);
                }
                
                if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-dp"), keyOffset != -1) {
                    if (!IsInteger(argValues[keyOffset])) {
                        printf("Destination Port invalid!\n");
                        return 0;
                    }
                    bTransportDestPortFilter = 1;
                    nTransportDestPort = atoi(argValues[keyOffset]);
                }
            }
        }
        else {
            printf("Unsupported protocol to filter\n");
            return 0;
        }
    }
    
    //Create Size Filter
    if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-minsize"), keyOffset != -1) {
        if (!IsInteger(argValues[keyOffset])) {
            printf("Minimum size invalid\n");
            return 0;
        }
        
        minsize = atoi(argValues[keyOffset]);
    }
    
    if (keyOffset = KeysFindOffset(argKeys, argPairsLength, "-maxsize"), keyOffset != -1) {
        if (!IsInteger(argValues[keyOffset])) {
            printf("Minimum size invalid\n");
            return 0;
        }
        
        maxsize = atoi(argValues[keyOffset]);
    }
    
    if (bIPSourceAddressFilter) {
        printf("Filter by Source IP: %s\n", sIPSourceAddressFilter);
    }
    
    if (bIPDestinationAddressFilter) {
        printf("Filter by Source IP: %s\n", sIPDestinationAddressFilter);
    }
    
    if (bTransportProtocolFilter) {
        printf("Filter by Protocol code: %hhu\n", nTransportProtocolFilter);
    }
    
    if (bTransportSrcPortFilter) {
        printf("Filter by Source Port: %hu\n", nTransportSrcPort);
    }
    
    if (bTransportDestPortFilter) {
        printf("Filter by Destination Port: %hu\n", nTransportDestPort);
    }
    
    while (rbytes = read(sk, frame, MAX_FRAME_SIZE), rbytes > 0) {
        if (rbytes >= minsize && rbytes <= maxsize) {
            ethPackage.pPackage = frame;
            ethPackage.length = rbytes;
            
            if (EthernetGetProtocol(&ethPackage) == ETH_PROTOCOL_IP4) {
                ipPackage = EthernetGetPayload(&ethPackage);
                
                if (IPGetProtocol(&ipPackage) == IP_PROTOCOL_ICMP) {
                    icmpPackage = IPGetPayload(&ipPackage);
                }
                else if (IPGetProtocol(&ipPackage) == IP_PROTOCOL_TCP) {
                    tcpPackage = IPGetPayload(&ipPackage);
                }
                else if (IPGetProtocol(&ipPackage) == IP_PROTOCOL_UDP) {
                    udpPackage = IPGetPayload(&ipPackage);
                }
            }
            
            if (bIPSourceAddressFilter || bIPDestinationAddressFilter || bTransportProtocolFilter || bTransportSrcPortFilter || bTransportDestPortFilter) {
                //have filter in any here, then it is IP protocol
                if (EthernetGetProtocol(&ethPackage) == ETH_PROTOCOL_IP4) {
                    if (bIPSourceAddressFilter) {
                        ipTmp = IPGetSourceIPString(&ipPackage);
                        
                        if (CompareIPAddress(ipTmp, sIPSourceAddressFilter)) {
                            free(ipTmp);
                            continue;
                        }
                    }
                    
                    if (bIPDestinationAddressFilter) {
                        ipTmp = IPGetDestinationString(&ipPackage);
                        
                        if (CompareIPAddress(ipTmp, sIPDestinationAddressFilter)) {
                            free(ipTmp);
                            continue;
                        }
                    }
                    
                    if (bTransportProtocolFilter) {
                        if (IPGetProtocol(&ipPackage) != nTransportProtocolFilter) {
                            continue;
                        }
                    }
                    
                    if (bTransportSrcPortFilter) {
                        if (TCPGetSourcePort(&tcpPackage) != nTransportSrcPort) {
                            continue;
                        }
                    }
                    
                    if (bTransportDestPortFilter) {
                        if (TCPGetDestinationPort(&tcpPackage) != nTransportDestPort) {
                            continue;
                        }
                    }
                }
                else {
                    continue;
                }
            }
            
            printf("#%u\n", index++);
            printf("********************************************************************\n");

            EthernetShowInfo(&ethPackage);
            printf("\n");
            
            if (EthernetGetProtocol(&ethPackage) == ETH_PROTOCOL_IP4) {
                IPShowInfo(&ipPackage);
                printf("\n");
                
                if (IPGetProtocol(&ipPackage) == IP_PROTOCOL_ICMP) {
                    ICMPShowInfo(&icmpPackage);
                    printf("\n");
                }
                else if (IPGetProtocol(&ipPackage) == IP_PROTOCOL_TCP) {
                    TCPShowInfo(&tcpPackage);
                    printf("\n");
                }
                else if (IPGetProtocol(&ipPackage) == IP_PROTOCOL_UDP) {
                    UDPShowInfo(&udpPackage);
                    printf("\n");
                }
            }

            printf("\n");
        }
    }
    
    printf("Finishing...");
    close(sk);
    
    return 0;
}
