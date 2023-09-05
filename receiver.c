#include "receiver.h"
#include "sender.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	

static List* outputList;
static List* inputList;

static pthread_t receiverThread;
static char* terminateLine = "!\n";
static char terminate = '!'; 
static int* cancelRequest; 
static int cancelFromClient = 0; 

static pthread_cond_t* condEmptyOutputList = NULL; 
static pthread_mutex_t* mutexOutputList = NULL; 
static pthread_cond_t* condFullOutputList = NULL;
static pthread_cond_t* condEmptyInputList = NULL;

static struct addrinfo* servInfo = NULL; 
static char* buffer = NULL; 
static int socketfd = 0;
static int socketClosed = 0;
static int addressFreed = 0;  

void* receiveThread() {
	
    while (1) {

        buffer = malloc(sizeof(char)*LEN); 
       
    	unsigned int resLen = sizeof(servInfo);

        if (recvfrom(socketfd, buffer, LEN, 0, 
            (struct sockaddr*) servInfo->ai_addr, &resLen) <= 0) {
            perror("Receive.c recvfrom error\n");
            exit(0); 
        } 

        pthread_mutex_lock(mutexOutputList);

        if ((List_count(outputList) + List_count(inputList)) >= 1000) {

        	printf("no more nodes in receiver.c! could be undefined behaviour");
    		pthread_cond_wait(condFullOutputList, mutexOutputList);
    	}

        List_add(outputList, buffer);
        if (strcmp(buffer, terminateLine) == 0 || strcmp(buffer, &terminate) == 0) {
            *cancelRequest = 1;
            
            // need to signal the sender to shutdown!
            pthread_cond_signal(condEmptyInputList);
            cancelFromClient = 1; 
        }
            
        pthread_mutex_unlock(mutexOutputList);
        pthread_cond_signal(condEmptyOutputList);
      
        if (*cancelRequest) {

            socketClosed = 1;  
            addressFreed = 1; 
            // need to sleep to exit!
            sleep(1);

            return NULL;  
        }
	}

	return NULL;
}

void receiverInit(List* pList, int* cancel,
    pthread_mutex_t* mutexOutput, pthread_cond_t* emptyCondVar, pthread_cond_t* fullCondVar, pthread_cond_t* emptyCondVarInput, 
    int fd, struct addrinfo* info, List* pList2) {

    outputList = pList;
    inputList = pList2;

    cancelRequest = cancel; 

    mutexOutputList = mutexOutput; 

    condEmptyOutputList = emptyCondVar; 
    condFullOutputList = fullCondVar;

    condEmptyInputList = emptyCondVarInput;

    socketfd = fd;
    servInfo = info;

    pthread_create(&receiverThread, NULL, receiveThread, NULL);

}

void receiverShutdown() {
    // Calling close and freeaddrinfo here so the client that send the "!"
    // can be cleaned up as well
  
    pthread_cancel(receiverThread);
    pthread_join(receiverThread, NULL); 
    
    if (cancelFromClient == 0) {
        sleep(1); 
        free(buffer); 
        
        close(socketfd); 
        buffer = NULL; 
    }
    
}
