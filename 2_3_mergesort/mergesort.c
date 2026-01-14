#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

static inline double now_sec() {
    struct timeval t; gettimeofday(&t, NULL); return t.tv_sec + t.tv_usec/1e6;
}

void seq_merge(int *a, int *tmp, int left, int mid, int right) {
    int i = left, j = mid, k = left;
    while (i < mid && j < right) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < mid) tmp[k++] = a[i++];
    while (j < right) tmp[k++] = a[j++];
    for (i = left; i < right; ++i) a[i] = tmp[i];
}

void seq_mergesort_rec(int *a, int *tmp, int left, int right) {
    if (right - left <= 1) return;
    int mid = left + (right - left)/2;
    seq_mergesort_rec(a, tmp, left, mid);
    seq_mergesort_rec(a, tmp, mid, right);
    seq_merge(a, tmp, left, mid, right);
}

void parallel_mergesort_rec(int *a, int *tmp, int left, int right, int depth) {
    if (right - left <= 1024) { // small cutoff
        seq_mergesort_rec(a, tmp, left, right);
        return;
    }
    int mid = left + (right - left)/2;
    #pragma omp task shared(a,tmp) if(depth < 16)
    parallel_mergesort_rec(a, tmp, left, mid, depth+1);
    #pragma omp task shared(a,tmp) if(depth < 16)
    parallel_mergesort_rec(a, tmp, mid, right, depth+1);
    #pragma omp taskwait
    seq_merge(a, tmp, left, mid, right);
}

int cmp_int(const void *a, const void *b) {
    int ai = *(const int*)a; int bi = *(const int*)b; return (ai < bi) ? -1 : (ai > bi);
}

int main(int argc, char *argv[]) {
    if (argc != 3) { fprintf(stderr, "Usage: %s <n> <threads|sequential>\n", argv[0]); return 1; }
    int n = atoi(argv[1]);
    int is_sequential = (strcmp(argv[2], "sequential") == 0);
    int threads = is_sequential ? 0 : atoi(argv[2]);
    if (n <= 0 || (!is_sequential && threads <= 0)) { fprintf(stderr, "Invalid args\n"); return 1; }

    double time_start = now_sec();
    int *a = malloc(n * sizeof(int));
    int *tmp = malloc(n * sizeof(int));
    int *b = malloc(n * sizeof(int)); // for verification
    double time_alloc_end = now_sec();

    unsigned int seed = 42;
    for (int i = 0; i < n; ++i) { a[i] = (rand_r(&seed) % 1000000) - 500000; }
    memcpy(b, a, n * sizeof(int));
    double time_init_end = now_sec();

    double time_compute = 0.0;
    double compute_start = 0.0, compute_end = 0.0;

    if (is_sequential) {
        compute_start = now_sec();
        seq_mergesort_rec(a, tmp, 0, n);
        compute_end = now_sec();
        time_compute = compute_end - compute_start;
    } else {
        omp_set_num_threads(threads);
        if (threads == 1) {
            compute_start = now_sec();
            seq_mergesort_rec(a, tmp, 0, n);
            compute_end = now_sec();
            time_compute = compute_end - compute_start;
        } else {
            compute_start = now_sec();
            #pragma omp parallel
            {
                #pragma omp single nowait
                {
                    parallel_mergesort_rec(a, tmp, 0, n, 0);
                }
            }
            compute_end = now_sec();
            time_compute = compute_end - compute_start;
        }
    }

    double time_verify_start = now_sec();
    qsort(b, n, sizeof(int), cmp_int);
    int ok = 1;
    for (int i = 0; i < n; ++i) if (a[i] != b[i]) { ok = 0; break; }
    double time_verify_end = now_sec();

    double time_cleanup_start = now_sec();
    free(a); free(tmp); free(b);
    double time_end = now_sec();

    double time_alloc = time_alloc_end - time_start;
    double time_init = time_init_end - time_alloc_end;
    double time_verify = time_verify_end - time_verify_start;
    double time_cleanup = time_end - time_cleanup_start;
    double time_total = time_end - time_start;

    if (is_sequential) {
        printf("%d,sequential,%f,%f,%f,%f,%f,%f,%s\n",
            n, time_alloc, time_init, time_compute, time_verify, time_cleanup, time_total,
            ok ? "PASS" : "FAIL");
    } else {
        printf("%d,%d,%f,%f,%f,%f,%f,%f,%s\n",
            n, threads, time_alloc, time_init, time_compute, time_verify, time_cleanup, time_total,
            ok ? "PASS" : "FAIL");
    }

    return 0;
}
