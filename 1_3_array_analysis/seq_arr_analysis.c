#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

struct array_stats_s {
    long long int info_array_0;
    long long int info_array_1;
    long long int info_array_2;
    long long int info_array_3;
};

/* Sequential analysis: count non-zero elements per array */
void analyze_arrays_seq(int **arrays, int size, struct array_stats_s *stats)
{
    long long count;

    count = 0; for (int j = 0; j < size; j++) if (arrays[0][j] != 0) count++; stats->info_array_0 = count;
    count = 0; for (int j = 0; j < size; j++) if (arrays[1][j] != 0) count++; stats->info_array_1 = count;
    count = 0; for (int j = 0; j < size; j++) if (arrays[2][j] != 0) count++; stats->info_array_2 = count;
    count = 0; for (int j = 0; j < size; j++) if (arrays[3][j] != 0) count++; stats->info_array_3 = count;
}

int main(int argc, char *argv[])
{
    if (argc != 2) { printf("Usage: %s <array_size>\n", argv[0]); return 1; }

    int size = atoi(argv[1]);
    int *arrays[4];
    struct array_stats_s stats = {0};

    struct timeval t_start, t_alloc, t_init, t_compute, t_end;
    gettimeofday(&t_start, NULL);

    for (int i = 0; i < 4; i++) arrays[i] = malloc(size * sizeof(int));
    gettimeofday(&t_alloc, NULL);

    srand(time(NULL));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < size; j++)
            arrays[i][j] = rand() % 10;
    gettimeofday(&t_init, NULL);

    analyze_arrays_seq(arrays, size, &stats);
    gettimeofday(&t_compute, NULL);

    for (int i = 0; i < 4; i++) free(arrays[i]);
    gettimeofday(&t_end, NULL);

    double time_alloc   = (t_alloc.tv_sec - t_start.tv_sec)   + (t_alloc.tv_usec - t_start.tv_usec)/1e6;
    double time_init    = (t_init.tv_sec - t_alloc.tv_sec)   + (t_init.tv_usec - t_alloc.tv_usec)/1e6;
    double time_compute = (t_compute.tv_sec - t_init.tv_sec) + (t_compute.tv_usec - t_init.tv_usec)/1e6;
    double time_total   = (t_end.tv_sec - t_start.tv_sec)    + (t_end.tv_usec - t_start.tv_usec)/1e6;

    printf("sequential,%d,%lld,%lld,%lld,%lld,%.6f,%.6f,0.0,0.0,%.6f,0.0,0.0,%.6f,PASS\n",
        size, stats.info_array_0, stats.info_array_1,
        stats.info_array_2, stats.info_array_3,
        time_alloc, time_init, time_compute, time_total
    );

    return 0;
}
