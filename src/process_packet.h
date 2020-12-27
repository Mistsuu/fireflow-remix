#ifndef PROCESS_PACKET_HEADER

    #define PROCESS_PACKET_HEADER

    #include <queue>
    #include "packet.h"

    using namespace std;

    int    extractBitValue(uint8_t& num, int bit);
    string getStringIP(uint32_t& intIP);
    string getProtocol(uint8_t& protocol);
    string getFlags(uint8_t& flags);

#endif