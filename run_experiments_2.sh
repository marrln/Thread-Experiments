#!/bin/bash

set -e

echo "Parallel Systems - Homework 2 Experiments"
echo "========================================"
echo

echo "Compiling C sources with gcc ..."
echo "Compilation finished. Running experiments..."

# Run all experiments
chmod +x 2_1_polynomial_multiplication/run_poly_mult.sh
chmod +x 2_2_sparse_array_vector_multiplication/run_sparse_arr_vector_mult.sh
chmod +x 2_3_mergesort/run_mergesort.sh

echo "Compiling C sources with gcc and OpenMP ..."
CC=gcc
CFLAGS="-Wall -O2 -fopenmp"
LDFLAGS="-fopenmp -lm"

# Ensure each experiment has a `bin/` directory to hold compiled executables
mkdir -p 2_1_polynomial_multiplication/bin
mkdir -p 2_2_sparse_array_vector_multiplication/bin
mkdir -p 2_3_mergesort/bin

# Compile each C source into an executable placed in the experiment's `bin/` folder
$CC $CFLAGS -o 2_1_polynomial_multiplication/bin/seq_poly_mult \
	2_1_polynomial_multiplication/sequential_polynomial_multiplication.c $LDFLAGS
$CC $CFLAGS -o 2_1_polynomial_multiplication/bin/poly_mult \
	2_1_polynomial_multiplication/thread_polynomial_multiplication.c $LDFLAGS

$CC $CFLAGS -o 2_2_sparse_array_vector_multiplication/bin/sparse_arr_vector_mult \
	2_2_sparse_array_vector_multiplication/sparse_arr_vector_mult.c $LDFLAGS

$CC $CFLAGS -o 2_3_mergesort/bin/mergesort \
	2_3_mergesort/mergesort.c $LDFLAGS

echo "Compilation finished. Running experiments..."

# Create logs dir and timestamped filenames so each run is preserved
LOG_DIR="logs"
mkdir -p "$LOG_DIR"
TS=$(date +%Y%m%d-%H%M%S)

# Determine RUN_USER and write per-user metadata JSON
if [ -z "$RUN_USER" ]; then
		RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi
export RUN_USER

METADATA_DIR="metadata"
mkdir -p "$METADATA_DIR"
METADATA_FILE="$METADATA_DIR/${RUN_USER}.json"

# Collect hardware/build info (best-effort, Linux-friendly)
HOST=$(hostname 2>/dev/null || echo unknown)
RUN_TS=$(date -u +%Y-%m-%dT%H:%M:%SZ)
CPU_MODEL=$(grep -m1 'model name' /proc/cpuinfo 2>/dev/null | sed -E 's/.*: //' || echo unknown)
CPU_MODEL=${CPU_MODEL//,/; }
NCPUS=$(nproc 2>/dev/null || echo 1)
MEM_KB=$(awk '/MemTotal/ {print $2}' /proc/meminfo 2>/dev/null || echo 0)
OS_INFO=$(lsb_release -ds 2>/dev/null || uname -srv)
GIT_COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo unknown)

cat > "$METADATA_FILE" <<EOF
{
	"user": "${RUN_USER}",
	"collected_at": "${RUN_TS}",
	"host": "${HOST}",
	"os": "${OS_INFO}",
	"cpu_model": "${CPU_MODEL}",
	"n_logical_cpus": ${NCPUS},
	"mem_kb": ${MEM_KB},
	"compiler": "${CC}",
	"compiler_flags": "${CFLAGS}",
	"git_commit": "${GIT_COMMIT}"
}
EOF

echo "Wrote metadata to $METADATA_FILE"

echo "Running Exercise 2.1 - Polynomial Multiplication Experiments..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_2.1_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 2_1_polynomial_multiplication && ./run_poly_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_2.1_${TS}.txt"
	echo >> "$LOG_DIR/results_2.1_${TS}.txt"
done

echo "Running Exercise 2.2 - Sparse Array Vector Multiplication Experiments..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_2.2_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 2_2_sparse_array_vector_multiplication && ./run_sparse_arr_vector_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_2.2_${TS}.txt"
	echo >> "$LOG_DIR/results_2.2_${TS}.txt"
done

echo "Running Exercise 2.3 - Mergesort Experiments..."
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_2.3_${TS}_run${i}.txt"
	echo "Run $i/5 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 2_3_mergesort && ./run_mergesort.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" | tee -a "$RUN_LOG"
	else
		echo "Run $i FAILED" | tee -a "$RUN_LOG"
	fi
	cat "$RUN_LOG" >> "$LOG_DIR/results_2.3_${TS}.txt"
	echo >> "$LOG_DIR/results_2.3_${TS}.txt"
done

echo "All experiments completed. Logs saved to $LOG_DIR/ (timestamp: $TS)"