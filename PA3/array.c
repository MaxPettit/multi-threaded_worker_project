#include "array.h"

typedef struct{
    char shared_arr[ARRAY_SIZE][MAX_NAME_LENGTH];
    int read_ptr;
    int write_ptr;
} SHARED_BUF_STRUCT;

SHARED_BUF_STRUCT shared_buf;
pthread_mutex_t array_mutex;
pthread_cond_t full_cond, empty_cond;


void array_init(void){
    shared_buf.read_ptr = 0;
    shared_buf.write_ptr = 0;
    pthread_mutex_init(&array_mutex, NULL);
    pthread_cond_init(&full_cond, NULL);
    pthread_cond_init(&empty_cond, NULL);
}

int arr_full(void){
    if((shared_buf.write_ptr+1)% ARRAY_SIZE == shared_buf.read_ptr) return 1;
    return 0;
}

int arr_empty(void){
    if(shared_buf.write_ptr == shared_buf.read_ptr) return 1;
    return 0;
}

void array_push(char * hostname){
    
    pthread_mutex_lock(&array_mutex);
    while(arr_full()) pthread_cond_wait(&full_cond, &array_mutex);

    strcpy(shared_buf.shared_arr[shared_buf.write_ptr], hostname);
    shared_buf.write_ptr = (shared_buf.write_ptr + 1) % ARRAY_SIZE;

    pthread_mutex_unlock(&array_mutex);
    pthread_cond_signal(&empty_cond);
    
}

void array_pop(char * readname){
    pthread_mutex_lock(&array_mutex);
    while(arr_empty()) pthread_cond_wait(&empty_cond, &array_mutex);

    strcpy(readname, shared_buf.shared_arr[shared_buf.read_ptr]);
    shared_buf.read_ptr = (shared_buf.read_ptr + 1) % ARRAY_SIZE;

    pthread_mutex_unlock(&array_mutex);
    pthread_cond_signal(&full_cond);
}
