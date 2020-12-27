Compile: 
g++ -o ../bin/fireflow fireflow.cpp capture.cpp process_packet.cpp -lpfring -lpcap

>>> But before that:

INSTALL PF_RING:
apt-get install pfring

INSTALL SPDLOG:
Follows the guide on: https://github.com/gabime/spdlog.
Then, copies the folder spdlog/ at include/ to /usr/local/include.
