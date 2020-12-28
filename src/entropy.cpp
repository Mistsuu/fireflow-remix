#include <queue>
#include <map>
#include <cmath>
#include "packet.h"

using namespace std;

/*
    getProbabilityMap():
        Get probability map of each item.
*/
template<typename T>
void getProbabilityMap(map<T, double>& probabilityMap, queue<Packet> packetQueue, uint8_t& protocol) {
    int windowSize = packetQueue.size();
    while (!packetQueue.empty()) {
        if (packetQueue.front().protocol == protocol) {
            if (!probabilityMap.count(packetQueue.front().srcIP)) probabilityMap[packetQueue.front().srcIP] =  1.0 / windowSize;
            else                                                  probabilityMap[packetQueue.front().srcIP] += 1.0 / windowSize;
        }
        packetQueue.pop();
    }
}

/*
    getEntropy():
        Get entropy of a given queue of packets.
*/
double getEntropy(queue<Packet>& packetQueue, uint8_t protocol) {
    map<uint32_t, double> probabilityMap;
    getProbabilityMap(probabilityMap, packetQueue, protocol);

    double entropy = 0;
    for (auto const& item : probabilityMap)
        entropy += item.second * log2(item.second);
    return -entropy;
}