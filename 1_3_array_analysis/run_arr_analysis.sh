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

# Array sizes to test - as suggested in exercise (10^5, 10^6) plus larger for speedup
for size in 100000 1000000 10000000 50000000 100000000; do
    echo "Array size: $size"

    # Sequential baseline
    echo -n "Sequential baseline - "
    output=$(./bin/seq_arr_analysis $size)
    echo "$output"
    echo "$output,$RUN_USER" >> $CSV_FILE

    # Parallel without padding
    echo -n "Parallel (unpadded) - "
    output=$(./bin/thread_arr_analysis_unpadded $size)
    echo "$output"
    echo "$output,$RUN_USER" >> $CSV_FILE

    # Parallel with padding
    echo -n "Parallel (padded) - "
    output=$(./bin/thread_arr_analysis_padded $size)
    echo "$output"
    echo "$output,$RUN_USER" >> $CSV_FILE

    # Parallel with local accumulation (best performance)
    echo -n "Parallel (local accum) - "
    output=$(./bin/thread_arr_analysis_local_accum $size)
    echo "$output"
    echo "$output,$RUN_USER" >> $CSV_FILE

    echo
done

echo "Results saved to $CSV_FILE"
