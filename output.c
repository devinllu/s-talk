#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>	

static List* outputList;
static pthread_t outputThread;
static int* cancelRequest;
static char* item = NULL;  

static pthread_cond_t* condEmptyOutputList = NULL; 
static pthread_mutex_t* mutexOutputList = NULL; 
static pthread_cond_t* condFullOutputList = NULL;

void* outputText() {

    while (1) {
        pthread_mutex_lock(mutexOutputList); 

        if (List_count(outputList) == 0) {
            pthread_cond_wait(condEmptyOutputList, mutexOutputList);
        }
    
        item = List_pop(outputList);
	
    	if (fputs(item, stdout) <= 0) {
            perror("Output.C write error\n"); 
            exit(0); 
    	} 		
	
        if (item != NULL) {
            free(item); 
            item = NULL;
        }         

        pthread_mutex_unlock(mutexOutputList); 
        pthread_cond_signal(condFullOutputList);
        
        if (*cancelRequest) {

            // // If the output was slow, this makes sure everything gets printed
            while (List_count(outputList) != 0) {
                item = List_pop(outputList); 
                if (fputs(item,stdout) <= 0) {
                    perror("Output.c write error\n"); 
                    exit(0); 
                }

                if (item != NULL) {
                    free(item); 
                    item = NULL;
                }
            }
            
            if (item != NULL) {
                free(item); 
                item = NULL;
            }

            return NULL;
        } 
        
    }

    return NULL;
}

void outputInit(List* pList, int* cancel, 
    pthread_mutex_t* mutexOutput, pthread_cond_t* emptyCondVar, pthread_cond_t* fullCondVar) {

    outputList = pList;
    cancelRequest = cancel;

    mutexOutputList = mutexOutput; 

    condEmptyOutputList = emptyCondVar; 
    condFullOutputList = fullCondVar;

    pthread_create(&outputThread, NULL, outputText, NULL);
}

void outputShutdown() {

    pthread_cancel(outputThread);
    pthread_join(outputThread, NULL); 
    
    if (item != NULL) {
        free(item); 
        item = NULL;
    }

}
