# Makefile for Thread Experiments
CFLAGS = -Wall -O2 -pthread
CFLAGS_OPENMP = $(CFLAGS) -fopenmp
CFLAGS_MPI    = $(CFLAGS)
CFLAGS_SIMD   = $(CFLAGS) -mavx2
LDFLAGS = -lm

# 1_{i} experiment binaries
BIN_1_1 = 1_1_polynomial_multiplication/bin
BIN_1_2 = 1_2_shared_variable_update/bin
BIN_1_3 = 1_3_array_analysis/bin
BIN_1_4 = 1_4_bank_simulation/bin
BIN_1_5 = 1_5_barrier_implementations/bin

# 2_{i} experiment binaries
BIN_2_1 = 2_1_polynomial_multiplication/bin
BIN_2_2 = 2_2_sparse_array_vector_multiplication/bin
BIN_2_3 = 2_3_mergesort/bin

# 3_{i} experiment binaries
BIN_3_1 = 3_1_polynomial_multiplication/bin
BIN_3_2 = 3_2_sparse_array_vector_multiplication/bin

# 4_{i} experiment binaries
BIN_4_1 = 4_1_polynomial_multiplication/bin

# Top-level all target
all:	$(BIN_1_1) $(BIN_1_2) $(BIN_1_3) $(BIN_1_4) $(BIN_1_5) \
		$(BIN_2_1)  $(BIN_2_2) $(BIN_2_3) \
		$(BIN_3_1) $(BIN_3_2) \
		$(BIN_4_1) \
		$(BIN_1_1)/seq_poly_mult $(BIN_1_1)/poly_mult \
		$(BIN_1_2)/shared_var \
		$(BIN_1_3)/seq_arr_analysis $(BIN_1_3)/thread_arr_analysis_unpadded $(BIN_1_3)/thread_arr_analysis_padded $(BIN_1_3)/thread_arr_analysis_local_accum \
		$(BIN_1_4)/seq_bank_sim $(BIN_1_4)/bank_sim \
		$(BIN_1_5)/barrier_pthread $(BIN_1_5)/barrier_cond $(BIN_1_5)/barrier_sense $(BIN_2_2)/sparse_arr_vector_mult $(BIN_2_2)/seq_sparse_arr_vector_mult $(BIN_3_2)/sparse_arr_vector_mult $(BIN_3_2)/seq_sparse_arr_vector_mult \
		$(BIN_2_1)/seq_poly_mult $(BIN_2_1)/poly_mult $(BIN_2_3)/mergesort \
		$(BIN_3_1)/seq_poly_mult $(BIN_3_1)/thread_poly_mult \
		$(BIN_4_1)/simd_poly_mult 

# Create binary directories
$(BIN_1_1) $(BIN_1_2) $(BIN_1_3) $(BIN_1_4) $(BIN_1_5) \
$(BIN_2_1) $(BIN_2_2) $(BIN_2_3) \
$(BIN_3_1) $(BIN_3_2) \
$(BIN_4_1):
	mkdir -p $@

# 1.1 Polynomial multiplication
$(BIN_1_1)/seq_poly_mult: 1_1_polynomial_multiplication/sequential_polynomial_multiplication.c | $(BIN_1_1)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) 
$(BIN_1_1)/poly_mult: 1_1_polynomial_multiplication/thread_polynomial_multiplication.c | $(BIN_1_1)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) 

# 1.2 Shared variable update
$(BIN_1_2)/shared_var: 1_2_shared_variable_update/thread_shared_var_up.c | $(BIN_1_2)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  

# 1.3 Array analysis (multiple thread variants)
$(BIN_1_3)/seq_arr_analysis: 1_3_array_analysis/seq_arr_analysis.c | $(BIN_1_3)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) 
$(BIN_1_3)/thread_arr_analysis_unpadded: 1_3_array_analysis/thread_arr_analysis_unpadded.c | $(BIN_1_3)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  
$(BIN_1_3)/thread_arr_analysis_padded: 1_3_array_analysis/thread_arr_analysis_padded.c | $(BIN_1_3)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  
$(BIN_1_3)/thread_arr_analysis_local_accum: 1_3_array_analysis/thread_arr_analysis_local_accum.c | $(BIN_1_3)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  

