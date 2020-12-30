#ifndef CAPTURE_HEADER

    #define CAPTURE_HEADER

    #include <string>
    #include <fstream>
    #include <queue>
    #include <pfring.h>
    #include <spdlog/spdlog.h>
    #include <spdlog/sinks/rotating_file_sink.h>
    #include "packet.h"
    #include "process_packet.h"
    #include "detection.h"

    using namespace std;
    using namespace spdlog;

    class Capture {
        static shared_ptr<logger> debugLogger;   // Debug logger.
        static shared_ptr<logger> packetLogger;  // Packet logger.
        pfring *ring=NULL;                       // PF_RING socket to capture data.

        public:
            static string   interface;           // The interface to capture packet.
            static string   debugLogPath;        // Path to debug log file.
            static string   packetLogPath;       // Path to packet log file.
            static int      windowSize;          // Size of the window of the capture.

            Capture(string interface, string debugLogPath, string packetLogPath, int windowSize);
            void        initLogging();
            void        start_pfring_capture();
            static void parsing_pfring_packet(const struct pfring_pkthdr *header, const u_char *buffer, const u_char *user_bytes);
            bool        start_pfring_packet_preprocessing(const char *dev);
            void        stop_pfring_capture();
            static void writeToPacketLogger(Packet& currentPacket);
    };

#endif
