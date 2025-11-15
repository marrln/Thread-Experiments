#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <method> <iterations> <num_threads>\n", argv[0]);
        return 1;
    }
    
    // method parameter accepted but ignored in sequential version
    long long iterations = atoll(argv[2]);
    int num_threads = atoi(argv[3]);
    long long total_iterations = iterations * num_threads;
    
    struct timeval t_start, t_init, t_compute, t_end;
    
    gettimeofday(&t_start, NULL);
    
    long long shared_var = 0;
    
    gettimeofday(&t_init, NULL);
    
    for (long long i = 0; i < total_iterations; i++) {
        shared_var++;
    }
    
    gettimeofday(&t_compute, NULL);
    gettimeofday(&t_end, NULL);
    
    double time_init = (t_init.tv_sec - t_start.tv_sec) + (t_init.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_compute = (t_compute.tv_sec - t_init.tv_sec) + (t_compute.tv_usec - t_init.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%lld,%f,0.0,%f,0.0,0.0,%f,PASS\n",
           shared_var, time_init, time_compute, time_total);
    
    return 0;
}