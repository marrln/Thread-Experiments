#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

typedef struct {
    int *A, *B;
    int *C;        // PRIVATE buffer for each thread
    int n;
    int thread_id, num_threads;
} thread_data_t;

void *multiply_polynomials_parallel(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    int size = 2 * data->n + 1;
    int chunk = (size + data->num_threads - 1) / data->num_threads;

    int start = data->thread_id * chunk;
    int end = start + chunk;
    if (end > size) end = size;

    for (int k = start; k < end; k++) {

        int i_start = (k < data->n) ? 0 : k - data->n;
        int i_end   = (k < data->n) ? k : data->n;

        int sum = 0;
        for (int i = i_start; i <= i_end; i++) {
            sum += data->A[i] * data->B[k - i];
        }
        data->C[k] = sum;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <degree> <num_threads>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int size = 2 * n + 1;
    
    struct timeval t_start, t_alloc, t_init, t_create, t_compute, t_join, t_verify, t_end;
    
    gettimeofday(&t_start, NULL);
    
    int *A = malloc((n + 1) * sizeof(int));
    int *B = malloc((n + 1) * sizeof(int));
    int *C_par = calloc(size, sizeof(int));
    int *C_seq = calloc(size, sizeof(int));

    // Allocate private C arrays
    int **C_priv = malloc(num_threads * sizeof(int *));
    for (int i = 0; i < num_threads; i++) {
        C_priv[i] = calloc(size, sizeof(int));
    }
    
    gettimeofday(&t_alloc, NULL);
    
    srand(time(NULL));
    for (int i = 0; i <= n; i++) {
        A[i] = (rand() % 9) + 1;
        B[i] = (rand() % 9) + 1;
    }
    
    gettimeofday(&t_init, NULL);
    
    struct timeval start_seq, end_seq;
    gettimeofday(&start_seq, NULL);
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            C_seq[i + j] += A[i] * B[j];
        }
    }
    gettimeofday(&end_seq, NULL);
    
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(num_threads * sizeof(thread_data_t));
    
    gettimeofday(&t_create, NULL);
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C_priv[i];   // <-- PRIVATE BUFFER
        thread_data[i].n = n;
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        pthread_create(&threads[i], NULL, multiply_polynomials_parallel, &thread_data[i]);
    }
    
    gettimeofday(&t_compute, NULL);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&t_join, NULL);

    // NEW: reduce private buffers into final C_par
    for (int t = 0; t < num_threads; t++) {
        for (int k = 0; k < size; k++) {
            C_par[k] += C_priv[t][k];
        }
    }
    
    // Verify
    int correct = 1;
    for (int i = 0; i < size; i++) {
        if (C_seq[i] != C_par[i]) {
            correct = 0;
            break;
        }
    }
    
    gettimeofday(&t_verify, NULL);

    // free memory
    for (int i = 0; i < num_threads; i++)
        free(C_priv[i]);
    free(C_priv);

    free(A); free(B); free(C_par); free(C_seq);
    free(threads); free(thread_data);
    
    gettimeofday(&t_end, NULL);
    
    double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
    double time_seq = (end_seq.tv_sec - start_seq.tv_sec) + (end_seq.tv_usec - start_seq.tv_usec) / 1000000.0;
    double time_create = (t_compute.tv_sec - t_create.tv_sec) + (t_compute.tv_usec - t_create.tv_usec) / 1000000.0;
    double time_compute = (t_join.tv_sec - t_compute.tv_sec) + (t_join.tv_usec - t_compute.tv_usec) / 1000000.0;
    double time_join = (t_verify.tv_sec - t_join.tv_sec) + (t_verify.tv_usec - t_join.tv_usec) / 1000000.0;
    double time_verify = (t_end.tv_sec - t_verify.tv_sec) + (t_end.tv_usec - t_verify.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%f,%f,%f,%f,%f,%f,%f,%f,%s\n",
           time_alloc, time_init, time_seq, time_create, 
           time_compute, time_join, time_verify, time_total,
           correct ? "PASS" : "FAIL");
    
    return 0;
}
