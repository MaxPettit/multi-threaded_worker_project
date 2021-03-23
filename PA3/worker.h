#ifndef WORKER_H
#define WORKER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h> 
#include "util.h"
#include "array.h"

#define MAX_IP_LENGTH INET6_ADDRSTRLEN

// struct for each type worker
typedef struct{
    int input_cnt;
    int pos;
    FILE * serviced_log;
    char **input_names;
}request_init_struct;

typedef struct{
    FILE * results_log;
    int complete;
}resolve_init_struct;

void worker_init(void);
void * request_worker(void * rq_init);
void * resolve_worker(void * rs_init);

#endif