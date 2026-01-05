#!/bin/bash

echo "Exercise 3.2 - Sparse matrix-vector (MPI, CSR)"
echo "============================================"

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

CSV_FILE="results_3.2.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "n,procs,sparsity,reps,nnz,time_csr_construct,time_send,time_spmv,time_csr_total,time_dense_total,verification,user" > "$CSV_FILE"
fi

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
BIN="$SCRIPTDIR/bin/sparse_arr_vector_mult"
if [ ! -x "$BIN" ]; then
    echo "Binary $BIN not found. Please run the top-level script (run_experiments_3.sh) to compile all binaries.";
    exit 1
fi

# Experiment parameters (customize as needed)
NS=(1000 2000)
SPARS=(0 90 99)
REPS=(1 5)
PROCS=(1 2 4)

for n in "${NS[@]}"; do
    for s in "${SPARS[@]}"; do
        for reps in "${REPS[@]}"; do
                    # sequential baseline (label as 'sequential') when procs = 1
            echo -n "Running (sequential) n=$n sparsity=$s reps=$reps ... "
            if command -v mpirun >/dev/null 2>&1; then
                raw=$(mpirun --oversubscribe -np 1 "$BIN" $n $s $reps)
            else
                raw=$(mpiexec --oversubscribe -n 1 "$BIN" $n $s $reps)
            fi
            echo "$raw"
            # reorder raw: n,sparsity,reps,procs,nnz,... -> n,procs,sparsity,reps,nnz,... with procs = 'sequential'
            line=$(echo "$raw" | awk -F',' '{printf "%s,%s,%s,%s", $1, "sequential", $2, $3; for(i=5;i<=NF;i++) printf ",%s", $i; printf "\n" }')
            echo "$line,$RUN_USER" >> "$CSV_FILE"

            for p in "${PROCS[@]}"; do
                if [ "$p" -eq 1 ]; then continue; fi
                echo -n "Running n=$n sparsity=$s reps=$reps procs=$p ... "
                if command -v mpirun >/dev/null 2>&1; then
                    raw=$(mpirun --oversubscribe -np $p "$BIN" $n $s $reps)
                else
                    raw=$(mpiexec --oversubscribe -n $p "$BIN" $n $s $reps)
                fi
                echo "$raw"
                # reorder raw to put procs second: n,sparsity,reps,procs,nnz,... -> n,procs,sparsity,reps,nnz,...
                line=$(echo "$raw" | awk -F',' '{printf "%s,%s,%s,%s", $1, $4, $2, $3; for(i=5;i<=NF;i++) printf ",%s", $i; printf "\n" }')
                echo "$line,$RUN_USER" >> "$CSV_FILE"
            done
        done
    done
done

echo "Results saved to $CSV_FILE"
