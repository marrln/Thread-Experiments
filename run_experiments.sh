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

# Ensure each experiment has a `bin/` directory to hold compiled executables
mkdir -p 1_1_polynomial_multiplication/bin
mkdir -p 1_2_shared_variable_update/bin
mkdir -p 1_3_array_analysis/bin
mkdir -p 1_4_bank_simulation/bin
mkdir -p 1_5_barrier_implementations/bin

# Compile each C source into an executable placed in the experiment's `bin/` folder
$CC $CFLAGS -o 1_1_polynomial_multiplication/bin/seq_poly_mult \
	1_1_polynomial_multiplication/sequential_polynomial_multiplication.c $LDFLAGS
$CC $CFLAGS -o 1_1_polynomial_multiplication/bin/poly_mult \
	1_1_polynomial_multiplication/thread_polynomial_multiplication.c $LDFLAGS

$CC $CFLAGS -o 1_2_shared_variable_update/bin/seq_shared_var \
	1_2_shared_variable_update/seq_shared_var_up.c $LDFLAGS
$CC $CFLAGS -o 1_2_shared_variable_update/bin/shared_var \
	1_2_shared_variable_update/thread_shared_var_up.c $LDFLAGS

$CC $CFLAGS -o 1_3_array_analysis/bin/seq_arr_analysis \
	1_3_array_analysis/seq_arr_analysis.c $LDFLAGS
$CC $CFLAGS -o 1_3_array_analysis/bin/array_analysis \
	1_3_array_analysis/thread_arr_analysis.c $LDFLAGS

$CC $CFLAGS -o 1_4_bank_simulation/bin/seq_bank_sim \
	1_4_bank_simulation/seq_bank_sim.c $LDFLAGS
$CC $CFLAGS -o 1_4_bank_simulation/bin/bank_sim \
	1_4_bank_simulation/thread_bank_sim.c $LDFLAGS

$CC $CFLAGS -o 1_5_barrier_implementations/bin/barrier_cond \
	1_5_barrier_implementations/cond_barrier_impl.c $LDFLAGS
$CC $CFLAGS -o 1_5_barrier_implementations/bin/barrier_pthread \
	1_5_barrier_implementations/pthread_barrier_impl.c $LDFLAGS
$CC $CFLAGS -o 1_5_barrier_implementations/bin/barrier_sense \
	1_5_barrier_implementations/sense_reversal_barrier_impl.c $LDFLAGS

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

echo "Running Exercise 1.1 - Polynomial Multiplication Experiments..."
# Run exercise 1.1 ten times, saving per-run and aggregate logs
for i in $(seq 1 5); do
	RUN_LOG="$LOG_DIR/results_1.1_${TS}_run${i}.txt"
	echo "Run $i/10 - $(date -u +%Y-%m-%dT%H:%M:%SZ)" > "$RUN_LOG"
	if (cd 1_1_polynomial_multiplication && ./run_poly_mult.sh) >> "$RUN_LOG" 2>&1; then
		echo "Run $i completed successfully" >> "$RUN_LOG"
	else
		echo "Run $i FAILED" >> "$RUN_LOG"
	fi
	echo "===== End Run $i =====" >> "$RUN_LOG"
	# append this run to aggregate file
	cat "$RUN_LOG" >> "$LOG_DIR/results_1.1_${TS}.txt"
	echo >> "$LOG_DIR/results_1.1_${TS}.txt"
done

#echo "Running Exercise 1.2 - Shared Variable Update Experiments..."
#(cd 1_2_shared_variable_update && ./run_shared_var_up.sh) > "$LOG_DIR/results_1.2_${TS}.txt" 2>&1

#echo "Running Exercise 1.3 - Array Analysis Experiments..."
#(cd 1_3_array_analysis && ./run_arr_analysis.sh) > "$LOG_DIR/results_1.3_${TS}.txt" 2>&1

#echo "Running Exercise 1.4 - Bank Simulation Experiments..."
#(cd 1_4_bank_simulation && ./run_bank_sim.sh) > "$LOG_DIR/results_1.4_${TS}.txt" 2>&1

#echo "Running Exercise 1.5 - Different Barrier Implementations Experiments..."
#(cd 1_5_barrier_implementations && ./run_barrier_impl.sh) > "$LOG_DIR/results_1.5_${TS}.txt" 2>&1

echo "All experiments completed. Logs saved to $LOG_DIR/ (timestamp: $TS)"