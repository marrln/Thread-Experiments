
**Exercise 1.2 — Shared Variable Update**
Overview:
Threads repeatedly update a shared variable; implement versions with no synchronization, with mutex/locks, and with atomic operations, then measure correctness and performance.

Synchronization Methods:

1. **Mutex (Mutual Exclusion Lock)**
   - Uses `pthread_mutex_lock/unlock` to serialize access to the shared variable
   - Only one thread can hold the lock at a time
   - Each thread increments by 1, so atomic operation per iteration is needed
   - High overhead due to context switching and lock contention

2. **Read-Write Lock (RWLock)**
   - Uses `pthread_rwlock_wrlock/unlock` for write access
   - Allows multiple readers but exclusive writer access
   - In this exercise, only writes occur, so it behaves similarly to mutex
   - Comparable overhead to mutex since all operations are writes

3. **Atomic Operations**
   - Uses `__atomic_fetch_add()` with `__ATOMIC_SEQ_CST` memory ordering
   - Employs local accumulation: each thread counts locally, then atomically adds the total
   - Single atomic operation per thread instead of per-increment
   - Significantly lower overhead than mutex/rwlock since synchronization happens once

Why Sequential is Faster:

The sequential baseline is faster because:
- **No synchronization overhead**: No locks, mutexes, or atomic operations needed
- **No context switching**: Single thread executes continuously without preemption
- **No memory bus contention**: No inter-thread communication or cache coherency overhead
- **Optimal CPU cache utilization**: All data stays in L1/L2 cache
- **Linear execution**: Pure computation without synchronization delays

The parallel versions must pay the cost of thread creation, coordination, and synchronization. The sequential version is the theoretical minimum for the pure computation work.

Expected Outcome:
- Unsynchronized updates will produce incorrect results due to races; mutex and atomic variants produce correct results. Atomics often outperform coarse-grained mutexes under light contention.

Answer to Handout questions:
- Q: Is synchronization necessary? A: Yes—without synchronization updates to a shared counter will race and produce wrong values.
- Q: Which synchronization performs better? A: It depends on contention: atomic operations (fetch_add) typically have lower overhead than a global mutex; fine-grained locking or per-thread batching can improve throughput further.
- Q: How to measure fairness and overhead? A: Measure total completed updates, per-thread throughput, and time spent in synchronization primitives.

Plots:
- ![plot](plots/plot_1_2_marrln.png)
- ![plot](plots/plot_1_2_phoebus.png)
- ![plot](plots/plot_1_2_total.png)

