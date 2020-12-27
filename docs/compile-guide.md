Compile: 
g++ -o ../bin/fireflow fireflow.cpp capture.cpp process_packet.cpp -lpfring -lpcap

INSTALL PF_RING:
apt-get install pfring

INSTALL SPDLOG:
https://github.com/gabime/spdlog
