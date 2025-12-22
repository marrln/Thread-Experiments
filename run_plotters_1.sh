#!/bin/bash

# Run all plotter notebooks

echo "Running plotter_1_1.ipynb..."
cd 1_1_polynomial_multiplication && jupyter nbconvert --to notebook --execute plotter_1_1.ipynb && cd ..

echo "Running plotter_1_2.ipynb..."
cd 1_2_shared_variable_update && jupyter nbconvert --to notebook --execute plotter_1_2.ipynb && cd ..

echo "Running plotter_1_3.ipynb..."
cd 1_3_array_analysis && jupyter nbconvert --to notebook --execute plotter_1_3.ipynb && cd ..

echo "Running plotter_1_4.ipynb..."
cd 1_4_bank_simulation && jupyter nbconvert --to notebook --execute plotter_1_4.ipynb && cd ..

echo "Running plotter_1_5.ipynb..."
cd 1_5_barrier_implementations && jupyter nbconvert --to notebook --execute plotter_1_5.ipynb && cd ..

echo "All plotters completed!"
