#!/bin/bash

echo "Exercise 4.1 - SIMD Polynomial Multiplication"
echo "=============================================="

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

CSV_FILE="results_4.1.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "degree,time_alloc,time_init,time_seq,time_simd,time_verify,time_cleanup,time_total,verification,user" > "$CSV_FILE"
fi

for degree in 5000 20000 80000 100000 200000 500000; do
    echo "Degree: $degree"
    output=$(./bin/simd_poly_mult $degree)
    echo "$output"
    echo "$degree,$output,$RUN_USER" >> $CSV_FILE
done

echo "Results saved to $CSV_FILE"
