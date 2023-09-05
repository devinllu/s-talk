#ifndef _INPUT_H
#define _INPUT_H

// thread for keyboard input

#include <pthread.h>
#include "list.h"

#define LEN 512

void* addToInputList();
void inputInit(List*, int*, pthread_mutex_t*, pthread_cond_t*, pthread_cond_t*, List*);
void inputShutdown();



#endif
