#!/bin/bash

echo "Exercise 1.1 - Polynomial Multiplication"
echo "========================================"

# Create CSV file with header
CSV_FILE="results_1.1.csv"
echo "degree,threads,time_alloc,time_init,time_seq_baseline,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total,verification" > $CSV_FILE

for degree in 1000 5000 10000; do
    echo "Degree: $degree"
    
    # Sequential baseline
    echo -n "Sequential baseline - "
    output=$(./seq_poly_mult $degree 1)
    echo "$output"
    echo "$degree,sequential,$output" >> $CSV_FILE
    
    # Parallel versions
    for threads in 1 2 4 8; do
        echo -n "Threads: $threads - "
        output=$(./poly_mult $degree $threads)
        echo "$output"
        echo "$degree,$threads,$output" >> $CSV_FILE
    done
    echo
done

echo "Results saved to $CSV_FILE"