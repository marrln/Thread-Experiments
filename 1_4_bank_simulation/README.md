
**Exercise 1.4 — Bank Simulation**
Overview:
Simulate a bank where multiple teller threads perform transactions on shared account balances (transfers and balance queries). Implement synchronization variants and measure correctness and performance.

Expected Outcome:
- Correct behavior requires synchronization for updates; coarse-grained locking is simple but can limit throughput, fine-grained locking or per-account locking improves concurrency. Measuring throughput and customer wait times shows the trade-offs.

Answer to Handout questions:
- Q: What synchronization is needed? A: Use mutexes for account updates and queue operations; condition variables let tellers wait for customers. Fine-grained locks per account reduce contention compared to a single global lock.
- Q: How does critical-section length affect results? A: Longer critical sections (e.g., additional work after reading balance) increase contention and make read-write locks or fine-grained locks more beneficial.
- Q: How to evaluate designs? A: Measure correctness (no lost updates), throughput (customers/sec), average wait time, and time spent blocked on locks.

Plots:
- `plots/1_4_marrln.png`
- `plots/1_4_phoebus.png`
- `plots/1_4_total.png`