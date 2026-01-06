#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

void multiply_polynomials_parallel(int *A, int *B, int *C, int n, int num_threads) {
	int size = 2 * n + 1;
	#pragma omp parallel for num_threads(num_threads) schedule(static)
	for (int k = 0; k < size; k++) {
		int i_start = (k < n) ? 0 : k - n;
		int i_end   = (k < n) ? k : n;
		int sum = 0;
		for (int i = i_start; i <= i_end; i++) {
			sum += A[i] * B[k - i];
		}
		C[k] = sum;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: %s <degree> <num_threads>\n", argv[0]);
		return 1;
	}
	int n = atoi(argv[1]);
	int num_threads = atoi(argv[2]);
	int size = 2 * n + 1;

	struct timeval t_start, t_alloc, t_init, t_parallel, t_verify_start, t_verify_end, t_cleanup, t_end;

	gettimeofday(&t_start, NULL);

	int *A = malloc((n + 1) * sizeof(int));
	int *B = malloc((n + 1) * sizeof(int));
	int *C_par = calloc(size, sizeof(int));

	gettimeofday(&t_alloc, NULL);

	// Use fixed seed for reproducible results
	unsigned int seed = 42;
	for (int i = 0; i <= n; i++) {
		A[i] = (rand_r(&seed) % 9) + 1;
		B[i] = (rand_r(&seed) % 9) + 1;
	}

	gettimeofday(&t_init, NULL);

	// Parallel multiplication
	multiply_polynomials_parallel(A, B, C_par, n, num_threads);

	gettimeofday(&t_parallel, NULL);

	// Verify correctness by computing sequential version
	gettimeofday(&t_verify_start, NULL);
	int *C_expected = calloc(size, sizeof(int));
	for (int k = 0; k < size; k++) {
		int i_start = (k < n) ? 0 : k - n;
		int i_end   = (k < n) ? k : n;
		int sum = 0;
		for (int i = i_start; i <= i_end; i++) {
			sum += A[i] * B[k - i];
		}
		C_expected[k] = sum;
	}
	int verification_passed = 1;
	for (int k = 0; k < size; k++) {
		if (C_par[k] != C_expected[k]) {
			verification_passed = 0;
			break;
		}
	}
	free(C_expected);
	gettimeofday(&t_verify_end, NULL);

	// Cleanup
	gettimeofday(&t_cleanup, NULL);
	free(A); free(B); free(C_par);
	gettimeofday(&t_end, NULL);

	double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
	double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
	double time_parallel = (t_parallel.tv_sec - t_init.tv_sec) + (t_parallel.tv_usec - t_init.tv_usec) / 1000000.0;
	double time_verify = (t_verify_end.tv_sec - t_verify_start.tv_sec) + (t_verify_end.tv_usec - t_verify_start.tv_usec) / 1000000.0;
	double time_cleanup = (t_end.tv_sec - t_cleanup.tv_sec) + (t_end.tv_usec - t_cleanup.tv_usec) / 1000000.0;
	double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;

	// Print times in CSV
	// time_alloc,time_init,time_thread_create,time_compute,time_join,time_reduce,time_verify,time_cleanup,time_total,verification
	printf("%f,%f,0.0,%f,0.0,0.0,%f,%f,%f,%s\n",
		time_alloc, time_init, time_parallel, time_verify, time_cleanup, time_total,
		verification_passed ? "PASS" : "FAIL");

	return 0;
}
