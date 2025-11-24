#!/bin/bash

echo "Exercise 1.3 - Array Analysis"
echo "============================="

# Create CSV file with header
CSV_FILE="results_1.3.csv"
echo "version,size,arr0,arr1,arr2,arr3,time_alloc,time_init,time_seq_baseline,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total,verification" > $CSV_FILE

for size in 100000 1000000 5000000; do
    echo "Array size: $size"
    
    echo -n "Sequential baseline - "
    output=$(./seq_arr_analysis $size)
    echo "$output"
    echo "sequential,$size,$output" >> $CSV_FILE
    
    echo -n "Parallel (4 threads) - "
    output=$(./array_analysis $size)
    echo "$output"
    echo "parallel,$size,$output" >> $CSV_FILE
    echo
done

echo "Results saved to $CSV_FILE"