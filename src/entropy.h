#ifndef ENTROPY_HEADER

    #define ENTROPY_HEADER

    #include <queue>
    #include <map>
    #include <cmath>
    #include "packet.h"

    using namespace std;

    
    template<typename T> void   getProbabilityMapICMP(map<T, double>& probabilityMap, queue<Packet> packetQueue, uint8_t& protocol);
                         double getEntropy(queue<Packet>& packetQueue, uint8_t protocol);

#endif