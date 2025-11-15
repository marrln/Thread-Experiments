#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

double perform_work(int value) {
    // Simulate work by calculating square root multiple times
    double result = 0;
    for (int i = 0; i < 100; i++) {
        result += sqrt(value + i);
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <num_accounts> <transactions_per_thread> <query_percentage> <num_threads>\n", argv[0]);
        return 1;
    }
    
    int num_accounts = atoi(argv[1]);
    int transactions_per_thread = atoi(argv[2]);
    double query_percentage = atof(argv[3]);
    int num_threads = atoi(argv[4]);
    int total_transactions = transactions_per_thread * num_threads;
    
    struct timeval t_start, t_alloc, t_init, t_compute, t_end;
    
    gettimeofday(&t_start, NULL);
    
    // Initialize accounts
    int *accounts = malloc(num_accounts * sizeof(int));
    
    gettimeofday(&t_alloc, NULL);
    
    srand(time(NULL));
    for (int i = 0; i < num_accounts; i++) {
        accounts[i] = (rand() % 100) + 1; // Initial balance 1-100
    }
    
    gettimeofday(&t_init, NULL);
    
    long query_sum = 0;
    for (int t = 0; t < total_transactions; t++) {
        if ((double)rand() / RAND_MAX < query_percentage) {
            // Query transaction
            int account = rand() % num_accounts;
            int balance = accounts[account];
            
            // Perform work with the balance value
            perform_work(balance);
            query_sum += balance;
        } else {
            // Transfer transaction
            int from = rand() % num_accounts;
            int to = rand() % num_accounts;
            while (to == from) to = rand() % num_accounts;
            int amount = (rand() % 10) + 1;
            
            if (accounts[from] >= amount) {
                accounts[from] -= amount;
                accounts[to] += amount;
            }
        }
    }
    
    gettimeofday(&t_compute, NULL);
    
    free(accounts);
    gettimeofday(&t_end, NULL);
    
    double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
    double time_compute = (t_compute.tv_sec - t_init.tv_sec) + (t_compute.tv_usec - t_init.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%f,%f,0.0,%f,0.0,0.0,%f\n",
           time_alloc, time_init, time_compute, time_total);
    
    return 0;
}