#!/bin/bash

echo "Exercise 1.3 - Array Analysis"
echo "============================="

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

# Create CSV file with header if it does not exist (append otherwise)
CSV_FILE="results_1.3.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "version,size,arr0,arr1,arr2,arr3,time_alloc,time_init,time_seq_baseline,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total,verification,user" > "$CSV_FILE"
fi

for size in 100000 1000000 5000000; do
    echo "Array size: $size"
    
    echo -n "Sequential baseline - "
    output=$(./seq_arr_analysis $size)
    echo "$output"
    echo "sequential,$size,$output,$RUN_USER" >> $CSV_FILE
    
    echo -n "Parallel (4 threads) - "
    output=$(./array_analysis $size)
    echo "$output"
    echo "parallel,$size,$output,$RUN_USER" >> $CSV_FILE
    echo
done

echo "Results saved to $CSV_FILE"