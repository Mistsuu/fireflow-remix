#include <queue>
#include <map>
#include <iostream>
#include "packet.h"
#include "entropy.h"
#include "statistic.h"

#define loop(_i_, _n_) for(int _i_ = 0; i < _n_; ++_i_)
#define ICMP_CUSUM_THRESHOLD 1000

using namespace std;

vector<double> ICMPListEntropy;

bool ICMP_CUSUM_Detection() {
    // Get mean (alpha) of the list entropy
    double mean = getMean(ICMPListEntropy);

    // Get CUSUM value of the entropy
    vector<double> judgementValues;
    CUSUM(ICMPListEntropy, judgementValues, 0);

    // Returns true if the value reaches more than threshold
    return judgementValues.back() > ICMP_CUSUM_THRESHOLD;
}

void ICMPDetection(queue<Packet> packetQueue) {
    // Fork entropy.
    double entropy = getEntropy(packetQueue, IPPROTO_ICMP);
    ICMPListEntropy.push_back(entropy);

    if (ICMP_CUSUM_Detection()) {
        cout << "[] Fuck yeah! Detected!" << endl;
    }
}

void UDPDetection(queue<Packet> packetQueue) {

}

void SYNDetection(queue<Packet> packetQueue) {

}

void performDetection(queue<Packet> packetQueue) {
    ICMPDetection(packetQueue);
    SYNDetection(packetQueue);
    UDPDetection(packetQueue);
}