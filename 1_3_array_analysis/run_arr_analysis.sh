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

# Array sizes to test
for size in 100000 1000000 5000000 10000000 20000000; do
    echo "Array size: $size"

    # Sequential baseline
    echo -n "Sequential baseline - "
    output=$(./bin/seq_arr_analysis $size)
    echo "$output"
    echo "$output,$RUN_USER" | sed "s/^/sequential,/" >> $CSV_FILE

    # Parallel without padding
    echo -n "Parallel (unpadded) - "
    output=$(./bin/thread_arr_analysis_unpadded $size)
    echo "$output"
    echo "$output,$RUN_USER" | sed "s/^/parallel_unpadded,/" >> $CSV_FILE

    # Parallel with padding
    echo -n "Parallel (padded) - "
    output=$(./bin/thread_arr_analysis_padded $size)
    echo "$output"
    echo "$output,$RUN_USER" | sed "s/^/parallel_padded,/" >> $CSV_FILE

    echo
done

echo "Results saved to $CSV_FILE"
