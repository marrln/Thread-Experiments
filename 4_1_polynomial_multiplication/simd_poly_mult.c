#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <immintrin.h>
#include <string.h>

void multiply_polynomials_seq(int *A, int *B, int *C, int n) {
    int size = 2 * n + 1;
    
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

void multiply_polynomials_simd(int *A, int *B, int *C, int n) {
    int size = 2 * n + 1;
    
    __m256i reverse_mask = _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    
    for (int k = 0; k < size; k++) {
        int i_start = (k < n) ? 0 : k - n;
        int i_end   = (k < n) ? k : n;
        int range = i_end - i_start + 1;
        
        __m256i vec_sum = _mm256_setzero_si256();
        int i = i_start;
        
        int simd_end = i_start + (range / 8) * 8;
        for (; i < simd_end; i += 8) {
            __m256i vec_a = _mm256_loadu_si256((__m256i*)&A[i]);
            __m256i vec_b_rev = _mm256_loadu_si256((__m256i*)&B[k - i - 7]);
            __m256i vec_b = _mm256_permutevar8x32_epi32(vec_b_rev, reverse_mask);
            __m256i vec_prod = _mm256_mullo_epi32(vec_a, vec_b);
            vec_sum = _mm256_add_epi32(vec_sum, vec_prod);
        }
        
        __m128i sum_low = _mm256_castsi256_si128(vec_sum);
        __m128i sum_high = _mm256_extracti128_si256(vec_sum, 1);
        __m128i sum_128 = _mm_add_epi32(sum_low, sum_high);
        
        __m128i sum_64 = _mm_hadd_epi32(sum_128, sum_128);
        __m128i sum_final = _mm_hadd_epi32(sum_64, sum_64);
        int sum = _mm_cvtsi128_si32(sum_final);
        
        for (; i <= i_end; i++) {
            sum += A[i] * B[k - i];
        }
        
        C[k] = sum;
    }
}

int verify_results(int *C_seq, int *C_simd, int size) {
    for (int i = 0; i < size; i++) {
        if (C_seq[i] != C_simd[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <degree>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    int size = 2 * n + 1;
    
    struct timeval t_start, t_alloc, t_init, t_seq_end, t_simd_end, t_verify_end, t_end;
    
    gettimeofday(&t_start, NULL);
    
    int *A = malloc((n + 1) * sizeof(int));
    int *B = malloc((n + 1) * sizeof(int));
    int *C_seq = calloc(size, sizeof(int));
    int *C_simd = calloc(size, sizeof(int));
    
    gettimeofday(&t_alloc, NULL);
    
    unsigned int seed = 42;
    for (int i = 0; i <= n; i++) {
        A[i] = (rand_r(&seed) % 9) + 1;
        B[i] = (rand_r(&seed) % 9) + 1;
    }
    
    gettimeofday(&t_init, NULL);
    
    multiply_polynomials_seq(A, B, C_seq, n);
    
    gettimeofday(&t_seq_end, NULL);
    
    multiply_polynomials_simd(A, B, C_simd, n);
    
    gettimeofday(&t_simd_end, NULL);
    
    int verification = verify_results(C_seq, C_simd, size);
    
    gettimeofday(&t_verify_end, NULL);
    
    free(A); free(B); free(C_seq); free(C_simd);
    
    gettimeofday(&t_end, NULL);
    
    double time_alloc = (t_alloc.tv_sec - t_start.tv_sec) + (t_alloc.tv_usec - t_start.tv_usec) / 1000000.0;
    double time_init = (t_init.tv_sec - t_alloc.tv_sec) + (t_init.tv_usec - t_alloc.tv_usec) / 1000000.0;
    double time_seq = (t_seq_end.tv_sec - t_init.tv_sec) + (t_seq_end.tv_usec - t_init.tv_usec) / 1000000.0;
    double time_simd = (t_simd_end.tv_sec - t_seq_end.tv_sec) + (t_simd_end.tv_usec - t_seq_end.tv_usec) / 1000000.0;
    double time_verify = (t_verify_end.tv_sec - t_simd_end.tv_sec) + (t_verify_end.tv_usec - t_simd_end.tv_usec) / 1000000.0;
    double time_cleanup = (t_end.tv_sec - t_verify_end.tv_sec) + (t_end.tv_usec - t_verify_end.tv_usec) / 1000000.0;
    double time_total = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
    
    printf("%f,%f,%f,%f,%f,%f,%f,%s\n",
        time_alloc, time_init, time_seq, time_simd, time_verify, time_cleanup, time_total,
        verification ? "PASS" : "FAIL");
    
    return 0;
}
