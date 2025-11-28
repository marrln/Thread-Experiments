#!/bin/bash

echo "Exercise 1.2 - Shared Variable Update"
echo "====================================="

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

# Create CSV file with header if it does not exist (append otherwise)
CSV_FILE="results_1.2.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "method,iterations,threads,final_value,time_init,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total,verification,user" > "$CSV_FILE"
fi

for iterations in 100000 1000000; do
    for threads in 1 2 4 8; do
        echo "Iterations: $iterations, Threads: $threads"
        
        # Sequential baseline
        echo -n "Sequential baseline - "
        output=$(./bin/seq_shared_var 1 $iterations $threads)
        echo "$output"
        echo "sequential,$iterations,$threads,$output,$RUN_USER" >> $CSV_FILE
        
        # Parallel methods
        for method in 1 2 3; do
            method_name="mutex"
            if [ $method -eq 2 ]; then method_name="rwlock"; fi
            if [ $method -eq 3 ]; then method_name="atomic"; fi
            
            echo -n "Method $method ($method_name) - "
            output=$(./bin/shared_var $method $iterations $threads)
            echo "$output"
            echo "$method_name,$iterations,$threads,$output,$RUN_USER" >> $CSV_FILE
        done
        echo
    done
done

echo "Results saved to $CSV_FILE"