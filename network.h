#ifndef _NETWORK_H
#define _NETWORK_H

// module, not thread, for setting up the network sockets

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	

int port;
int socketfd;
struct sockaddr_in currentSocket;
socklen_t socketLen;
struct addrinfo* res;

void setupNetwork(char*, char*, char*);


#endif