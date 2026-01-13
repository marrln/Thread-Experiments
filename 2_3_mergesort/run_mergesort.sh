#!/bin/bash

echo "Exercise 2.3 - Mergesort (OpenMP)"
echo "================================"

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

CSV_FILE="results_2.3.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "n,threads,time_alloc,time_init,time_thread_create,time_compute,time_join,time_reduce,time_verify,time_cleanup,time_total,verification,user" > "$CSV_FILE"
fi

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
BIN="$SCRIPTDIR/bin/mergesort"
if [ ! -x "$BIN" ]; then
    echo "Binary $BIN not found. Please run the top-level script (run_experiments_2.sh) to compile all binaries.";
    exit 1
fi

NS=(10000 50000 100000)
THREADS=(sequential 1 2 4 8)

for n in "${NS[@]}"; do
    for th in "${THREADS[@]}"; do
        echo -n "Running n=$n threads=$th ... "
        raw=$($BIN $n $th)
        echo "$raw"
        echo "$raw,$RUN_USER" >> "$CSV_FILE"
    done
    echo
done

echo "Results saved to $CSV_FILE"
