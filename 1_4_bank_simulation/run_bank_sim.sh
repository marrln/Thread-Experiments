#!/bin/bash

echo "Exercise 1.4 - Bank Simulation"
echo "=============================="

transactions_per_thread=10000
num_threads=4

# Create CSV file with header if it does not exist (append otherwise)
CSV_FILE="results_1.4.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "locking_scheme,accounts,query_pct,time_alloc,time_init,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total" > "$CSV_FILE"
fi

echo "Sequential Baseline"
echo "-------------------"
for accounts in 100 1000 10000; do
    for query_pct in 0.1 0.5 0.9; do
        echo -n "Accounts: $accounts, Query%: $query_pct - "
        output=$(./seq_bank_sim $accounts $transactions_per_thread $query_pct $num_threads)
        echo "$output"
        echo "sequential,$accounts,$query_pct,$output" >> $CSV_FILE
    done
done
echo

for scheme in 1 2 3; do
    scheme_name="coarse"
    if [ $scheme -eq 2 ]; then scheme_name="fine"; fi
    if [ $scheme -eq 3 ]; then scheme_name="rwlock"; fi
    
    echo "Parallel - Locking scheme: $scheme (1-coarse, 2-fine, 3-rwlock)"
    echo "----------------------------------------------------------------"
    for accounts in 100 1000 10000; do
        for query_pct in 0.1 0.5 0.9; do
            echo -n "Accounts: $accounts, Query%: $query_pct - "
            output=$(./bank_sim $accounts $transactions_per_thread $query_pct $scheme $num_threads)
            echo "$output"
            echo "$scheme_name,$accounts,$query_pct,$output" >> $CSV_FILE
        done
    done
    echo
done

echo "Results saved to $CSV_FILE"