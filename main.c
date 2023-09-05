#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h> 

#include "receiver.h"
#include "sender.h"
#include "input.h"
#include "output.h"
#include "list.h"
#include "network.h"

// IDEA: setup the 4 threads, using cond vars and mutexes to solve the CSP
// the 4 threads are split up into separate .c files, as well as another .c file
// for networking. relevant information is passed into the init functions as parameters 
// similar to Dr. Fraser's workshop video. 

// program continues until an "!", or cancel request is inputted. there is a shared variable
// for a cancel request where every thread can check if the cancel request is true. this gets 
// set to true either in the keyboard input thread, or in the receiver thread when it gets sent
// to the remote machine. program terminates through cancel and join, and then everything
// is properly cleaned up


// THIS IS S-TALK, a multithreaded chat application

static int cancelRequest = 0; 

static pthread_cond_t emptyInputListCondition = PTHREAD_COND_INITIALIZER; 
static pthread_cond_t emptyOutputListCondition = PTHREAD_COND_INITIALIZER; 

static pthread_cond_t fullInputListCondition = PTHREAD_COND_INITIALIZER;
static pthread_cond_t fullOutputListCondition = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t inputMutex = PTHREAD_MUTEX_INITIALIZER; 
static pthread_mutex_t outputMutex = PTHREAD_MUTEX_INITIALIZER; 

// function for list free
static void freeListFunction(void* pNode) {
    free(pNode);
}


int main(int args, char** argv) {

    List* receiverList = List_create();
    List* senderList = List_create();

    if (args != 4) {
        printf("sorry, please pass in 4 arguments\n");

        return 0;
    }

    // sets the socket up for networking, and sets all
    // the relevant fields
    setupNetwork(argv[1], argv[2], argv[3]);



    inputInit(senderList, &cancelRequest, 
        &inputMutex, &emptyInputListCondition, &fullInputListCondition, receiverList);

    senderInit(senderList, &cancelRequest, 
        &inputMutex, &emptyInputListCondition, &fullInputListCondition, res, socketfd);
    

    receiverInit(receiverList, &cancelRequest,
        &outputMutex, &emptyOutputListCondition, &fullOutputListCondition, &emptyInputListCondition, socketfd, res, senderList);

    outputInit(receiverList, &cancelRequest, 
        &outputMutex, &emptyOutputListCondition, &fullOutputListCondition);




    senderShutdown();
    receiverShutdown();
    
    outputShutdown();
    inputShutdown();

    pthread_mutex_destroy(&outputMutex); 
    pthread_mutex_destroy(&inputMutex); 
    pthread_cond_destroy(&emptyOutputListCondition); 
    pthread_cond_destroy(&emptyInputListCondition); 
    pthread_cond_destroy(&fullInputListCondition);
    pthread_cond_destroy(&fullOutputListCondition);


    freeaddrinfo(res);
    close(socketfd);

    List_free(senderList, freeListFunction);
    List_free(receiverList, freeListFunction);

    if (List_count(receiverList) != 0) {
        printf("receiver list size %d\n", List_count(receiverList)); 
    }

    if (List_count(senderList) != 0) {
        printf("sender list size %d\n", List_count(senderList)); 
    }

 



    return 0; 
}