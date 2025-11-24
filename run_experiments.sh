#!/bin/bash

set -e

echo "Parallel Systems - Homework 1 Experiments"
echo "========================================"
echo

# Run all experiments
chmod +x 1_1_polynomial_multiplication/run_poly_mult.sh
chmod +x 1_2_shared_variable_update/run_shared_var_up.sh
chmod +x 1_3_array_analysis/run_arr_analysis.sh
chmod +x 1_4_bank_simulation/run_bank_sim.sh
chmod +x 1_5_barrier_implementations/run_barrier_impl.sh

echo "Compiling C sources with gcc ..."
CC=gcc
CFLAGS="-Wall -O2 -pthread"
LDFLAGS="-pthread -lm"

# Compile each C source into an executable placed next to the source
$CC $CFLAGS -o 1_1_polynomial_multiplication/seq_poly_mult \
	1_1_polynomial_multiplication/sequential_polynomial_multiplication.c $LDFLAGS
$CC $CFLAGS -o 1_1_polynomial_multiplication/poly_mult \
	1_1_polynomial_multiplication/thread_polynomial_multiplication.c $LDFLAGS

$CC $CFLAGS -o 1_2_shared_variable_update/seq_shared_var \
	1_2_shared_variable_update/seq_shared_var_up.c $LDFLAGS
$CC $CFLAGS -o 1_2_shared_variable_update/shared_var \
	1_2_shared_variable_update/thread_shared_var_up.c $LDFLAGS

$CC $CFLAGS -o 1_3_array_analysis/seq_arr_analysis \
	1_3_array_analysis/seq_arr_analysis.c $LDFLAGS
$CC $CFLAGS -o 1_3_array_analysis/array_analysis \
	1_3_array_analysis/thread_arr_analysis.c $LDFLAGS

$CC $CFLAGS -o 1_4_bank_simulation/seq_bank_sim \
	1_4_bank_simulation/seq_bank_sim.c $LDFLAGS
$CC $CFLAGS -o 1_4_bank_simulation/bank_sim \
	1_4_bank_simulation/thread_bank_sim.c $LDFLAGS

$CC $CFLAGS -o 1_5_barrier_implementations/barrier_cond \
	1_5_barrier_implementations/cond_barrier_impl.c $LDFLAGS
$CC $CFLAGS -o 1_5_barrier_implementations/barrier_pthread \
	1_5_barrier_implementations/pthread_barrier_impl.c $LDFLAGS
$CC $CFLAGS -o 1_5_barrier_implementations/barrier_sense \
	1_5_barrier_implementations/sense_reversal_barrier_impl.c $LDFLAGS

echo "Compilation finished. Running experiments..."

echo "Running Exercise 1.1 - Polynomial Multiplication Experiments..."
(cd 1_1_polynomial_multiplication && ./run_poly_mult.sh) > results_1.1.txt

echo "Running Exercise 1.2 - Shared Variable Update Experiments..."
(cd 1_2_shared_variable_update && ./run_shared_var_up.sh) > results_1.2.txt

echo "Running Exercise 1.3 - Array Analysis Experiments..."
(cd 1_3_array_analysis && ./run_arr_analysis.sh) > results_1.3.txt

echo "Running Exercise 1.4 - Bank Simulation Experiments..."
(cd 1_4_bank_simulation && ./run_bank_sim.sh) > results_1.4.txt

echo "Running Exercise 1.5 - Different Barrier Implementations Experiments..."
(cd 1_5_barrier_implementations && ./run_barrier_impl.sh) > results_1.5.txt

echo "All experiments completed. Results saved to results_*.txt files"