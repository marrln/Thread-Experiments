CC = gcc
CFLAGS = -Wall -O2 -pthread
LDFLAGS = -pthread -lm

all: seq_poly_mult poly_mult seq_shared_var shared_var seq_arr_analysis array_analysis seq_bank_sim bank_sim barrier_pthread barrier_cond barrier_sense

seq_poly_mult: 1_1_polynomial_multiplication/sequential_polynomial_multiplication.c
	$(CC) $(CFLAGS) -o seq_poly_mult 1_1_polynomial_multiplication/sequential_polynomial_multiplication.c $(LDFLAGS)

poly_mult: 1_1_polynomial_multiplication/thread_polynomial_multiplication.c
	$(CC) $(CFLAGS) -o poly_mult 1_1_polynomial_multiplication/thread_polynomial_multiplication.c $(LDFLAGS)

seq_shared_var: 1_2_shared_variable_update/seq_shared_var_up.c
	$(CC) $(CFLAGS) -o seq_shared_var 1_2_shared_variable_update/seq_shared_var_up.c $(LDFLAGS)

shared_var: 1_2_shared_variable_update/thread_shared_var_up.c
	$(CC) $(CFLAGS) -o shared_var 1_2_shared_variable_update/thread_shared_var_up.c $(LDFLAGS)

seq_arr_analysis: 1_3_array_analysis/seq_arr_analysis.c
	$(CC) $(CFLAGS) -o seq_arr_analysis 1_3_array_analysis/seq_arr_analysis.c $(LDFLAGS)

array_analysis: 1_3_array_analysis/thread_arr_analysis.c
	$(CC) $(CFLAGS) -o array_analysis 1_3_array_analysis/thread_arr_analysis.c $(LDFLAGS)

seq_bank_sim: 1_4_bank_simulation/seq_bank_sim.c
	$(CC) $(CFLAGS) -o seq_bank_sim 1_4_bank_simulation/seq_bank_sim.c $(LDFLAGS)

bank_sim: 1_4_bank_simulation/thread_bank_sim.c
	$(CC) $(CFLAGS) -o bank_sim 1_4_bank_simulation/thread_bank_sim.c $(LDFLAGS)

barrier_pthread: 1_5_barrier_implementations/pthread_barrier_impl.c
	$(CC) $(CFLAGS) -o barrier_pthread 1_5_barrier_implementations/pthread_barrier_impl.c $(LDFLAGS)

barrier_cond: 1_5_barrier_implementations/cond_barrier_impl.c
	$(CC) $(CFLAGS) -o barrier_cond 1_5_barrier_implementations/cond_barrier_impl.c $(LDFLAGS)

barrier_sense: 1_5_barrier_implementations/sense_reversal_barrier_impl.c
	$(CC) $(CFLAGS) -o barrier_sense 1_5_barrier_implementations/sense_reversal_barrier_impl.c $(LDFLAGS)

clean:
	rm -f seq_poly_mult poly_mult seq_shared_var shared_var seq_arr_analysis array_analysis seq_bank_sim bank_sim barrier_pthread barrier_cond barrier_sense

.PHONY: all clean