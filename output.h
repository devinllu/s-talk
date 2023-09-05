#ifndef _OUTPUT_H
#define _OUTPUT_H

// thread for screen output, (or file output)

#include <pthread.h> 
#include "list.h"

#define LEN 512

void* outputText();
void outputInit(List*, int*, pthread_mutex_t*, pthread_cond_t*, pthread_cond_t*);
void outputShutdown();


#endif