#!/bin/bash

set -e

echo "Running Sequential Sparse Array-Vector Multiplication Experiments"
echo "=================================================================="
echo

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi
export RUN_USER

# Create bin directories if they don't exist
mkdir -p 2_2_sparse_array_vector_multiplication/bin
mkdir -p 3_2_sparse_array_vector_multiplication/bin

# Compile sequential binaries
echo "Compiling sequential binaries..."
gcc -Wall -O2 -pthread -o 2_2_sparse_array_vector_multiplication/bin/seq_sparse_arr_vector_mult \
    2_2_sparse_array_vector_multiplication/sequential_sparse_arr_vector_mult.c -lm

gcc -Wall -O2 -pthread -o 3_2_sparse_array_vector_multiplication/bin/seq_sparse_arr_vector_mult \
    3_2_sparse_array_vector_multiplication/sequential_sparse_arr_vector_mult.c -lm

echo "Compilation finished."
echo

# Experiment parameters (same as in the main scripts)
NS=(1000 4000 6000 10000)
SPARS=(0 50 90 95 99)
REPS=(1 5 10)

# 2_2 Sequential runs
CSV_2_2="2_2_sparse_array_vector_multiplication/results_2.2.csv"
SEQ_BIN_2_2="2_2_sparse_array_vector_multiplication/bin/seq_sparse_arr_vector_mult"

if [ ! -f "$CSV_2_2" ]; then
    echo "n,threads,sparsity,reps,nnz,time_init,time_csr_construct,time_spmv_total,time_dense_total,verification,user" > "$CSV_2_2"
fi

echo "Running Exercise 2.2 - Sequential Sparse Array-Vector Multiplication (5 runs)..."
for run in $(seq 1 5); do
    echo "  Run $run/5..."
    for n in "${NS[@]}"; do
        for s in "${SPARS[@]}"; do
            for reps in "${REPS[@]}"; do
                echo -n "    n=$n sparsity=$s reps=$reps ... "
                raw=$($SEQ_BIN_2_2 $n $s $reps)
                echo "$raw"
                # seq output: n,sparsity,reps,nnz,time_init,time_csr_construct,time_spmv_total,time_dense_total,verification
                # target: n,threads,sparsity,reps,nnz,time_init,time_csr_construct,time_spmv_total,time_dense_total,verification,user
                line=$(echo "$raw" | awk -F',' '{printf "%s,%s,%s,%s", $1, "sequential", $2, $3; for(i=4;i<=NF;i++) printf ",%s", $i; printf "\n" }')
                echo "$line,$RUN_USER" >> "$CSV_2_2"
            done
        done
    done
done
echo "Exercise 2.2 sequential runs completed. Results saved to $CSV_2_2"
echo

# 3_2 Sequential runs
CSV_3_2="3_2_sparse_array_vector_multiplication/results_3.2.csv"
SEQ_BIN_3_2="3_2_sparse_array_vector_multiplication/bin/seq_sparse_arr_vector_mult"

if [ ! -f "$CSV_3_2" ]; then
    echo "n,procs,sparsity,reps,nnz,time_csr_construct,time_send,time_spmv,time_csr_total,time_dense_total,verification,user" > "$CSV_3_2"
fi

echo "Running Exercise 3.2 - Sequential Sparse Array-Vector Multiplication (5 runs)..."
for run in $(seq 1 5); do
    echo "  Run $run/5..."
    for n in "${NS[@]}"; do
        for s in "${SPARS[@]}"; do
            for reps in "${REPS[@]}"; do
                echo -n "    n=$n sparsity=$s reps=$reps ... "
                raw=$($SEQ_BIN_3_2 $n $s $reps)
                echo "$raw"
                # seq output: n,sparsity,reps,nnz,time_csr_construct,time_spmv_total,time_dense_total,time_total,verification
                # target: n,procs,sparsity,reps,nnz,time_csr_construct,time_send,time_spmv,time_csr_total,time_dense_total,verification,user
                # set time_send=0 for sequential
                line=$(echo "$raw" | awk -F',' '{printf "%s,%s,%s,%s,%s,%s,0.0,%s,%s,%s,%s", $1, "sequential", $2, $3, $4, $5, $6, $8, $7, $9; printf "\n" }')
                echo "$line,$RUN_USER" >> "$CSV_3_2"
            done
        done
    done
done
echo "Exercise 3.2 sequential runs completed. Results saved to $CSV_3_2"
echo

echo "All sequential experiments completed!"
echo "Results appended to:"
echo "  - $CSV_2_2"
echo "  - $CSV_3_2"
