#!/bin/bash

# Run all plotter notebooks

BASE_DIR=$(pwd)

echo "Running plotter_1_1.ipynb..."
cd "$BASE_DIR/1_1_polynomial_multiplication" && python3 -m jupyter execute plotter_1_1.ipynb

echo "Running plotter_1_2.ipynb..."
cd "$BASE_DIR/1_2_shared_variable_update" && python3 -m jupyter execute plotter_1_2.ipynb

echo "Running plotter_1_3.ipynb..."
cd "$BASE_DIR/1_3_array_analysis" && python3 -m jupyter execute plotter_1_3.ipynb

echo "Running plotter_1_4.ipynb..."
cd "$BASE_DIR/1_4_bank_simulation" && python3 -m jupyter execute plotter_1_4.ipynb

echo "Running plotter_1_5.ipynb..."
cd "$BASE_DIR/1_5_barrier_implementations" && python3 -m jupyter execute plotter_1_5.ipynb

cd "$BASE_DIR"
echo "All plotters completed!"
