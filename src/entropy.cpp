#include <queue>
#include <map>
#include <cmath>
#include "packet.h"

using namespace std;

/*
    getProbabilityMap():
        Get probability map of each IP.
*/
map<uint32_t, double> getProbabilityMap(queue<Packet> packetQueue, int& windowSize) {
    map<uint32_t, double> probabilityMap; 
    while (!packetQueue.empty()) {
        if (!probabilityMap.count(packetQueue.front().srcIP)) probabilityMap[packetQueue.front().srcIP] =  1.0 / windowSize;
        else                                                  probabilityMap[packetQueue.front().srcIP] += 1.0 / windowSize;
        packetQueue.pop();
    }
    return probabilityMap;
}
/*
    getEntropy():
        Get entropy of a given queue of packets.
*/
double getEntropy(queue<Packet>& packetQueue) {
    int                   windowSize     = packetQueue.size();
    map<uint32_t, double> probabilityMap = getProbabilityMap(packetQueue, windowSize);
    double                entropy        = 0;

    for (auto const& item : probabilityMap)
        entropy += item.second * log2(item.second);
    return entropy;
}