
**Exercise 1.1 — Polynomial Multiplication**

Overview:
Implement multiplication of two degree-n polynomials using a sequential algorithm and a Pthreads parallel algorithm; verify correctness and measure timings for allocation, initialization, sequential baseline, thread creation, parallel compute, join, cleanup and total time.

Expected Outcome:
- For small degrees the sequential version will usually be faster due to thread overhead; for sufficiently large n the parallel version can outperform the sequential one, but speedup depends on load balance and memory locality.

Answer to Handout questions:
- Q: Do you observe speedup? A: Only for larger polynomial degrees and well-balanced thread counts; small n shows overhead-dominated slowdowns.
- Q: Did you need synchronization? A: No locks are required if each thread writes to disjoint `C[k]` indices; partitioning must ensure no overlapping writes.
- Q: What causes variability in timings? A: Thread creation/join overhead, load imbalance (different work per `k`), cache effects and OS scheduling noise.

Results Analysis:
The measurements show that speedup emerges at degree 80,000 and improves further at degree 100,000. With degree 20,000, thread overhead dominates and prevents speedup. Thread creation costs are significant (17ms for 16 threads), but this overhead is amortized as polynomial degree increases. The speedup plateaus around 8 threads; additional threads create contention without benefit due to CPU core limitations and unbalanced workload distribution across threads. Sequential time grows cubically with degree (O(n^2) operations).

Cross-User Comparison (Marr vs Phoebus):
The phoebus user system shows identical speedup patterns to marr but with lower absolute variability across runs. Both users achieve same peak speedups (~5.8× for degree 100K at 16 threads), indicating that speedup ratios are architecture-independent; the parallelization strategy works equally well on both systems. Minor timing differences between users reflect different hardware characteristics (CPU speed, cache hierarchy), but the fundamental speedup ceiling remains constant—demonstrating that algorithmic scalability limits are determined by workload balance and degree, not system speed.

Plots:
- ![plot](plots/speedup_per_degree_marr.png) - Speedup vs thread count for marr user, showing all polynomial degrees (5K, 20K, 80K, 100K). Degree 5K: ~1.2× at 1 thread, ~1.9× at 2 threads, ~2.3× at 4 threads, ~2.5× at 8 threads, ~1.7× at 16 threads (thread overhead dominates small workload). Degree 20K: ~1.2× at 1 thread, ~2.4× at 2, ~2.9× at 4, ~3.9× at 8, ~3.7× at 16 threads. Degree 80K: ~1.3× at 1 thread, ~2.5× at 2, ~3.2× at 4, ~4.9× at 8, ~5.4× at 16 threads (good parallelism). Degree 100K: ~1.3× at 1, ~2.5× at 2, ~3.3× at 4, ~5.0× at 8, ~5.8× at 16 threads (best speedup scaling).
- ![plot](plots/speedup_per_degree_phoebus.png) - Speedup vs thread count for phoebus user. Same overall pattern but with higher variance (different system characteristics). Degree 5K again shows limitations even at 8 threads (~2.5×). Degree 20K shows ~3.9× at 8 threads. Degree 80K achieves ~4.9× at 8 threads. Degree 100K reaches ~5.0× at 8 threads. All degrees show that benefits plateau beyond 8 threads on this system.
- ![plot](plots/speedup_per_degree_all_users.png) - Combined speedup across all users (marr and phoebus). Error bars show variability between users and runs. Confirms critical pattern: degree 5K capped at ~2.5× max; degree 20K reaches ~3.9× at 8 threads; degrees 80K-100K reach ~5.0-5.8× at 8-16 threads. Sweet spot: 8 threads for most degrees. Key insight: speedup limited by O(n²) work distribution—work per thread varies significantly with degree.
