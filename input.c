#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	
#include <pthread.h>

static pthread_t inputThread;
static List* inputList; 
static List* outputList;
static int* cancelRequest;
static char* buffer = NULL; 

static pthread_cond_t* condEmptyInputList = NULL; 
static pthread_cond_t* condFullInputList = NULL;

static pthread_mutex_t* mutexInputList = NULL; 

void* addToInputList() {  

    while (1) {
    	
        if (*cancelRequest) {
            return NULL; 
        }
  
        buffer = malloc(sizeof(char*)*LEN);  
        fgets(buffer, LEN, stdin); 

        if (buffer !=  NULL) {

	        pthread_mutex_lock(mutexInputList);

            if ((List_count(inputList) + List_count(outputList)) >= 1000) {
	        	printf("no more available nodes in input.c! there could be undefined behaviour");
				pthread_cond_wait(condFullInputList, mutexInputList);
            }

            if (*cancelRequest == 0 && buffer != NULL) {
			     List_add(inputList, buffer); 
            }

	        pthread_mutex_unlock(mutexInputList);
        	pthread_cond_signal(condEmptyInputList);
        }
        
    }
            
    return NULL;
}

void inputInit(List* listIn, int* cancel, pthread_mutex_t* mutex,
    pthread_cond_t* emptyCondVar, pthread_cond_t* fullCondVar, List* listOut) {
   
    inputList = listIn;
    outputList = listOut;

    cancelRequest = cancel; 

    mutexInputList = mutex; 

    condEmptyInputList = emptyCondVar; 
    condFullInputList = fullCondVar;

   
    pthread_create(&inputThread, NULL, addToInputList, NULL);

}

void inputShutdown() {

    if (buffer != NULL) {
        free(buffer); 
        buffer = NULL; 
    }

    pthread_cancel(inputThread);
    pthread_join(inputThread, NULL); 

}
