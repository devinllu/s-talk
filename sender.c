#include "sender.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	
#include <pthread.h>

static List* inputList;
static pthread_t senderThread;

static int* cancelRequest; 
static char terminate = '!'; 
static char* terminateLine = "!\n"; 
static char* item = NULL;

static struct addrinfo* servInfo = NULL;
static int socketfd;

static pthread_cond_t* condEmptyInputList = NULL; 
static pthread_mutex_t* mutexInputList = NULL; 
static pthread_cond_t* condFullInputList = NULL;


void* sendThread() {

    while (1) {

        pthread_mutex_lock(mutexInputList); 

        if (List_count(inputList) == 0) {
            pthread_cond_wait(condEmptyInputList, mutexInputList);
        }


        if (*cancelRequest) {
            // Cancel remaining messages in case of fast shut down
            sleep(1); 
            return NULL;
        }

        item = List_pop(inputList);
        if (sendto(socketfd, item, LEN, 0, servInfo->ai_addr, servInfo->ai_addrlen) == -1 && item != NULL) {
            perror("SENDER.C sendto error\n");
            exit(0); 
        }
 
        if(item != NULL) { 
        	if ((strcmp(item, terminateLine) == 0) || (strcmp(item, &terminate) == 0)) {
            	*cancelRequest = 1; 
        	}
        }

        if (item != NULL) {
            free(item);
            item = NULL; 
        }
    
        pthread_mutex_unlock(mutexInputList);
        pthread_cond_signal(condFullInputList);

        if (*cancelRequest) {
            // Cancel remaining messages in case of fast shut down
            sleep(1); 
            return NULL;
        }
    }

    return NULL;
}

void senderInit(List* pList, int* cancel,
    pthread_mutex_t* mutexInput, pthread_cond_t* emptyCondVar, pthread_cond_t* fullCondVar, struct addrinfo* results, int fd) {

    inputList = pList;
    cancelRequest = cancel; 

    mutexInputList = mutexInput; 

    condEmptyInputList = emptyCondVar; 
    condFullInputList = fullCondVar;

    servInfo = results;
    socketfd = fd;

    pthread_create(&senderThread, NULL, sendThread, NULL);
}

void senderShutdown() {

    item = NULL;

    pthread_join(senderThread, NULL);
    
    if (item != NULL) {
        free(item); 
        item = NULL; 
    }
    
}

