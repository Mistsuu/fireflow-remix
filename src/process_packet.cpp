#include <queue>
#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include "packet.h"
#include "detection.h"
#include "statistic.h"

using namespace std;

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

void update(Packet* newPacket=NULL, Packet* oldPacket=NULL) {
    if (newPacket && !oldPacket) {
        updateEntropyAndMap(srcIPCount,   srcIPEntropy,   &newPacket->srcIP,   (uint32_t*)NULL, windowSize);
        updateEntropyAndMap(srcPortCount, srcPortEntropy, &newPacket->srcPort, (uint16_t*)NULL, windowSize);
        updateEntropyAndMap(dstPortCount, dstPortEntropy, &newPacket->dstPort, (uint16_t*)NULL, windowSize);
    } else if (oldPacket) {
        updateEntropyAndMap(srcIPCount,   srcIPEntropy,   &newPacket->srcIP,   &oldPacket->srcIP,   windowSize);
        updateEntropyAndMap(srcPortCount, srcPortEntropy, &newPacket->srcPort, &oldPacket->srcPort, windowSize);
        updateEntropyAndMap(dstPortCount, dstPortEntropy, &newPacket->dstPort, &oldPacket->dstPort, windowSize);
    }
}

void processPacket(Packet& packet, int& _windowSize_) {
    packetQueue.push(packet);

    if (packetQueue.size() == 1) 
        windowSize = _windowSize_, update();
    else if (packetQueue.size() == windowSize+1) {
        update(&packet, &packetQueue.front());
        packetQueue.pop();
        performDetection(packetQueue);
    }
    else update(&packet);
}
