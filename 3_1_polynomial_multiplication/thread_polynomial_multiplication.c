#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (argc < 2 || argc > 3) {
		if (rank == 0)
			printf("Usage: %s <degree> <num_threads>\n", argv[0]);
		MPI_Finalize();
		return 1;
	}
	int n = atoi(argv[1]);
	int poly_size = n + 1;
	int result_size = 2 * n + 1;

	int *A = NULL, *B = NULL, *C = NULL;
	double t_send = 0, t_compute = 0, t_recv = 0, t_total = 0;
	struct timeval t0, t1, t2, t3, t4;

	if (rank == 0) {
		// Allocate and initialize polynomials
		A = malloc(poly_size * sizeof(int));
		B = malloc(poly_size * sizeof(int));
		C = calloc(result_size, sizeof(int));
		unsigned int seed = 42;
		for (int i = 0; i < poly_size; i++) {
			A[i] = (rand_r(&seed) % 9) + 1;
			B[i] = (rand_r(&seed) % 9) + 1;
		}
	}

	// Start total timer (after allocation/initialization)
	if (rank == 0) gettimeofday(&t0, NULL);

	// Broadcast n to all processes
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// Send A and B to all processes
	if (rank == 0) gettimeofday(&t1, NULL);
	if (rank != 0) {
		A = malloc(poly_size * sizeof(int));
		B = malloc(poly_size * sizeof(int));
	}
	MPI_Bcast(A, poly_size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, poly_size, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		gettimeofday(&t2, NULL);
		t_send = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
	}

	// Each process computes its chunk
	int chunk = (result_size + size - 1) / size;
	int start = rank * chunk;
	int end = start + chunk;
	if (end > result_size) end = result_size;
	int local_len = end - start;
	int *C_local = calloc(local_len, sizeof(int));

	if (rank == 0) gettimeofday(&t3, NULL);
	// Parallel computation
	for (int k = start; k < end; k++) {
		int i_start = (k < n) ? 0 : k - n;
		int i_end   = (k < n) ? k : n;
		int sum = 0;
		for (int i = i_start; i <= i_end; i++) {
			sum += A[i] * B[k - i];
		}
		C_local[k - start] = sum;
	}
	if (rank == 0) {
		gettimeofday(&t4, NULL);
		t_compute = (t4.tv_sec - t3.tv_sec) + (t4.tv_usec - t3.tv_usec) / 1000000.0;
	}

	// Gather results to root
	int *recvcounts = NULL, *displs = NULL;
	if (rank == 0) {
		recvcounts = malloc(size * sizeof(int));
		displs = malloc(size * sizeof(int));
		for (int i = 0; i < size; i++) {
			int s = i * chunk;
			int e = s + chunk;
			if (e > result_size) e = result_size;
			recvcounts[i] = e - s;
			displs[i] = s;
		}
		gettimeofday(&t3, NULL); // reuse t3 for receive timing
	}
	MPI_Gatherv(C_local, local_len, MPI_INT,
				C, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		gettimeofday(&t4, NULL);
		t_recv = (t4.tv_sec - t3.tv_sec) + (t4.tv_usec - t3.tv_usec) / 1000000.0;
		gettimeofday(&t4, NULL);
		t_total = (t4.tv_sec - t0.tv_sec) + (t4.tv_usec - t0.tv_usec) / 1000000.0;
	}

	// Print timings
	if (rank == 0) {
		/* Compute verification by comparing gathered C with sequential result */
		int verification_passed = 1;
		int *C_expected = calloc(result_size, sizeof(int));
		for (int k = 0; k < result_size; k++) {
			int i_start = (k < n) ? 0 : k - n;
			int i_end = (k < n) ? k : n;
			int sum = 0;
			for (int i = i_start; i <= i_end; i++) {
				sum += A[i] * B[k - i];
			}
			C_expected[k] = sum;
			if (C[k] != C_expected[k]) verification_passed = 0;
		}
		free(C_expected);

		/* Map measured times */
		double time_alloc = 0.0;
		double time_init = 0.0;
		double time_create = t_send; 
		double time_compute = t_compute;
		double time_join = t_recv; /* use this slot for gather/recv time */
		double time_reduce = 0.0;
		double time_verify = 0.0; /* verification cost not timed separately */
		double time_cleanup = 0.0;
		double time_total = t_total;

		printf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%s\n",
			time_alloc, time_init, time_create, time_compute, time_join, time_reduce, time_verify, time_cleanup, time_total,
			verification_passed ? "PASS" : "FAIL");
	}

	// Cleanup
	free(C_local);
	if (rank == 0) {
		free(A); free(B); free(C);
		free(recvcounts); free(displs);
	} else {
		free(A); free(B);
	}
	MPI_Finalize();
	return 0;
}
