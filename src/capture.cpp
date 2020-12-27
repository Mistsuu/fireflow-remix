#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pfring.h>
#include "packet.h"
#include "capture.h"
#include "detection.h"
#include "process_packet.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

using namespace std;
using namespace spdlog;

// Defines all statics member here.
shared_ptr<logger> Capture::debugLogger;     // Debug logger.
shared_ptr<logger> Capture::packetLogger;    // Packet logger.
string             Capture::interface;       // The interface to capture packet.
string             Capture::debugLogPath;    // Path to debug log file.
string             Capture::packetLogPath;   // Path to packet log file.
int                Capture::windowSize;      // Size of the window of the capture.
queue<Packet>      Capture::packetQueue;     // Queue of packets.


// Defines return error codes.
#define PF_RING_INIT_ERROR_CODE             -1
#define DEBUG_LOG_OPENED_FAILED_ERROR_CODE  -2
#define PACKET_LOG_OPENED_FAILED_ERROR_CODE -3

// Defines size of programs
#define GB * 1024 * 1024 * 1024
#define MB * 1024 * 1024
#define KB * 1024

/*
    Capture() constructor:
        Create class with following properties:
            + Interface.
            + Debug log path.
            + Packet log path.
            + Window size.
*/
Capture::Capture(string _interface_, string _debugLogPath_, string _packetLogPath_, int _windowSize_) {
    if (_interface_     != "") interface     = _interface_;
    if (_debugLogPath_  != "") debugLogPath  = _debugLogPath_;
    if (_packetLogPath_ != "") packetLogPath = _packetLogPath_;
    if (_windowSize_    != 0)  windowSize    = _windowSize_;
}

/*
    initLogging():
        Create log files & settings.
*/
void Capture::initLogging() {
    // Create log files.
    try {
        debugLogger  = rotating_logger_mt("debugLog",  debugLogPath,  3 MB, 3);
    } catch (const spdlog_ex& exception) {
        cerr << "Debug log opened failed! Error: " << exception.what() << endl;
        exit(DEBUG_LOG_OPENED_FAILED_ERROR_CODE);
    }

    try {
        packetLogger = rotating_logger_mt("packetLog", packetLogPath, 3 MB, 3);
    } catch (const spdlog_ex& exception) {
        cerr << "Packet log opened failed! Error: " << exception.what() << endl;
        exit(PACKET_LOG_OPENED_FAILED_ERROR_CODE);
    }

    // Set pattern for the log files.
    debugLogger->set_pattern("[%d/%m/%Y %T.%e] [%^%l%$] %v");
    packetLogger->set_pattern("%v");

    // Outputs message that signifies the creation of logfile.
    debugLogger->info("Logger initialized!");
}

/*
    start_pfring_capture():
        Try initialize PF_RING.
*/
void Capture::start_pfring_capture() {

    // Prints selected device.
    debugLogger->info("PF_RING plugin started.");
    debugLogger->info("We selected interface: {}.", interface);
    
    // Initialize PF_RING
    if (!start_pfring_packet_preprocessing(interface.c_str())) {
        debugLogger->error("PF_RING initialization failed.");
        exit(PF_RING_INIT_ERROR_CODE);
    }

}

/*
    writeToPacketLogger():
        Write packet to packet logger.
    [Args]:
        Packet& currentPacket: A reference to the current packet.
*/
void Capture::writeToPacketLogger(Packet& currentPacket) {
    packetLogger->info("{} {} {} {} {} {} {}",
        getStringIP(currentPacket.srcIP), currentPacket.srcPort,
        getStringIP(currentPacket.dstIP), currentPacket.dstPort,
        getProtocol(currentPacket.protocol), getFlags(currentPacket.flags),
        currentPacket.length
    );
}

