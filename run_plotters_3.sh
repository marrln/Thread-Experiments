#!/bin/bash

# Run all plotter notebooks

BASE_DIR=$(pwd)

echo "Running plotter_3_1.ipynb..."
cd "$BASE_DIR/3_1_polynomial_multiplication" && python3 -m jupyter execute plotter_3_1.ipynb

echo "Running plotter_3_2.ipynb..."
cd "$BASE_DIR/3_2_sparse_array_vector_multiplication" && python3 -m jupyter execute plotter_3_2.ipynb

cd "$BASE_DIR"
echo "All plotters completed!"
