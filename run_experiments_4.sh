#!/bin/bash

set -e

echo "Parallel Systems - Homework 4 Experiments"
echo "========================================"
echo

echo "Compiling C sources with gcc and SIMD ..."
CC=gcc
CFLAGS="-Wall -O2 -pthread -mavx2"
LDFLAGS="-lm"

mkdir -p 4_1_polynomial_multiplication/bin

$CC $CFLAGS -o 4_1_polynomial_multiplication/bin/simd_poly_mult \
	4_1_polynomial_multiplication/simd_poly_mult.c $LDFLAGS

echo "Compilation finished. Running experiments..."

chmod +x 4_1_polynomial_multiplication/run_poly_mult.sh

LOG_DIR="logs"
mkdir -p "$LOG_DIR"
TS=$(date +%Y%m%d-%H%M%S)

if [ -z "$RUN_USER" ]; then
	RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi
export RUN_USER

echo "Running Exercise 4.1 - SIMD Polynomial Multiplication ..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_4.1_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 4_1_polynomial_multiplication && ./run_poly_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_4.1_${TS}.txt"
	echo >> "$LOG_DIR/results_4.1_${TS}.txt"
done

echo
echo "All experiments completed!"
echo "Results saved to 4_1_polynomial_multiplication/results_4.1.csv"
echo "Full log available at $LOG_DIR/results_4.1_${TS}.txt"
