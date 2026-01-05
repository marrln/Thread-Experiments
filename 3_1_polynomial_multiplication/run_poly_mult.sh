#!/bin/bash

echo "Exercise 3.1 - Polynomial Multiplication"
echo "========================================"

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

# Create CSV file with header if it does not exist (append otherwise)
CSV_FILE="results_3.1.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "degree,threads,time_alloc,time_init,time_thread_create,time_compute,time_join,time_reduce,time_verify,time_cleanup,time_total,verification,user" > "$CSV_FILE"
fi

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
BIN="$SCRIPTDIR/bin/thread_poly_mult"
if [ ! -x "$BIN" ]; then
    echo "Binary $BIN not found. Please run the top-level script (run_experiments_3.sh) to compile all binaries.";
    exit 1
fi

for degree in 5000 20000 80000 100000; do
    echo "Degree: $degree"
    
    # Sequential baseline
    echo -n "Sequential baseline - "
    output=$(./bin/seq_poly_mult $degree 1)
    echo "$output"
    echo "$degree,sequential,$output,$RUN_USER" >> $CSV_FILE
    
    # Parallel versions
    for threads in 1 2 4 8 16; do
        echo -n "Threads: $threads - "
        if command -v mpirun >/dev/null 2>&1; then
            output=$(mpirun --oversubscribe -np $threads "$BIN" $degree)
        else
            output=$(mpiexec --oversubscribe -n $threads "$BIN" $degree)
        fi
        echo "$output"
        echo "$degree,$threads,$output,$RUN_USER" >> $CSV_FILE
    done
    echo
done

echo "Results saved to $CSV_FILE"