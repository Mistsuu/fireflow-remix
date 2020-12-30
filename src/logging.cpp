#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TCP_FIN_FLAG_SHIFT 1
#define TCP_SYN_FLAG_SHIFT 2
#define TCP_RST_FLAG_SHIFT 3
#define TCP_PSH_FLAG_SHIFT 4
#define TCP_ACK_FLAG_SHIFT 5
#define TCP_URG_FLAG_SHIFT 6

using namespace std;

int extractBitValue(uint8_t& num, int bit) {
    if (bit > 0 && bit <= 8) return ((num >> (bit - 1)) & 1);
    else                     return 0;
}

string getStringIP(uint32_t& intIP) {
    return 
        to_string((intIP)       & 0xff) + "." +
        to_string((intIP >> 8)  & 0xff) + "." +
        to_string((intIP >> 16) & 0xff) + "." +
        to_string((intIP >> 24) & 0xff);
}

string getProtocol(uint8_t& protocol) {
    if      (protocol == IPPROTO_ICMP) return "ICMP";
    else if (protocol == IPPROTO_TCP)  return "TCP";
    else if (protocol == IPPROTO_UDP)  return "UDP";
    else                               return "-";
}

string getFlags(uint8_t& flags) {
    if (flags == 0)
        return "-";

    //  0    0    0    0    0    0    0    0
    // CWR	ECE	 URG  ACK  PSH	RST	 SYN  FIN
    string flagsStr = "";
    if (extractBitValue(flags, TCP_FIN_FLAG_SHIFT)) flagsStr += "FIN,";
    if (extractBitValue(flags, TCP_SYN_FLAG_SHIFT)) flagsStr += "SYN,";
    if (extractBitValue(flags, TCP_RST_FLAG_SHIFT)) flagsStr += "RST,";
    if (extractBitValue(flags, TCP_PSH_FLAG_SHIFT)) flagsStr += "PSH,";
    if (extractBitValue(flags, TCP_ACK_FLAG_SHIFT)) flagsStr += "ACK,";
    if (extractBitValue(flags, TCP_URG_FLAG_SHIFT)) flagsStr += "URG,";
    flagsStr.pop_back();

    return flagsStr;
}