#!/bin/bash

echo "Parallel Systems - Homework 1 Experiments"
echo "========================================"
echo

# Run all experiments
chmod +x 1_1_polynomial_multiplication/run_poly_mult.sh
chmod +x 1_2_shared_variable_update/run_shared_var_up.sh
chmod +x 1_3_array_analysis/run_arr_analysis.sh
chmod +x 1_4_bank_simulation/run_bank_sim.sh
chmod +x 1_5_barrier_implementations/run_barrier_impl.sh

echo "Running Exercise 1.1 - Polynomial Multiplication Experiments..."
./1_1_polynomial_multiplication/run_poly_mult.sh > results_1.1.txt

echo "Running Exercise 1.2 - Shared Variable Update Experiments..."
./1_2_shared_variable_update/run_shared_var_up.sh > results_1.2.txt

echo "Running Exercise 1.3 - Array Analysis Experiments..."
./1_3_array_analysis/run_arr_analysis.sh > results_1.3.txt

echo "Running Exercise 1.4 - Bank Simulation Experiments..."
./1_4_bank_simulation/run_bank_sim.sh > results_1.4.txt

echo "Running Exercise 1.5 - Different Barrier Implementations Experiments..."
./1_5_barrier_implementations/run_barrier_impl.sh > results_1.5.txt

echo "All experiments completed. Results saved to results_*.txt files"