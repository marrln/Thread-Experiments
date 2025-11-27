
**Exercise 1.1 — Polynomial Multiplication**

Overview:
Implement multiplication of two degree-n polynomials using a sequential algorithm and a Pthreads parallel algorithm; verify correctness and measure timings for allocation, initialization, sequential baseline, thread creation, parallel compute, join, cleanup and total time.

Expected Outcome:
- For small degrees the sequential version will usually be faster due to thread overhead; for sufficiently large n the parallel version can outperform the sequential one, but speedup depends on load balance and memory locality.

Answer to Handout questions:
- Q: Do you observe speedup? A: Only for larger polynomial degrees and well-balanced thread counts; small n shows overhead-dominated slowdowns.
- Q: Did you need synchronization? A: No locks are required if each thread writes to disjoint `C[k]` indices; partitioning must ensure no overlapping writes.
- Q: What causes variability in timings? A: Thread creation/join overhead, load imbalance (different work per `k`), cache effects and OS scheduling noise.

Plots:
- ![plot](plots/results_1_1_graph_marrln.png)
- ![plot](plots/results_1_1_graph_phoebus.png)
- ![plot](plots/results_1_1_graph_total.png)
