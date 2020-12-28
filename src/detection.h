#ifndef DETECTION_HEADER

    #define DETECTION_HEADER

    #include <queue>
    #include "packet.h"

    using namespace std;

    void performDetection(queue<Packet> packetQueue);
    void ICMPDetection(queue<Packet>& packetQueue);
    void UDPDetection(queue<Packet>& packetQueue);
    void SYNDetection(queue<Packet>& packetQueue);

#endif