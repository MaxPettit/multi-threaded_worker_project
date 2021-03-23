#include "multi-lookup.h"

static void print_usage(void){
    printf("usage: multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]\n");
}

int main(int argc, char *argv[]){
    int rv, input_files;
    int request_n, resolve_n;
    FILE * requester_log,  * resolver_log;
    
    struct timeval start_time, end_time;
    
    pthread_t requester_id[10], resolver_id[10];
    
    request_init_struct rq_init;
    resolve_init_struct rs_init;

    // Start Timer
    if(gettimeofday(&start_time, NULL)){
        fprintf(stderr, "gettimeofday error\n");
        return -1;
    }

    if(argc < 6){
        print_usage();
        return 0;
    }
    rv = sscanf(argv[1], "%d", &request_n);
    if(rv != 1 || request_n > MAX_REQUESTER_THREADS || request_n < 1){
        fprintf(stderr, "Invalid number of requester threads (limit %d)\n", MAX_REQUESTER_THREADS);
        return -1;
    }

    rv = sscanf(argv[2], "%d", &resolve_n);
    if(rv != 1 || resolve_n > MAX_RESOLVER_THREADS || || resolve_n < 1){
        fprintf(stderr, "Invalid number of resolver threads (limit %d)\n", MAX_RESOLVER_THREADS);
        return -1;
    }

    // Subtract 5 from argc to account for first 4 inputs and program name
    input_files = argc - 5;
    if(input_files > MAX_INPUT_FILES){   
        fprintf(stderr, "Too many input files (limit %d)\n", MAX_INPUT_FILES);
        return -1;
    }

    requester_log = fopen(argv[3], "w");
	if (requester_log == NULL){
		printf("Error opening requester_log file\n");
		return -2;
	}

    resolver_log = fopen(argv[4], "w");
	if (resolver_log == NULL){
		printf("Error opening resolver_log file\n");
		return -2;
	}

    worker_init();

    // Request struct
    rq_init.input_cnt = input_files;            // number of input files
    rq_init.serviced_log = requester_log;       // log file
    rq_init.input_names = &argv[5];             // argv starting at first input file
    rq_init.pos = 0;                            // pos in input names array

    // Resolve struct
    rs_init.complete = 0;                       // boolean of intake complete
    rs_init.results_log = resolver_log;         // log file

    for(int i = 0; i < request_n; i++){
        pthread_create(&requester_id[i], NULL, &request_worker, &rq_init);
    }

    for(int i = 0; i < resolve_n; i++){
        pthread_create(&resolver_id[i], NULL, &resolve_worker, &rs_init);
    }

    for(int i = 0; i < request_n; i++){
        pthread_join(requester_id[i], NULL);
    }

    rs_init.complete = 1;

    for(int i = 0; i < resolve_n; i++){
        pthread_join(resolver_id[i], NULL);
    }

    // close log files
    fclose(requester_log);
    fclose(resolver_log);

    if(gettimeofday(&end_time, NULL)){
        fprintf(stderr, "gettimeofday error\n");
        return -1;
    }

    printf("%s: total time is %ld.%06d seconds\n", argv[0], 
<<<<<<< HEAD
        end_time.tv_sec-start_time.tv_sec, (int) (end_time.tv_usec-start_time.tv_usec /1000));
=======
        end_time.tv_sec-start_time.tv_sec, (int)(end_time.tv_usec-start_time.tv_usec /1000));
>>>>>>> 42b08ebd16d2f42fd891ffda14e9ff2b4980ba76

    return 0;
}
