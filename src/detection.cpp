#include <queue>
#include <iostream>
#include "packet.h"
#include "entropy.h"

using namespace std;

void performDetection(queue<Packet>& packetQueue) {
    cout << getEntropy(packetQueue) << endl;
}