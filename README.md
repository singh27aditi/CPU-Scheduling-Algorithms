# CPU Scheduling Algorithms

This project implements various CPU scheduling algorithms in C++. The scheduling policies covered include:

- First Come First Serve (FCFS)
- Round Robin (RR) with varying time quantum
- Shortest Process Next (SPN)
- Shortest Remaining Time (SRT)
- Highest Response Ratio Next (HRRN)
- Feedback (FB) and Feedback with varying time quantum (FBV)
- Aging

## Table of Contents
- [Algorithms](#algorithms)
  - [First Come First Serve (FCFS)](#first-come-first-serve-fcfs)
  - [Round Robin (RR)](#round-robin-rr)
  - [Shortest Process Next (SPN)](#shortest-process-next-spn)
  - [Shortest Remaining Time (SRT)](#shortest-remaining-time-srt)
  - [Highest Response Ratio Next (HRRN)](#highest-response-ratio-next-hrrn)
  - [Feedback (FB)](#feedback-fb)
  - [Feedback with varying time quantum (FBV)](#feedback-with-varying-time-quantum-fbv)
  - [Aging](#aging)
- [Input Format](#input-format)

## Algorithms

### First Come First Serve (FCFS)
FCFS is a non-preemptive scheduling algorithm where processes are executed in the order they arrive. While simple to implement, it may lead to long waiting times if large burst-time processes arrive early. It is non-preemptive, meaning a process will run until completion before the next process is executed. While straightforward, it suffers from the convoy effect, where shorter processes may be delayed if a longer one arrives first.

### Round Robin (RR)
RR is a preemptive scheduling algorithm where each process is given a fixed time slice (quantum) before moving to the end of the queue. The variable quantum version allows dynamic adjustment based on process requirements, improving efficiency and reducing starvation. If a process does not complete within its assigned quantum, it is moved to the end of the queue, ensuring fair CPU allocation. This method helps minimize response time but may increase context switching overhead.

### Shortest Process Next (SPN)
SPN selects the process with the shortest burst time for execution. It is non-preemptive and minimizes waiting time but can lead to starvation as longer processes may be continuously delayed.

### Shortest Remaining Time (SRT)
SRT is the preemptive version of SPN, where a new process with a shorter remaining time can interrupt an executing process. This approach reduces average waiting time but may introduce higher overhead due to frequent context switches.

### Highest Response Ratio Next (HRRN)
HRRN prioritizes processes based on their response ratio. The response ratio is calculated by taking the ratio of the waiting time of a process and its burst time. The process with the highest response ratio is executed first, and as new processes arrive, they are added to the queue and sorted based on their response ratio. The process with the highest response ratio will always be at the front of the queue, and thus will always be executed next.

### Feedback (FB)
Feedback scheduling dynamically adjusts process priority. Processes start at a high-priority queue and move to lower-priority queues as they consume CPU time. This approach balances responsiveness and fairness.

### Feedback with Varying Time Quantum (FBV)
FBV extends FB by assigning different time quanta to different priority levels. Higher-priority queues get smaller quanta, while lower-priority queues receive larger ones. This allows efficient handling of both short and long processes while maintaining fairness.

### Aging
Aging prevents starvation by gradually increasing the priority of waiting processes over time. Each scheduling event increments the priority of all ready processes, ensuring that no process is indefinitely postponed.

## Input Format
- **Line 1:** `trace` or `statistics` (determines output format)
- **Line 2:** Comma-separated list of scheduling algorithms to analyze. Algorithms are represented by numbers:
  1. FCFS
  2. RR (requires a quantum parameter, e.g., `2-4` for `q=4`)
  3. SPN
  4. SRT
  5. HRRN
  6. FB-1 (Feedback with `q=1` for all queues)
  7. FB-2i (Feedback with `q=2^i`)
  8. Aging (requires quantum parameter, e.g., `8-1` for `q=1`)
- **Line 3:** Simulation time limit (integer)
- **Line 4:** Number of processes (integer)
- **Lines 5+:** Process descriptions:
  - **For algorithms 1-7:** `Process_Name, Arrival_Time, Service_Time`
  - **For Aging (Algorithm 8):** `Process_Name, Arrival_Time, Priority`

Processes are sorted by arrival time, with ties resolved by priority.
