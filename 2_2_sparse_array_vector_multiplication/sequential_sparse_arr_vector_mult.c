#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static double now_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec/1e6;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n> <sparsity_percent> <reps>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int sparsity = atoi(argv[2]);
    int reps = atoi(argv[3]);

    if (n <= 0 || sparsity < 0 || sparsity > 100 || reps <= 0) {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }

    const unsigned int base_seed = 42u;

    long long matrix_size = (long long)n * (long long)n;
    int *dense = malloc(sizeof(int) * matrix_size);
    if (!dense) {
        fprintf(stderr, "Failed to allocate dense matrix (%lld elements)\n", matrix_size);
        return 1;
    }

    int *x = malloc(sizeof(int) * n);
    int *y = malloc(sizeof(int) * n);
    if (!x || !y) {
        fprintf(stderr, "Failed to allocate vectors\n");
        free(dense); return 1;
    }

    double t_init_start = now_sec();

    for (int i = 0; i < n; ++i) {
        unsigned int seed = base_seed + (unsigned int)i;
        for (int j = 0; j < n; ++j) {
            int is_nonzero = ((rand_r(&seed) % 100) >= sparsity) ? 1 : 0;
            if (is_nonzero) {
                dense[(long long)i * n + j] = (rand_r(&seed) % 9) + 1;
            } else {
                dense[(long long)i * n + j] = 0;
            }
        }
    }

    unsigned int seed_x = base_seed + 12345u;
    for (int i = 0; i < n; ++i) x[i] = (rand_r(&seed_x) % 9) + 1;

    double t_init_end = now_sec();

    int *row_nnz = malloc(sizeof(int) * n);
    if (!row_nnz) { free(dense); free(x); free(y); return 1; }

    double t_csr_start = now_sec();

    for (int i = 0; i < n; ++i) {
        int count = 0;
        for (int j = 0; j < n; ++j) if (dense[(long long)i * n + j] != 0) count++;
        row_nnz[i] = count;
    }

    long long nnz = 0;
    int *row_ptr = malloc(sizeof(int) * (n + 1));
    if (!row_ptr) { free(dense); free(x); free(y); free(row_nnz); return 1; }
    row_ptr[0] = 0;
    for (int i = 0; i < n; ++i) {
        nnz += row_nnz[i];
        row_ptr[i+1] = (int)nnz;
    }

    int *col_idx = malloc(sizeof(int) * nnz);
    int *values = malloc(sizeof(int) * nnz);
    if ((nnz > 0 && (!col_idx || !values)) ) {
        fprintf(stderr, "Allocation failed for CSR arrays\n");
        free(dense); free(x); free(y); free(row_nnz); free(row_ptr);
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        int offset = 0;
        int base = row_ptr[i];
        for (int j = 0; j < n; ++j) {
            int v = dense[(long long)i * n + j];
            if (v != 0) {
                int idx = base + offset;
                values[idx] = v;
                col_idx[idx] = j;
                offset++;
            }
        }
    }

    double t_csr_end = now_sec();

    int *x_copy = malloc(sizeof(int) * n);
    int *y_csr = malloc(sizeof(int) * n);
    int *y_dense = malloc(sizeof(int) * n);
    if (!x_copy || !y_csr || !y_dense) {
        fprintf(stderr, "Allocation failed\n");
        goto cleanup;
    }
    memcpy(x_copy, x, sizeof(int) * n);

    for (int i = 0; i < n; ++i) {
        long long sum = 0;
        for (int k = row_ptr[i]; k < row_ptr[i+1]; ++k)
            sum += (long long)values[k] * x_copy[col_idx[k]];
        y_csr[i] = (int)sum;
    }

    for (int i = 0; i < n; ++i) {
        long long sum = 0;
        for (int j = 0; j < n; ++j)
            sum += (long long)dense[(long long)i * n + j] * x_copy[j];
        y_dense[i] = (int)sum;
    }

    int verification_passed = 1;
    for (int i = 0; i < n; ++i) {
        if (y_csr[i] != y_dense[i]) {
            verification_passed = 0;
            break;
        }
    }

    memcpy(x_copy, x, sizeof(int) * n);
    double t_spmv_start = now_sec();
    for (int r = 0; r < reps; ++r) {
        for (int i = 0; i < n; ++i) {
            long long sum = 0;
            for (int k = row_ptr[i]; k < row_ptr[i+1]; ++k)
                sum += (long long)values[k] * x_copy[col_idx[k]];
            y[i] = (int)sum;
        }
        int *tmp = x_copy; x_copy = y; y = tmp;
    }
    double t_spmv_end = now_sec();

    unsigned int seed_restore = base_seed + 12345u;
    for (int i = 0; i < n; ++i) x_copy[i] = (rand_r(&seed_restore) % 9) + 1;

    double t_dense_start = now_sec();
    for (int r = 0; r < reps; ++r) {
        for (int i = 0; i < n; ++i) {
            long long sum = 0;
            for (int j = 0; j < n; ++j)
                sum += (long long)dense[(long long)i * n + j] * x_copy[j];
            y[i] = (int)sum;
        }
        int *tmp = x_copy; x_copy = y; y = tmp;
    }
    double t_dense_end = now_sec();

    double time_init = t_init_end - t_init_start;
    double time_csr_construct = t_csr_end - t_csr_start;
    double time_spmv_total = t_spmv_end - t_spmv_start;
    double time_dense_total = t_dense_end - t_dense_start;

    printf("%d,%d,%d,%lld,%.6f,%.6f,%.6f,%.6f,%s\n",
           n, sparsity, reps, (long long)nnz,
           time_init, time_csr_construct, time_spmv_total, time_dense_total,
           verification_passed ? "PASS" : "FAIL");

cleanup:
    free(dense); free(x); free(y); free(row_nnz); free(row_ptr);
    free(col_idx); free(values);
    free(x_copy); free(y_csr); free(y_dense);
    return 0;
}
