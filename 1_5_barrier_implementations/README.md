
**Exercise 1.5 — Barrier Implementations**
Overview:
Compare three barrier implementations (POSIX `pthread_barrier`, a condition-variable + mutex barrier, and a reusable sense-reversal spin barrier) by synchronizing threads repeatedly and measuring latency and scalability.

Expected Outcome:
- The condition-variable barrier and `pthread_barrier` provide low-overhead blocking synchronization; the sense-reversal centralized spin barrier can be faster for short barriers on dedicated cores but wastes CPU while spinning and may not scale when threads exceed cores.

Answer to Handout questions:
- Q: Which implementation performs best? A: It depends: blocking barriers (pthread_barrier or cond-var) avoid CPU waste and are preferable when threads must yield; spin barriers can be faster when wait times are very short and threads are running on reserved cores.
- Q: What happens when threads > cores? A: Spinning causes contention and wasted CPU; blocking barriers are more robust when oversubscribed.
- Q: Why is sense-reversal reusable? A: It alternates a per-phase sense so threads can wait for the next phase without leftover state; the reversing sense prevents earlier arrivals from matching later phases.

Plots:
- `plots/1_5_marrln.png`
- `plots/1_5_phoebus.png`
- `plots/1_5_total.png`