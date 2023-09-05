#ifndef _RECEIVER_H
#define _RECEIVER_H

// thread for receiving data from remote machine

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	

#include "list.h"

#define LEN 512

void* receiveThread();
void receiverInit(List*, int*, pthread_mutex_t*, pthread_cond_t*, pthread_cond_t*, pthread_cond_t*, int, struct addrinfo*, List*);
void receiverShutdown();


#endif