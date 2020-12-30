#ifndef PROCESS_PACKET_HEADER

    #define PROCESS_PACKET_HEADER

    #include <queue>
    #include "packet.h"

    using namespace std;
    
    void update(Packet* newPacket=NULL, Packet* oldPacket=NULL);
    void processPacket(Packet& packet, int& windowSize);

#endif