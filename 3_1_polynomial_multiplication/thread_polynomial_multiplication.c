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

}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <degree> <num_threads>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int size = 2 * n + 1;

    return 0;
}
