#ifndef PROCESS_PACKET_HEADER

    #define PROCESS_PACKET_HEADER

    #include "packet.h"

    using namespace std;
    
    void   update(Packet* newPacket=NULL, Packet* oldPacket=NULL);
    void   processPacket(Packet& packet, int& windowSize);
    int    extractBitValue(uint8_t& num, int bit);
    string getStringIP(uint32_t& intIP);
    string getProtocol(uint8_t& protocol);
    string getFlags(uint8_t& flags);

#endif