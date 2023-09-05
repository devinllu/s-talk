#ifndef _SENDER_H
#define _SENDER_H

// thread for sending data to remote machine

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	

#include "list.h"

#define LEN 512


void* sendThread();
void senderInit(List*, int*, pthread_mutex_t*, pthread_cond_t*, pthread_cond_t*, struct addrinfo*, int);
void senderShutdown();


#endif