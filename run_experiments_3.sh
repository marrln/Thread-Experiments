#!/bin/bash

set -e

echo "Parallel Systems - Homework 3 Experiments"
echo "========================================"
echo


echo "Compiling C sources with mpicc ..."
CC=mpicc
CFLAGS="-Wall -O2"
LDFLAGS="-lm"

# Ensure each experiment has a bin/ directory
mkdir -p 3_1_polynomial_multiplication/bin
mkdir -p 3_2_sparse_array_vector_multiplication/bin
mkdir -p 3_3_is_2_2_and_3_2_hybrid/bin

# Compile each C source into an executable in the experiment's bin/ folder
$CC $CFLAGS -o 3_1_polynomial_multiplication/bin/seq_poly_mult \
	3_1_polynomial_multiplication/sequential_polynomial_multiplication.c $LDFLAGS

$CC $CFLAGS -o 3_1_polynomial_multiplication/bin/thread_poly_mult \
	3_1_polynomial_multiplication/thread_polynomial_multiplication.c $LDFLAGS

$CC $CFLAGS -o 3_2_sparse_array_vector_multiplication/bin/sparse_arr_vector_mult \
	3_2_sparse_array_vector_multiplication/sparse_arr_vector_mult.c $LDFLAGS

$CC $CFLAGS -o 3_3_is_2_2_and_3_2_hybrid/bin/sparse_arr_vector_mult \
	3_3_is_2_2_and_3_2_hybrid/sparse_arr_vector_mult.c $LDFLAGS

echo "Compilation finished. Running experiments..."


# Run all experiments
chmod +x 3_1_polynomial_multiplication/run_poly_mult.sh
chmod +x 3_2_sparse_array_vector_multiplication/run_sparse_arr_vector_mult.sh
chmod +x 3_3_is_2_2_and_3_2_hybrid/run_sparse_arr_vector_mult.sh

LOG_DIR="logs"
mkdir -p "$LOG_DIR"
TS=$(date +%Y%m%d-%H%M%S)

echo "Running Exercise 3.1 - Polynomial Multiplication (MPI) ..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_3.1_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 3_1_polynomial_multiplication && ./run_poly_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_3.1_${TS}.txt"
	echo >> "$LOG_DIR/results_3.1_${TS}.txt"
done

echo "Running Exercise 3.2 - Sparse Array Vector Multiplication (MPI) ..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_3.2_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 3_2_sparse_array_vector_multiplication && ./run_sparse_arr_vector_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_3.2_${TS}.txt"
	echo >> "$LOG_DIR/results_3.2_${TS}.txt"
done

echo "Running Exercise 3.3 - Hybrid (MPI) ..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_3.3_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 3_3_is_2_2_and_3_2_hybrid && ./run_sparse_arr_vector_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_3.3_${TS}.txt"
	echo >> "$LOG_DIR/results_3.3_${TS}.txt"
done

echo "All experiments completed. Logs saved to $LOG_DIR/ (timestamp: $TS)"