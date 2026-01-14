#!/bin/bash

echo "Exercise 2.2 - Sparse matrix-vector (CSR)"
echo "========================================"

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

CSV_FILE="results_2.2.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "n,threads,sparsity,reps,nnz,time_init,time_csr_construct,time_spmv_total,time_dense_total,verification,user" > "$CSV_FILE"
fi

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
BIN="$SCRIPTDIR/bin/sparse_arr_vector_mult"
if [ ! -x "$BIN" ]; then
    echo "Binary $BIN not found. Please run the top-level script (run_experiments_2.sh) to compile all binaries.";
    exit 1
fi

# Experiment parameters
NS=(1000 4000 6000 10000)
SPARS=(0 50 90 95 99)
REPS=(1 5 10)
THREADS=(1 2 4 8)

for n in "${NS[@]}"; do
    for s in "${SPARS[@]}"; do
        for reps in "${REPS[@]}"; do
            # sequential baseline (label as 'sequential')
            echo -n "Running (sequential) n=$n sparsity=$s reps=$reps ... "
            raw=$($BIN $n $s $reps 1)
            echo "$raw"
            # reorder fields: raw is n,sparsity,reps,threads,nnz,... -> we want n,threads,sparsity,reps,nnz,... with threads='sequential'
            line=$(echo "$raw" | awk -F',' '{printf "%s,%s,%s,%s", $1, "sequential", $2, $3; for(i=5;i<=NF;i++) printf ",%s", $i; printf "\n" }')
            echo "$line,$RUN_USER" >> "$CSV_FILE"

            # parallel runs
            for th in "${THREADS[@]}"; do
                if [ "$th" -eq 1 ]; then continue; fi
                echo -n "Running n=$n sparsity=$s reps=$reps threads=$th ... "
                raw=$($BIN $n $s $reps $th)
                echo "$raw"
                # reorder raw to put threads second: n,sparsity,reps,threads,nnz,... -> n,threads,sparsity,reps,nnz,...
                line=$(echo "$raw" | awk -F',' '{printf "%s,%s,%s,%s", $1, $4, $2, $3; for(i=5;i<=NF;i++) printf ",%s", $i; printf "\n" }')
                echo "$line,$RUN_USER" >> "$CSV_FILE"
            done
        done
    done
done

echo "Results saved to $CSV_FILE"
