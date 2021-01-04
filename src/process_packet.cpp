#include <queue>
#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "packet.h"
#include "detection.h"
#include "statistic.h"

using namespace std;

////////////////// HANDLE NEW PACKET ///////////////////////////

// Maps that counts the occurance of certain attributes.
map<uint32_t, int> srcIPCount;
map<uint16_t, int> srcPortCount;
map<uint16_t, int> dstPortCount;

// Queue of packets.
queue<Packet> packetQueue;
int           windowSize;

// Current entropy of the queue of the packets.
double srcIPEntropy   = 0;
double srcPortEntropy = 0;
double dstPortEntropy = 0;

void processPacket(Packet& packet, int& _windowSize_) {
    // Set window size first...
    if (packetQueue.empty())
        windowSize   = _windowSize_,
        srcIPEntropy = 0,
        srcPortEntropy = 0,
        dstPortEntropy = 0;

    packetQueue.push(packet);
    if (packetQueue.size() <= windowSize) {
        update(srcIPCount,   srcIPEntropy,   &packet.srcIP,   (uint32_t*)NULL,              windowSize);
        update(srcPortCount, srcPortEntropy, &packet.srcPort, (uint16_t*)NULL,              windowSize);
        update(dstPortCount, dstPortEntropy, &packet.dstPort, (uint16_t*)NULL,              windowSize);
    }
    else {
        update(srcIPCount,   srcIPEntropy,   &packet.srcIP,   &packetQueue.front().srcIP,   windowSize);
        update(srcPortCount, srcPortEntropy, &packet.srcPort, &packetQueue.front().srcPort, windowSize);
        update(dstPortCount, dstPortEntropy, &packet.dstPort, &packetQueue.front().dstPort, windowSize);

        packetQueue.pop();
        performDetection(packetQueue);
    }

    cerr << packetQueue.size() - 1 << " " << srcIPEntropy << endl;
}


////////////////////////// CONVERSION ////////////////////////////////////

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