# 1.4 Bank simulation
$(BIN_1_4)/seq_bank_sim: 1_4_bank_simulation/seq_bank_sim.c | $(BIN_1_4)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  
$(BIN_1_4)/bank_sim: 1_4_bank_simulation/thread_bank_sim.c | $(BIN_1_4)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  

# 1.5 Barrier implementations
$(BIN_1_5)/barrier_pthread: 1_5_barrier_implementations/pthread_barrier_impl.c | $(BIN_1_5)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)  
$(BIN_1_5)/barrier_cond: 1_5_barrier_implementations/cond_barrier_impl.c | $(BIN_1_5)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) 
$(BIN_1_5)/barrier_sense: 1_5_barrier_implementations/sense_reversal_barrier_impl.c | $(BIN_1_5)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) 

# 2.1 Polynomial multiplication (OpenMP)
$(BIN_2_1)/seq_poly_mult: 2_1_polynomial_multiplication/sequential_polynomial_multiplication.c | $(BIN_2_1)
	gcc $(CFLAGS_OPENMP) -o $@ $< $(LDFLAGS)
	
# 2.2 Sparse array-vector multiplication (OpenMP)
$(BIN_2_2)/sparse_arr_vector_mult: 2_2_sparse_array_vector_multiplication/sparse_arr_vector_mult.c | $(BIN_2_2)
	gcc $(CFLAGS_OPENMP) -o $@ $< $(LDFLAGS)
$(BIN_2_2)/seq_sparse_arr_vector_mult: 2_2_sparse_array_vector_multiplication/sequential_sparse_arr_vector_mult.c | $(BIN_2_2)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS)
$(BIN_2_1)/poly_mult: 2_1_polynomial_multiplication/thread_polynomial_multiplication.c | $(BIN_2_1)
	gcc $(CFLAGS_OPENMP) -o $@ $< $(LDFLAGS)  

# 2.3 Mergesort (OpenMP)
$(BIN_2_3)/mergesort: 2_3_mergesort/mergesort.c | $(BIN_2_3)
	gcc $(CFLAGS_OPENMP) -o $@ $< $(LDFLAGS)  

# 3.1 Polynomial multiplication (MPI)
$(BIN_3_1)/seq_poly_mult: 3_1_polynomial_multiplication/sequential_polynomial_multiplication.c | $(BIN_3_1)
	mpicc $(CFLAGS_MPI) -o $@ $< $(LDFLAGS) 
$(BIN_3_1)/thread_poly_mult: 3_1_polynomial_multiplication/thread_polynomial_multiplication.c | $(BIN_3_1)
	mpicc $(CFLAGS_MPI) -o $@ $< $(LDFLAGS) 

# 3.2 Sparse array-vector multiplication (MPI)
$(BIN_3_2)/sparse_arr_vector_mult: 3_2_sparse_array_vector_multiplication/sparse_arr_vector_mult.c | $(BIN_3_2)
	mpicc $(CFLAGS_MPI) -o $@ $< $(LDFLAGS)
$(BIN_3_2)/seq_sparse_arr_vector_mult: 3_2_sparse_array_vector_multiplication/sequential_sparse_arr_vector_mult.c | $(BIN_3_2)
	gcc $(CFLAGS) -o $@ $< $(LDFLAGS) 

# 4.1 SIMD Polynomial multiplication
$(BIN_4_1)/simd_poly_mult: 4_1_polynomial_multiplication/simd_poly_mult.c | $(BIN_4_1)
	gcc $(CFLAGS_SIMD) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(BIN_1_1)/* $(BIN_1_2)/* $(BIN_1_3)/* $(BIN_1_4)/* $(BIN_1_5)/* $(BIN_2_1)/* $(BIN_2_2)/* $(BIN_2_3)/* $(BIN_3_1)/*  $(BIN_3_2)/* $(BIN_4_1)/*

.PHONY: all clean