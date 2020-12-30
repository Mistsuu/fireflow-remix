#ifndef LOGGING_HEADER

    #define LOGGING_HEADER

    #include <string>

    using namespace std;

    int    extractBitValue(uint8_t& num, int bit);
    string getStringIP(uint32_t& intIP);
    string getProtocol(uint8_t& protocol);
    string getFlags(uint8_t& flags);

#endif