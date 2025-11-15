#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

typedef struct {
    int *accounts;
    int num_accounts;
    int transactions_per_thread;
    double query_percentage;
    int num_threads;
    int locking_scheme; // 1: coarse-grained, 2: fine-grained, 3: read-write
    pthread_mutex_t *mutexes;
    pthread_rwlock_t *rwlocks;
    pthread_mutex_t global_mutex;
} bank_data_t;

typedef struct {
    bank_data_t *bank_data;
    int thread_id;
    long query_sum;
} thread_data_t;

double perform_work(int value) {
    // Simulate work by calculating square root multiple times
    double result = 0;
    for (int i = 0; i < 100; i++) {
        result += sqrt(value + i);
    }
    return result;
}

void *bank_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    bank_data_t *bank = data->bank_data;
    
    for (int t = 0; t < bank->transactions_per_thread; t++) {
        if ((double)rand() / RAND_MAX < bank->query_percentage) {
            // Query transaction
            int account = rand() % bank->num_accounts;
            
            if (bank->locking_scheme == 1) { // Coarse-grained
                pthread_mutex_lock(&bank->global_mutex);
                int balance = bank->accounts[account];
                pthread_mutex_unlock(&bank->global_mutex);
                
                // Perform work with the balance value
                perform_work(balance);
                data->query_sum += balance;
            }
            else if (bank->locking_scheme == 2) { // Fine-grained
                pthread_mutex_lock(&bank->mutexes[account]);
                int balance = bank->accounts[account];
                pthread_mutex_unlock(&bank->mutexes[account]);
                
                perform_work(balance);
                data->query_sum += balance;
            }
            else { // Read-write locks
                pthread_rwlock_rdlock(&bank->rwlocks[account]);
                int balance = bank->accounts[account];
                pthread_rwlock_unlock(&bank->rwlocks[account]);
                
                perform_work(balance);
                data->query_sum += balance;
            }
        } else {
            // Transfer transaction
            int from = rand() % bank->num_accounts;
            int to = rand() % bank->num_accounts;
            while (to == from) to = rand() % bank->num_accounts;
            int amount = (rand() % 10) + 1;
            
            if (bank->locking_scheme == 1) { // Coarse-grained
                pthread_mutex_lock(&bank->global_mutex);
                if (bank->accounts[from] >= amount) {
                    bank->accounts[from] -= amount;
                    bank->accounts[to] += amount;
                }
                pthread_mutex_unlock(&bank->global_mutex);
            }
            else if (bank->locking_scheme == 2) { // Fine-grained
                // Lock in order to prevent deadlock
                int first = (from < to) ? from : to;
                int second = (from < to) ? to : from;
                
                pthread_mutex_lock(&bank->mutexes[first]);
                pthread_mutex_lock(&bank->mutexes[second]);
                
                if (bank->accounts[from] >= amount) {
                    bank->accounts[from] -= amount;
                    bank->accounts[to] += amount;
                }
                
                pthread_mutex_unlock(&bank->mutexes[second]);
                pthread_mutex_unlock(&bank->mutexes[first]);
            }
            else { // Read-write locks
                int first = (from < to) ? from : to;
                int second = (from < to) ? to : from;
                
                pthread_rwlock_wrlock(&bank->rwlocks[first]);
                pthread_rwlock_wrlock(&bank->rwlocks[second]);
                
                if (bank->accounts[from] >= amount) {
                    bank->accounts[from] -= amount;
                    bank->accounts[to] += amount;
                }
                
                pthread_rwlock_unlock(&bank->rwlocks[second]);
                pthread_rwlock_unlock(&bank->rwlocks[first]);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <num_accounts> <transactions_per_thread> <query_percentage> <locking_scheme> <num_threads>\n", argv[0]);
        printf("Locking schemes: 1-coarse, 2-fine, 3-rwlock\n");
        return 1;
    }
    
    int num_accounts = atoi(argv[1]);
    int transactions_per_thread = atoi(argv[2]);
    double query_percentage = atof(argv[3]);
    int locking_scheme = atoi(argv[4]);
    int num_threads = atoi(argv[5]);
    
    struct timeval t_start, t_alloc, t_init, t_create, t_compute, t_join, t_cleanup, t_end;
    
    gettimeofday(&t_start, NULL);
    
    // Initialize bank data
    bank_data_t bank_data;
    bank_data.accounts = malloc(num_accounts * sizeof(int));
    bank_data.num_accounts = num_accounts;
    bank_data.transactions_per_thread = transactions_per_thread;
    bank_data.query_percentage = query_percentage;
    bank_data.locking_scheme = locking_scheme;
    bank_data.num_threads = num_threads;
    
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(num_threads * sizeof(thread_data_t));
    
    gettimeofday(&t_alloc, NULL);
    
    srand(time(NULL));
    for (int i = 0; i < num_accounts; i++) {
        bank_data.accounts[i] = (rand() % 100) + 1; // Initial balance 1-100
    }
    
    // Initialize locks based on scheme
    if (locking_scheme == 1) {
        pthread_mutex_init(&bank_data.global_mutex, NULL);
    } else if (locking_scheme == 2) {
        bank_data.mutexes = malloc(num_accounts * sizeof(pthread_mutex_t));
        for (int i = 0; i < num_accounts; i++) {
            pthread_mutex_init(&bank_data.mutexes[i], NULL);
        }
    } else {
        bank_data.rwlocks = malloc(num_accounts * sizeof(pthread_rwlock_t));
        for (int i = 0; i < num_accounts; i++) {
            pthread_rwlock_init(&bank_data.rwlocks[i], NULL);
        }
    }
    
    gettimeofday(&t_init, NULL);
    gettimeofday(&t_create, NULL);
    
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].bank_data = &bank_data;
        thread_data[i].thread_id = i;
        thread_data[i].query_sum = 0;
        pthread_create(&threads[i], NULL, bank_thread, &thread_data[i]);
    }
    gettimeofday(&t_compute, NULL);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&t_join, NULL);
    
    // Cleanup
    if (locking_scheme == 1) {
        pthread_mutex_destroy(&bank_data.global_mutex);
    } else if (locking_scheme == 2) {
        for (int i = 0; i < num_accounts; i++) pthread_mutex_destroy(&bank_data.mutexes[i]);
        free(bank_data.mutexes);
    } else if (locking_scheme == 3) {
        for (int i = 0; i < num_accounts; i++) pthread_rwlock_destroy(&bank_data.rwlocks[i]);
        free(bank_data.rwlocks);
    }
    
    gettimeofday(&t_cleanup, NULL);
    
    free(bank_data.accounts);
    free(threads);
    free(thread_data);
    
    gettimeofday(&t_end, NULL);
    
    double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
    double time_create = (t_compute.tv_sec - t_create.tv_sec) + (t_compute.tv_usec - t_create.tv_usec) / 1000000.0;
    double time_compute = (t_join.tv_sec - t_compute.tv_sec) + (t_join.tv_usec - t_compute.tv_usec) / 1000000.0;
    double time_join = (t_cleanup.tv_sec - t_join.tv_sec) + (t_cleanup.tv_usec - t_join.tv_usec) / 1000000.0;
    double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%f,%f,%f,%f,%f,%f,%f\n",
           time_alloc, time_init, time_create, time_compute,
           time_join, time_cleanup, time_total);
    
    return 0;
}