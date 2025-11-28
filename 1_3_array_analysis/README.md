
**Exercise 1.3 — Array Analysis**

Overview:
Use threads to analyze large arrays of data (e.g., compute min, max, sum, mean, histogram) by partitioning the input among threads and combining partial results.

Expected Outcome:
- Per-thread local aggregation plus a final reduction yields correct results; for large arrays the parallel version should show measurable speedup, while small arrays are overhead-dominated.

Answer to Handout questions:
- Q: How to avoid synchronization overhead during aggregation? A: Have each thread compute local partials and then combine them serially or with low-overhead atomic merges; reduce frequency of shared updates.
- Q: What affects scalability? A: Data size, memory locality, and the cost of combining partial results; cache-friendly partitioning improves throughput.

Plots:
- `plots/1_3_marrln.png`
- `plots/1_3_phoebus.png`
- `plots/1_3_total.png`