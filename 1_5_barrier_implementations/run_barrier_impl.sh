#!/bin/bash

echo "Exercise 1.5 - Barrier Implementations"
echo "======================================"

# Create CSV file with header
CSV_FILE="results_1.5.csv"
echo "barrier_type,iterations,threads,time_init,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total" > $CSV_FILE

for iterations in 10000 50000 100000; do
    for threads in 2 4 8 16; do
        echo "Iterations: $iterations, Threads: $threads"
        
        echo -n "Pthread Barrier: "
        output=$(./barrier_pthread $iterations $threads)
        echo "$output"
        echo "pthread,$iterations,$threads,$output" >> $CSV_FILE
        
        echo -n "Condition Barrier: "
        output=$(./barrier_cond $iterations $threads)
        echo "$output"
        echo "cond_var,$iterations,$threads,$output" >> $CSV_FILE
        
        echo -n "Sense-Reversal Barrier: "
        output=$(./barrier_sense $iterations $threads)
        echo "$output"
        echo "sense_reversal,$iterations,$threads,$output" >> $CSV_FILE
        
        echo
    done
done

echo "Results saved to $CSV_FILE"