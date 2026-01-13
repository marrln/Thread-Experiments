#!/bin/bash

# Run all plotter notebooks

BASE_DIR=$(pwd)

echo "Running plotter_2_1.ipynb..."
cd "$BASE_DIR/2_1_polynomial_multiplication" && python3 -m jupyter execute plotter_2_1.ipynb

echo "Running plotter_2_2.ipynb..."
cd "$BASE_DIR/2_2_sparse_array_vector_multiplication" && python3 -m jupyter execute plotter_2_2.ipynb

echo "Running plotter_2_3.ipynb..."
cd "$BASE_DIR/2_3_mergesort" && python3 -m jupyter execute plotter_2_3.ipynb

cd "$BASE_DIR"
echo "All plotters completed!"
