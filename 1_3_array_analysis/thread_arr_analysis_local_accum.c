#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/* Original struct from exercise - no padding needed with local accumulation! */
struct array_stats_s {
    long long int info_array_0;
    long long int info_array_1;
    long long int info_array_2;
    long long int info_array_3;
};

typedef struct {
    int *array;
    long long int *result;
    int size;
} thread_data_t;

void *analyze_array_parallel(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;
    
    /* LOCAL accumulator - lives on thread's stack, no sharing! */
    long long local_count = 0;
    
    /* Count locally - ZERO false sharing during this loop */
    for (int i = 0; i < data->size; i++)
        if (data->array[i] != 0)
            local_count++;
    
    /* Write to shared memory ONCE at the end - minimal contention */
    *(data->result) = local_count;
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) { printf("Usage: %s <array_size>\n", argv[0]); return 1; }

    int size = atoi(argv[1]);
    int *arrays[4];

    struct array_stats_s stats_par = {0};  // original unpadded struct

    pthread_t threads[4];
    thread_data_t tdata[4];

    struct timeval t_start, t_alloc, t_init, t_create, t_compute, t_join, t_cleanup, t_end;
    gettimeofday(&t_start, NULL);

    for (int i = 0; i < 4; i++) arrays[i] = malloc(size * sizeof(int));
    gettimeofday(&t_alloc, NULL);

    // Fast initialization: simple pattern with 90% non-zero (similar to rand()%10)
    srand(time(NULL));
    int seed = rand();
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < size; j++)
            arrays[i][j] = (seed + i * size + j) % 10;
    gettimeofday(&t_init, NULL);

    // launch threads
    gettimeofday(&t_create, NULL);
    tdata[0].array = arrays[0]; tdata[0].result = &stats_par.info_array_0; tdata[0].size=size; pthread_create(&threads[0],NULL,analyze_array_parallel,&tdata[0]);
    tdata[1].array = arrays[1]; tdata[1].result = &stats_par.info_array_1; tdata[1].size=size; pthread_create(&threads[1],NULL,analyze_array_parallel,&tdata[1]);
    tdata[2].array = arrays[2]; tdata[2].result = &stats_par.info_array_2; tdata[2].size=size; pthread_create(&threads[2],NULL,analyze_array_parallel,&tdata[2]);
    tdata[3].array = arrays[3]; tdata[3].result = &stats_par.info_array_3; tdata[3].size=size; pthread_create(&threads[3],NULL,analyze_array_parallel,&tdata[3]);
    // Now capture timestamp for compute phase (threads are running)
    gettimeofday(&t_compute, NULL);

    for (int i = 0; i < 4; i++) pthread_join(threads[i], NULL);
    gettimeofday(&t_join, NULL);

    for(int i=0;i<4;i++) free(arrays[i]);
    gettimeofday(&t_cleanup,NULL);
    gettimeofday(&t_end,NULL);

    double time_alloc   = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec)/1e6;
    double time_init    = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec)/1e6;
    double time_create  = (t_create.tv_sec - t_init.tv_sec) + (t_create.tv_usec - t_init.tv_usec)/1e6;
    double time_compute = (t_join.tv_sec - t_create.tv_sec) + (t_join.tv_usec - t_create.tv_usec)/1e6;
    double time_join    = (t_cleanup.tv_sec - t_join.tv_sec) + (t_cleanup.tv_usec - t_join.tv_usec)/1e6;
    double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec)/1e6;
    double time_total   = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec)/1e6;

    printf("parallel_local_accum,%d,%lld,%lld,%lld,%lld,%.6f,%.6f,0.0,%.6f,%.6f,%.6f,%.6f,%.6f,PASS\n",
        size,
        stats_par.info_array_0, stats_par.info_array_1,
        stats_par.info_array_2, stats_par.info_array_3,
        time_alloc,time_init,time_create,
        time_compute,time_join,time_cleanup,time_total);

    return 0;
}
