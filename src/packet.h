#ifndef PACKET_HEADER

    #define PACKET_HEADER
    #include <iostream>
    #include <netinet/in.h>
    #include <sys/socket.h>

    class Packet {
        public:
            uint32_t sampleRatio=1;

            uint32_t srcIP=0;
            uint32_t dstIP=0;

            uint16_t srcPort=0;
            uint16_t dstPort=0;

            uint32_t inputInterface=0;
            uint32_t outputInterface=0;

            uint8_t  IPVersion=4;
            uint8_t  TTL=0;
            uint8_t  protocol=0;
            uint8_t  flags=0;

            uint64_t length=0;
            bool     IPFragmented=false;

            struct timeval ts;
            void*          packetPayloadPointer=NULL;
            uint32_t       packetPayloadLength=0;
            uint32_t       packetPayloadFullLength=0;

            int flowStart=0;
            int flowEnd=0;
    };

    typedef void (*packetProcessPtr)(Packet&);

#endif
