#!/bin/bash

echo "Exercise 1.4 - Bank Simulation"
echo "=============================="

if [ -z "$RUN_USER" ]; then
    RUN_USER=$(whoami 2>/dev/null || echo "${USER:-unknown}")
fi

# Create CSV file with header if it does not exist (append otherwise)
CSV_FILE="results_1.4.csv"
if [ ! -f "$CSV_FILE" ]; then
    echo "locking_scheme,accounts,query_pct,num_threads,transactions_per_thread,time_alloc,time_init,time_thread_create,time_compute,time_thread_join,time_cleanup,time_total,user" > "$CSV_FILE"
fi

echo "Sequential Baseline"
echo "-------------------"
for transactions_per_thread in 5000 10000 20000; do
    for accounts in 100 1000 10000; do
        for query_pct in 0.1 0.5 0.9; do
            echo -n "Trans: $transactions_per_thread, Accounts: $accounts, Query%: $query_pct - "
            output=$(./bin/seq_bank_sim $accounts $transactions_per_thread $query_pct 1)
            echo "$output"
            echo "sequential,$accounts,$query_pct,1,$transactions_per_thread,$output,$RUN_USER" >> $CSV_FILE
        done
    done
done
echo

for scheme in 1 2 3; do
    scheme_name="coarse"
    if [ $scheme -eq 2 ]; then scheme_name="fine"; fi
    if [ $scheme -eq 3 ]; then scheme_name="rwlock"; fi
    
    echo "Parallel - Locking scheme: $scheme_name (1-coarse, 2-fine, 3-rwlock)"
    echo "----------------------------------------------------------------"
    for num_threads in 4 8; do
        for transactions_per_thread in 5000 10000 20000; do
            for accounts in 100 1000 10000; do
                for query_pct in 0.1 0.5 0.9; do
                    echo -n "T=$num_threads, Trans=$transactions_per_thread, Acct=$accounts, Q%=$query_pct - "
                    output=$(./bin/bank_sim $accounts $transactions_per_thread $query_pct $scheme $num_threads)
                    echo "$output"
                    echo "$scheme_name,$accounts,$query_pct,$num_threads,$transactions_per_thread,$output,$RUN_USER" >> $CSV_FILE
                done
            done
        done
    done
    echo
done

echo "Results saved to $CSV_FILE"