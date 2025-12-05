#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

typedef struct {
    int count;
    int waiting;
    int sense;
    pthread_mutex_t mutex;
} sense_barrier_t;

void sense_barrier_init(sense_barrier_t *barrier, int count) {
    barrier->count = count;
    barrier->waiting = 0;
    barrier->sense = 0;
    pthread_mutex_init(&barrier->mutex, NULL);
}

void sense_barrier_wait(sense_barrier_t *barrier, int *local_sense) {
    *local_sense = !(*local_sense);
    int my_sense = *local_sense;
    
    pthread_mutex_lock(&barrier->mutex);
    barrier->waiting++;
    
    if (barrier->waiting == barrier->count) {
        barrier->waiting = 0;
        barrier->sense = my_sense;
        pthread_mutex_unlock(&barrier->mutex);
    } else {
        pthread_mutex_unlock(&barrier->mutex);
        while (barrier->sense != my_sense) {
            // Busy wait - reads are safe without lock
        }
    }
}

void sense_barrier_destroy(sense_barrier_t *barrier) {
    pthread_mutex_destroy(&barrier->mutex);
}

typedef struct {
    int thread_id;
    int num_threads;
    int iterations;
    sense_barrier_t *barrier;
} sense_barrier_data_t;

void *sense_barrier_thread(void *arg) {
    sense_barrier_data_t *data = (sense_barrier_data_t *)arg;
    int local_sense = 0;
    
    for (int i = 0; i < data->iterations; i++) {
        // Simulate some work
        for (int j = 0; j < 100; j++);
        
        sense_barrier_wait(data->barrier, &local_sense);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <iterations> <num_threads>\n", argv[0]);
        return 1;
    }
    
    int iterations = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    
    struct timeval t_start, t_init, t_create, t_compute, t_join, t_cleanup, t_end;
    
    gettimeofday(&t_start, NULL);
    
    sense_barrier_t barrier;
    sense_barrier_init(&barrier, num_threads);
    
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    sense_barrier_data_t *thread_data = malloc(num_threads * sizeof(sense_barrier_data_t));
    
    gettimeofday(&t_init, NULL);
    gettimeofday(&t_create, NULL);
    
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].iterations = iterations;
        thread_data[i].barrier = &barrier;
        pthread_create(&threads[i], NULL, sense_barrier_thread, &thread_data[i]);
    }
    gettimeofday(&t_compute, NULL);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&t_join, NULL);
    
    sense_barrier_destroy(&barrier);
    gettimeofday(&t_cleanup, NULL);
    
    free(threads);
    free(thread_data);
    gettimeofday(&t_end, NULL);
    
    double time_init = (t_init.tv_sec - t_start.tv_sec) + (t_init.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_create = (t_compute.tv_sec - t_create.tv_sec) + (t_compute.tv_usec - t_create.tv_usec) / 1000000.0;
    double time_compute = (t_join.tv_sec - t_compute.tv_sec) + (t_join.tv_usec - t_compute.tv_usec) / 1000000.0;
    double time_join = (t_cleanup.tv_sec - t_join.tv_sec) + (t_cleanup.tv_usec - t_join.tv_usec) / 1000000.0;
    double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%f,%f,%f,%f,%f,%f\n",
           time_init, time_create, time_compute, time_join, time_cleanup, time_total);
    
    return 0;
}