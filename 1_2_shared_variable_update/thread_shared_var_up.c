#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

// Method 1: Mutex
typedef struct {
    long long *shared_var;
    long long iterations;
    pthread_mutex_t *mutex;
} mutex_data_t;

void *increment_mutex(void *arg) {
    mutex_data_t *data = (mutex_data_t *)arg;
    for (long long i = 0; i < data->iterations; i++) {
        pthread_mutex_lock(data->mutex);
        (*data->shared_var)++;
        pthread_mutex_unlock(data->mutex);
    }
    return NULL;
}

// Method 2: Read-Write Lock (used as write lock)
typedef struct {
    long long *shared_var;
    long long iterations;
    pthread_rwlock_t *rwlock;
} rwlock_data_t;

void *increment_rwlock(void *arg) {
    rwlock_data_t *data = (rwlock_data_t *)arg;
    for (long long i = 0; i < data->iterations; i++) {
        pthread_rwlock_wrlock(data->rwlock);
        (*data->shared_var)++;
        pthread_rwlock_unlock(data->rwlock);
    }
    return NULL;
}

// Method 3: Atomic Operations
typedef struct {
    long long *shared_var;
    long long iterations;
} atomic_data_t;

void *increment_atomic(void *arg) {
    atomic_data_t *data = (atomic_data_t *)arg;
    long long local_sum = 0;
    for (long long i = 0; i < data->iterations; i++) {
        local_sum++;
    }
    __atomic_fetch_add(data->shared_var, local_sum, __ATOMIC_SEQ_CST);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <method> <iterations> <threads>\n", argv[0]);
        printf("Methods: 1-mutex, 2-rwlock, 3-atomic\n");
        return 1;
    }
    
    int method = atoi(argv[1]);
    long long iterations = atoll(argv[2]);
    int num_threads = atoi(argv[3]);
    long long expected = iterations * num_threads;
    
    struct timeval t_start, t_init, t_create, t_compute, t_join, t_cleanup, t_end;
    
    gettimeofday(&t_start, NULL);
    
    long long shared_var = 0;
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    
    gettimeofday(&t_init, NULL);
    
    switch (method) {
        case 1: { // Mutex
            pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
            mutex_data_t *thread_data = malloc(num_threads * sizeof(mutex_data_t));
            
            gettimeofday(&t_create, NULL);
            for (int i = 0; i < num_threads; i++) {
                thread_data[i].shared_var = &shared_var;
                thread_data[i].iterations = iterations;
                thread_data[i].mutex = &mutex;
                pthread_create(&threads[i], NULL, increment_mutex, &thread_data[i]);
            }
            gettimeofday(&t_compute, NULL);
            
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }
            gettimeofday(&t_join, NULL);
            
            pthread_mutex_destroy(&mutex);
            free(thread_data);
            break;
        }
        case 2: { // RWLock
            pthread_rwlock_t rwlock;
            pthread_rwlock_init(&rwlock, NULL);
            rwlock_data_t *thread_data = malloc(num_threads * sizeof(rwlock_data_t));
            
            gettimeofday(&t_create, NULL);
            for (int i = 0; i < num_threads; i++) {
                thread_data[i].shared_var = &shared_var;
                thread_data[i].iterations = iterations;
                thread_data[i].rwlock = &rwlock;
                pthread_create(&threads[i], NULL, increment_rwlock, &thread_data[i]);
            }
            gettimeofday(&t_compute, NULL);
            
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }
            gettimeofday(&t_join, NULL);
            
            pthread_rwlock_destroy(&rwlock);
            free(thread_data);
            break;
        }
        case 3: { // Atomic
            atomic_data_t *thread_data = malloc(num_threads * sizeof(atomic_data_t));
            
            gettimeofday(&t_create, NULL);
            for (int i = 0; i < num_threads; i++) {
                thread_data[i].shared_var = &shared_var;
                thread_data[i].iterations = iterations;
                pthread_create(&threads[i], NULL, increment_atomic, &thread_data[i]);
            }
            gettimeofday(&t_compute, NULL);
            
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }
            gettimeofday(&t_join, NULL);
            
            free(thread_data);
            break;
        }
    }
    
    gettimeofday(&t_cleanup, NULL);
    free(threads);
    gettimeofday(&t_end, NULL);
    
    double time_init = (t_init.tv_sec - t_start.tv_sec) + (t_init.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_create = (t_compute.tv_sec - t_create.tv_sec) + (t_compute.tv_usec - t_create.tv_usec) / 1000000.0;
    double time_compute = (t_join.tv_sec - t_compute.tv_sec) + (t_join.tv_usec - t_compute.tv_usec) / 1000000.0;
    double time_join = (t_cleanup.tv_sec - t_join.tv_sec) + (t_cleanup.tv_usec - t_join.tv_usec) / 1000000.0;
    double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%lld,%f,%f,%f,%f,%f,%f,%s\n",
           shared_var, time_init, time_create, time_compute, 
           time_join, time_cleanup, time_total,
           (shared_var == expected) ? "PASS" : "FAIL");
    
    return 0;
}