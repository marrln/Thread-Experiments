CC = gcc
CFLAGS = -Wall -O2 -pthread
LDFLAGS = -pthread -lm

# Per-experiment bin directories
BIN1 = 1_1_polynomial_multiplication/bin
BIN2 = 1_2_shared_variable_update/bin
BIN3 = 1_3_array_analysis/bin
BIN4 = 1_4_bank_simulation/bin
BIN5 = 1_5_barrier_implementations/bin

all: $(BIN1) $(BIN2) $(BIN3) $(BIN4) $(BIN5) \
    $(BIN1)/seq_poly_mult $(BIN1)/poly_mult \
    $(BIN2)/seq_shared_var $(BIN2)/shared_var \
    $(BIN3)/seq_arr_analysis $(BIN3)/thread_arr_analysis_unpadded $(BIN3)/thread_arr_analysis_padded $(BIN3)/thread_arr_analysis_local_accum \
    $(BIN4)/seq_bank_sim $(BIN4)/bank_sim \
    $(BIN5)/barrier_pthread $(BIN5)/barrier_cond $(BIN5)/barrier_sense

$(BIN1) $(BIN2) $(BIN3) $(BIN4) $(BIN5):
	mkdir -p $@

# 1.1 Polynomial multiplication
$(BIN1)/seq_poly_mult: 1_1_polynomial_multiplication/sequential_polynomial_multiplication.c | $(BIN1)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN1)/poly_mult: 1_1_polynomial_multiplication/thread_polynomial_multiplication.c | $(BIN1)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 1.2 Shared variable update
$(BIN2)/seq_shared_var: 1_2_shared_variable_update/seq_shared_var_up.c | $(BIN2)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN2)/shared_var: 1_2_shared_variable_update/thread_shared_var_up.c | $(BIN2)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 1.3 Array analysis (multiple thread variants)
$(BIN3)/seq_arr_analysis: 1_3_array_analysis/seq_arr_analysis.c | $(BIN3)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN3)/thread_arr_analysis_unpadded: 1_3_array_analysis/thread_arr_analysis_unpadded.c | $(BIN3)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN3)/thread_arr_analysis_padded: 1_3_array_analysis/thread_arr_analysis_padded.c | $(BIN3)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN3)/thread_arr_analysis_local_accum: 1_3_array_analysis/thread_arr_analysis_local_accum.c | $(BIN3)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 1.4 Bank simulation
$(BIN4)/seq_bank_sim: 1_4_bank_simulation/seq_bank_sim.c | $(BIN4)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN4)/bank_sim: 1_4_bank_simulation/thread_bank_sim.c | $(BIN4)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 1.5 Barrier implementations
$(BIN5)/barrier_pthread: 1_5_barrier_implementations/pthread_barrier_impl.c | $(BIN5)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN5)/barrier_cond: 1_5_barrier_implementations/cond_barrier_impl.c | $(BIN5)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN5)/barrier_sense: 1_5_barrier_implementations/sense_reversal_barrier_impl.c | $(BIN5)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(BIN1)/* $(BIN2)/* $(BIN3)/* $(BIN4)/* $(BIN5)/*

.PHONY: all clean