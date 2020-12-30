#ifndef LOGGING_HEADER

    #define LOGGING_HEADER

    #include <string>

    using namespace std;

    inline int extractBitValue(uint8_t& num, int bit);
    inline string getStringIP(uint32_t& intIP);
    inline string getProtocol(uint8_t& protocol);
    inline string getFlags(uint8_t& flags);

#endif