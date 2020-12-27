#ifndef ENTROPY_HEADER

    #define ENTROPY_HEADER

    #include <queue>
    #include <map>
    #include <cmath>
    #include "packet.h"

    using namespace std;

    map<uint32_t, double> getProbabilityMap(queue<Packet> packetQueue, int& windowSize);
    double                getEntropy(queue<Packet>& packetQueue);

#endif