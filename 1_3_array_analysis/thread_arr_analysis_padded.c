#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define CACHELINE 64

struct array_stats_s {
    long long int info_array_0;
    long long int info_array_1;
    long long int info_array_2;
    long long int info_array_3;
};

/* padded struct to avoid false sharing */
struct array_stats_padded_s {
    long long int info_array_0; char pad0[CACHELINE - sizeof(long long int)];
    long long int info_array_1; char pad1[CACHELINE - sizeof(long long int)];
    long long int info_array_2; char pad2[CACHELINE - sizeof(long long int)];
    long long int info_array_3; char pad3[CACHELINE - sizeof(long long int)];
};

typedef struct {
    int *array;
    long long int *result;
    int size;
} thread_data_t;

void *analyze_array_parallel(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;
    for(int i=0;i<data->size;i++)
        if(data->array[i]!=0)
            (*(data->result))++;
    return NULL;
}

int main(int argc,char *argv[])
{
    if(argc!=2){ printf("Usage: %s <array_size>\n",argv[0]); return 1; }

    int size = atoi(argv[1]);
    int *arrays[4];
    struct array_stats_s stats_seq = {0};
    struct array_stats_padded_s stats_par = {0};

    pthread_t threads[4];
    thread_data_t tdata[4];

    struct timeval t_start,t_alloc,t_init,t_seq,t_create,t_compute,t_join,t_cleanup,t_end;
    gettimeofday(&t_start,NULL);

    for(int i=0;i<4;i++) arrays[i]=malloc(size*sizeof(int));
    gettimeofday(&t_alloc,NULL);

    srand(time(NULL));
    for(int i=0;i<4;i++)
        for(int j=0;j<size;j++)
            arrays[i][j]=rand()%10;
    gettimeofday(&t_init,NULL);

    // sequential baseline
    long long count;
    count=0; for(int j=0;j<size;j++) if(arrays[0][j]!=0) count++; stats_seq.info_array_0=count;
    count=0; for(int j=0;j<size;j++) if(arrays[1][j]!=0) count++; stats_seq.info_array_1=count;
    count=0; for(int j=0;j<size;j++) if(arrays[2][j]!=0) count++; stats_seq.info_array_2=count;
    count=0; for(int j=0;j<size;j++) if(arrays[3][j]!=0) count++; stats_seq.info_array_3=count;
    gettimeofday(&t_seq,NULL);

    gettimeofday(&t_create,NULL);
    tdata[0].array=arrays[0]; tdata[0].result=&stats_par.info_array_0; tdata[0].size=size; pthread_create(&threads[0],NULL,analyze_array_parallel,&tdata[0]);
    tdata[1].array=arrays[1]; tdata[1].result=&stats_par.info_array_1; tdata[1].size=size; pthread_create(&threads[1],NULL,analyze_array_parallel,&tdata[1]);
    tdata[2].array=arrays[2]; tdata[2].result=&stats_par.info_array_2; tdata[2].size=size; pthread_create(&threads[2],NULL,analyze_array_parallel,&tdata[2]);
    tdata[3].array=arrays[3]; tdata[3].result=&stats_par.info_array_3; tdata[3].size=size; pthread_create(&threads[3],NULL,analyze_array_parallel,&tdata[3]);
    gettimeofday(&t_compute,NULL);

    for(int i=0;i<4;i++) pthread_join(threads[i],NULL);
    gettimeofday(&t_join,NULL);

    int correct=1;
    if(stats_seq.info_array_0!=stats_par.info_array_0) correct=0;
    if(stats_seq.info_array_1!=stats_par.info_array_1) correct=0;
    if(stats_seq.info_array_2!=stats_par.info_array_2) correct=0;
    if(stats_seq.info_array_3!=stats_par.info_array_3) correct=0;

    for(int i=0;i<4;i++) free(arrays[i]);
    gettimeofday(&t_cleanup,NULL);
    gettimeofday(&t_end,NULL);

    double time_alloc   = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec)/1e6;
    double time_init    = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec)/1e6;
    double time_seq     = (t_seq.tv_sec - t_init.tv_sec) + (t_seq.tv_usec - t_init.tv_usec)/1e6;
    double time_create  = (t_compute.tv_sec - t_create.tv_sec) + (t_compute.tv_usec - t_create.tv_usec)/1e6;
    double time_compute = (t_join.tv_sec - t_compute.tv_sec) + (t_join.tv_usec - t_compute.tv_usec)/1e6;
    double time_join    = (t_cleanup.tv_sec - t_join.tv_sec) + (t_cleanup.tv_usec - t_join.tv_usec)/1e6;
    double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec)/1e6;
    double time_total   = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec)/1e6;

    printf("parallel_padded,%d,%lld,%lld,%lld,%lld,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%s\n",
        size,
        stats_par.info_array_0, stats_par.info_array_1,
        stats_par.info_array_2, stats_par.info_array_3,
        time_alloc,time_init,time_seq,time_create,
        time_compute,time_join,time_cleanup,time_total,
        correct?"PASS":"FAIL");

    return 0;
}
