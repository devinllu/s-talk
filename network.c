#include "network.h"


void setupNetwork(char* localPort, char* address, char* remotePort) {

	port = atoi(remotePort);

    if (port == 0) {
        perror("RECEIVER.C Port not converted to int sucessfully\n");
        exit(0); 
    }

	socketLen = sizeof(currentSocket); 

	socketfd = socket(PF_INET, SOCK_DGRAM, 0);

    if (socketfd < 0) {
        perror("RECEVIER.C Socket not created sucessfully\n"); 
        exit(0); 
    }

	if (memset(&currentSocket, 0, sizeof(currentSocket)) != &currentSocket) {
        perror("RECEIVER.C memset error\n");
        exit(0); 
    }

    struct addrinfo* resultAddr;

    int getAddrInfoRes = getaddrinfo(address, localPort, NULL, &resultAddr); 
    if (getAddrInfoRes != 0) {
        perror("RECEIVER.C getAddrInfo error\n"); 
        exit(0); 
    }

    res = resultAddr;

    currentSocket.sin_family = AF_INET;
	currentSocket.sin_addr.s_addr = htonl(INADDR_ANY);
	currentSocket.sin_port = htons(port);

	int bindResult = bind(socketfd, (struct sockaddr*) &currentSocket, sizeof(currentSocket));
    if (bindResult < 0) {
        perror("RECEIVER.C bind not successful\n"); 
        exit(0); 
    }
}