/*
    parsing_pfring_packet():
        Parsing PF_RING packet.
*/
void Capture::parsing_pfring_packet(const struct pfring_pkthdr *header, const u_char *buffer, const u_char *user_bytes) {
    // A packet. Description of all fields: "packet.h"
    Packet currentPacket;
    memset((void *) &header->extended_hdr.parsed_pkt, 0, sizeof(header->extended_hdr.parsed_pkt));

    // Capture packet
    u_int8_t timestamp = 0;  // We do not calculate timestamps here because it's useless and consumes so much CPU
    u_int8_t add_hash  = 0;  // https://github.com/ntop/PF_RING/issues/9
    pfring_parse_pkt((u_char *)buffer, (struct pfring_pkthdr *)header, 4, timestamp, add_hash);

    // Ignores non IPv4 and IPv6 packet
    if (header->extended_hdr.parsed_pkt.ip_version != 4 && header->extended_hdr.parsed_pkt.ip_version != 6)
        return;

    // Get packet IP version to our psuedo header
    currentPacket.IPVersion = header->extended_hdr.parsed_pkt.ip_version;

    // Get IPv4 source/dest to our psuedo header.
    if (currentPacket.IPVersion == 4) {
        // PF_RING stores data in host byte order but we use network byte order
        currentPacket.srcIP = htonl(header->extended_hdr.parsed_pkt.ip_src.v4);
        currentPacket.dstIP = htonl(header->extended_hdr.parsed_pkt.ip_dst.v4);
    }

    // Get port to our psuedo header
    currentPacket.srcPort = header->extended_hdr.parsed_pkt.l4_src_port;
    currentPacket.dstPort = header->extended_hdr.parsed_pkt.l4_dst_port;

    // We need this for deep packet inspection
    currentPacket.packetPayloadLength  = header->len;
    currentPacket.packetPayloadPointer = (void *)buffer;

    // Get other data to our psuedo header
    currentPacket.length   = header->len;
    currentPacket.protocol = header->extended_hdr.parsed_pkt.l3_proto;
    currentPacket.ts       = header->ts;

    // Copy flags from PF_RING header to our pseudo header
    if (currentPacket.protocol == IPPROTO_TCP) currentPacket.flags = header->extended_hdr.parsed_pkt.tcp.flags;
    else                                       currentPacket.flags = 0;

    // Add packet to queue.
    packetQueue.push(currentPacket);
    writeToPacketLogger(currentPacket);
    if (packetQueue.size() == windowSize){
        performDetection(packetQueue);
        packetQueue.pop();
    }
}

/*
    start_pfring_packet_preprocessing():
        Intialize PF_RING variables and start capturing;
        Get info about PF_RING version;
        Set application name to 'fireflow';
        etc...
    [Args:]
        const char* dev: Name of the device we want to capture.
*/
bool Capture::start_pfring_packet_preprocessing(const char *dev) {

    // Set variables to set flags.
    bool promiscuousMode=true;          // Enable promiscuous mode.
    bool useExtendedPKTHeader=true;     // PF_RING fills the field extended_hdr of struct pfring_pkthdr to get extra information
    bool enableHWTimestamp=false;       // Get timestamp from hardware.
    bool dontStripTimestamps=false;     // Don't strip hardware timestamp from the packets.
    bool PFRingKerelParser=true;        // Enable packet parsing.

    // Set flags.
    uint32_t flags;
    if (useExtendedPKTHeader) flags |= PF_RING_LONG_HEADER;
    if (promiscuousMode)      flags |= PF_RING_PROMISC;
    if (enableHWTimestamp)    flags |= PF_RING_HW_TIMESTAMP;
    if (!dontStripTimestamps) flags |= PF_RING_STRIP_HW_TIMESTAMP;
    if (!PFRingKerelParser)   flags |= PF_RING_DO_NOT_PARSE;
    flags |= PF_RING_DNA_SYMMETRIC_RSS;

    // Use snaplen similar to 'fastnetmon' source code.
    unsigned int snaplen = 128;
    ring = pfring_open(dev, snaplen, flags);
    if (!ring) {
        debugLogger->error("pfring_open() ERROR: {} (PF_RING not loaded or perhaps you use quick mode and have already a socket bound to: {})", 
            strerror(errno), dev
        );
        return false;
    }

    // Prints successful message!
    debugLogger->info("Sucessfully binded to: {}.",     dev);
    debugLogger->info("Device RX channels number: {}.", int(pfring_get_num_rx_channels(ring)));

    // Sets application name in /proc
    if (pfring_set_application_name(ring, (char *)"fireflow") != 0)
        debugLogger->info("Can't set program name for PF_RING: pfring_set_application_name. But it's okay. We still continue.");
    
    // Get PF_RING version
    u_int32_t version;
    pfring_version(ring, &version);
    debugLogger->info(
        "Using PF_RING v{}.{}.{}",
            (version & 0xFFFF0000) >> 16,
            (version & 0x0000FF00) >> 8,
            (version & 0x000000FF)
    );

    // Set socket mode to RECEIVE ONLY
    int pfring_socketmode_result = pfring_set_socket_mode(ring, recv_only_mode);
    if (pfring_socketmode_result != 0)
        debugLogger->info("pfring_set_socket_mode returned [rc={}].", pfring_socketmode_result);

    // Enable ring
    if (pfring_enable_ring(ring) != 0) {
        debugLogger->error("Unable to enable ring :-(");
        pfring_close(ring);
        return false;
    }

    // Set wait-for-packet mode & capture.
    u_int8_t wait_for_packet = 1;
    pfring_loop(ring, parsing_pfring_packet, NULL, wait_for_packet);
    return true;
}

/*
    stop_pfring_capture():
        Shuts down PF_RING capture.
*/
void Capture::stop_pfring_capture() {
    pfring_breakloop(ring);
    pfring_close(ring);
}
