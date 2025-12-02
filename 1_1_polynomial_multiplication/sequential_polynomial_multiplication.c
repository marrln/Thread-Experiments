#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void multiply_polynomials_seq(int *A, int *B, int *C, int n) {
    int size = 2 * n + 1;
    
    // Same algorithm as threaded version: accumulate in local variable, write once
    for (int k = 0; k < size; k++) {
        int i_start = (k < n) ? 0 : k - n;
        int i_end   = (k < n) ? k : n;
        
        int sum = 0;
        for (int i = i_start; i <= i_end; i++) {
            sum += A[i] * B[k - i];
        }
        C[k] = sum;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <degree> <num_threads>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    // num_threads parameter accepted but not used in sequential version
    int size = 2 * n + 1;
    
    struct timeval t_start, t_alloc, t_init, t_compute, t_end;
    
    gettimeofday(&t_start, NULL);
    
    int *A = malloc((n + 1) * sizeof(int));
    int *B = malloc((n + 1) * sizeof(int));
    int *C_seq = calloc(size, sizeof(int));
    
    gettimeofday(&t_alloc, NULL);
    
    // Initialize polynomials with random non-zero coefficients
    srand(time(NULL));
    for (int i = 0; i <= n; i++) {
        A[i] = (rand() % 9) + 1; // 1-9
        B[i] = (rand() % 9) + 1;
    }
    
    gettimeofday(&t_init, NULL);
    
    multiply_polynomials_seq(A, B, C_seq, n);
    
    gettimeofday(&t_compute, NULL);
    
    free(A); free(B); free(C_seq);
    
    gettimeofday(&t_end, NULL);
    
    // Calculate times
    double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
    double time_seq = (t_compute.tv_sec - t_init.tv_sec) + (t_compute.tv_usec - t_init.tv_usec) / 1000000.0;
    double time_cleanup = (t_end.tv_sec - t_compute.tv_sec) + (t_end.tv_usec - t_compute.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
        printf("%f,%f,%f,0.0,0.0,0.0,%f,%f\n",
            time_alloc, time_init, time_seq, time_cleanup, time_total);
    
    return 0;
}