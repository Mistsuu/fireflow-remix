#include <iostream>
#include <string>
#include <csignal>
#include "../include/CLI11.hpp"
#include "capture.h"
#include "packet.h"
#include "process_packet.h"
#include "detection.h"
using namespace std;

Capture *captureInterface=NULL;
void sigIntHandler(int signal);

int main(int argc, char* argv[]){
    // Start program with SIGINT detector.
    signal(SIGINT, sigIntHandler);
    CLI::App app{"FIREFLOW: USTH ANTI_DDOS"};

    // Parse data from arguments.
    string interface, debugLogPath, packetLogPath;
    int    windowSize = 0;
    app.add_option("-i,--interface", interface,      "Capture interface"            )->required();
    app.add_option("-l",             debugLogPath,   "Where to dump log file"       )->default_val("/tmp/fireflow-log.txt");
    app.add_option("-p",             packetLogPath,  "Where to dump packet log"     )->default_val("/tmp/packetlogger.txt");
    app.add_option("-w",             windowSize,     "The amount of packets to read")->default_val(1000);
    CLI11_PARSE(app, argc, argv);

    // Create Capture instance & start capturing.
    captureInterface = new Capture(interface, debugLogPath, packetLogPath, windowSize);
    captureInterface->initLogging();
    captureInterface->start_pfring_capture();
}

/*
    sigIntHandler():
        Outputs to terminal during pressing CTRL-C & Exits.
*/
void sigIntHandler(int signal) {
    std::cout << "\nExiting..." << std::endl;
    if (captureInterface)
        captureInterface->stop_pfring_capture();
    exit(signal);
}
