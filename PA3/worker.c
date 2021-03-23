#include "worker.h"

pthread_mutex_t pos_mutex;
pthread_mutex_t print_mutex;
pthread_mutex_t rq_log_mutex;
pthread_mutex_t rs_log_mutex;

void worker_init(void){
    pthread_mutex_init(&pos_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);
    pthread_mutex_init(&rq_log_mutex, NULL);
    pthread_mutex_init(&rq_log_mutex, NULL);
    array_init();
}

// pushes a close string to array for each resolver thread
void worker_close(int n){
    for(int i = 0; i < n; i++) array_push(CLOSE_PILL);
}

void * request_worker(void * rq_init){
    request_init_struct * rq_struct = (request_init_struct *) rq_init;

    int local_pos;
    int count = 0;
    int len;
    FILE * fptr;
    char * hostname;
    pthread_t tid = pthread_self();

    hostname = (char *)malloc(MAX_NAME_LENGTH * sizeof(char));
    
    while(1){
        pthread_mutex_lock(&pos_mutex);
        if((local_pos = rq_struct->pos) >= rq_struct->input_cnt){
            pthread_mutex_unlock(&pos_mutex);
            break;
        }
        rq_struct->pos++;
        pthread_mutex_unlock(&pos_mutex);

        if ((fptr = fopen(rq_struct->input_names[local_pos], "r")) == NULL){
            pthread_mutex_lock(&print_mutex);
            fprintf(stderr, "invalid file %s\n", rq_struct->input_names[local_pos]);
            pthread_mutex_unlock(&print_mutex);
            continue;
        }

        while(fgets(hostname, MAX_NAME_LENGTH, fptr)){
            len = strlen(hostname);
            if(len > 0 && hostname[len-1] == '\n') hostname[len-1] = '\0';
            array_push(hostname);  

            pthread_mutex_lock(&rq_log_mutex);
            fprintf(rq_struct->serviced_log, "%s\n", hostname);
            pthread_mutex_unlock(&rq_log_mutex);
        }
        count++;
        fclose(fptr);
    }
    
    // print count
    pthread_mutex_lock(&print_mutex);
    printf("thread %8x serviced %d files\n", (int) tid, count);
    pthread_mutex_unlock(&print_mutex);

    free(hostname);
    pthread_exit(NULL);
}

void * resolve_worker(void * rs_init){
    resolve_init_struct * rs_struct = (resolve_init_struct *) rs_init;
    char ip_string[MAX_IP_LENGTH];
    char * hostname;
    int count = 0;
    pthread_t tid = pthread_self();
    hostname = (char *)malloc(MAX_NAME_LENGTH * sizeof(char));

    while(1){
        array_pop(hostname);
        if(strcmp(hostname, CLOSE_PILL) == 0) break;
        if(dnslookup(hostname, ip_string, MAX_IP_LENGTH)){
            strcpy(ip_string, "NOT_RESOLVED");
        }
        if(strcmp("UNHANDELED", ip_string) == 0) strcpy(ip_string, "NOT_RESOLVED");
        pthread_mutex_lock(&rs_log_mutex);
        fprintf(rs_struct->results_log, "%s, %s\n", hostname, ip_string);
        pthread_mutex_unlock(&rs_log_mutex);
        count++;
    }

    // print count
    pthread_mutex_lock(&print_mutex);
    printf("thread %8x resolved %d hostnames\n", (int) tid, count);
    pthread_mutex_unlock(&print_mutex);

    free(hostname);
    pthread_exit(NULL);
}
