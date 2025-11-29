#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

struct array_stats_s {
    long long int info_array_0;
    long long int info_array_1;
    long long int info_array_2;
    long long int info_array_3;
};

// Padded version to prevent false sharing (each field on separate cache line)
struct array_stats_padded_s {
    long long int info_array_0;
    char pad0[64 - sizeof(long long int)];
    long long int info_array_1;
    char pad1[64 - sizeof(long long int)];
    long long int info_array_2;
    char pad2[64 - sizeof(long long int)];
    long long int info_array_3;
    char pad3[64 - sizeof(long long int)];
};

typedef struct {
    int *array;
    long long int *result;
    int size;
    int array_id;
} thread_data_t;

void *analyze_array_parallel(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    long long count = 0;
    
    for (int i = 0; i < data->size; i++) {
        if (data->array[i] != 0) {
            count++;
        }
    }
    
    *(data->result) = count;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <array_size>\n", argv[0]);
        return 1;
    }
    
    int size = atoi(argv[1]);
     int *arrays[4];
     struct array_stats_s stats_seq = {0};
     /* Use padded stats struct for parallel results to avoid false sharing
         between threads writing adjacent struct fields. */
     struct array_stats_padded_s stats_par = {0};
    
    struct timeval t_start, t_alloc, t_init, t_seq, t_create, t_compute, t_join, t_cleanup, t_end;
    
    gettimeofday(&t_start, NULL);
    
    // Allocate arrays
    for (int i = 0; i < 4; i++) {
        arrays[i] = malloc(size * sizeof(int));
    }
    gettimeofday(&t_alloc, NULL);
    
    // Initialize arrays
    srand(time(NULL));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < size; j++) {
            arrays[i][j] = rand() % 10;
        }
    }
    gettimeofday(&t_init, NULL);
    
    // Sequential analysis (baseline)
    for (int i = 0; i < 4; i++) {
        long long count = 0;
        for (int j = 0; j < size; j++) {
            if (arrays[i][j] != 0) {
                count++;
            }
        }
        switch(i) {
            case 0: stats_seq.info_array_0 = count; break;
            case 1: stats_seq.info_array_1 = count; break;
            case 2: stats_seq.info_array_2 = count; break;
            case 3: stats_seq.info_array_3 = count; break;
        }
    }
    gettimeofday(&t_seq, NULL);
    
    // Parallel analysis
    pthread_t threads[4];
    thread_data_t thread_data[4];
    long long *results[4] = {&stats_par.info_array_0, &stats_par.info_array_1,
                            &stats_par.info_array_2, &stats_par.info_array_3};
    
    gettimeofday(&t_create, NULL);
    for (int i = 0; i < 4; i++) {
        thread_data[i].array = arrays[i];
        thread_data[i].result = results[i];
        thread_data[i].size = size;
        thread_data[i].array_id = i;
        pthread_create(&threads[i], NULL, analyze_array_parallel, &thread_data[i]);
    }
    gettimeofday(&t_compute, NULL);
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&t_join, NULL);
    
    // Verification
    int correct = (stats_seq.info_array_0 == stats_par.info_array_0 &&
                  stats_seq.info_array_1 == stats_par.info_array_1 &&
                  stats_seq.info_array_2 == stats_par.info_array_2 &&
                  stats_seq.info_array_3 == stats_par.info_array_3);
    
    gettimeofday(&t_cleanup, NULL);
    for (int i = 0; i < 4; i++) free(arrays[i]);
    gettimeofday(&t_end, NULL);
    
    double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
    double time_seq = (t_seq.tv_sec - t_init.tv_sec) + (t_seq.tv_usec - t_init.tv_usec) / 1000000.0;
    double time_create = (t_compute.tv_sec - t_create.tv_sec) + (t_compute.tv_usec - t_create.tv_usec) / 1000000.0;
    double time_compute = (t_join.tv_sec - t_compute.tv_sec) + (t_join.tv_usec - t_compute.tv_usec) / 1000000.0;
    double time_join = (t_cleanup.tv_sec - t_join.tv_sec) + (t_cleanup.tv_usec - t_join.tv_usec) / 1000000.0;
    double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%lld,%lld,%lld,%lld,%f,%f,%f,%f,%f,%f,%f,%f,%s\n",
           stats_par.info_array_0, stats_par.info_array_1,
           stats_par.info_array_2, stats_par.info_array_3,
           time_alloc, time_init, time_seq, time_create,
           time_compute, time_join, time_cleanup, time_total,
           correct ? "PASS" : "FAIL");
    
    return 0;
}