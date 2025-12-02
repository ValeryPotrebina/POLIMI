# Parallel Computing - Exam Answers
## PC-1st-part-25-07-21.pdf - July 21st, 2025

---

## Exercise 1: PRAM Models (4 points)

### A. On a PRAM model, the time complexity can be equal to the number of instructions executed. True/False? Why? (1 point)

**Answer: False**

**Explanation:**

In a PRAM model, **time complexity is NOT equal to the total number of instructions executed**.

**Why:**

**Time Complexity = Number of Parallel Steps**
- Each parallel step allows **multiple processors** to execute instructions **simultaneously**
- Time is measured by the number of **synchronous parallel rounds**
- All processors that work in a step execute their instructions in **parallel** (same time unit)

**Example:**

Consider summing n=8 numbers with P=4 processors:

**Sequential (1 processor):**
- Instructions: 7 additions
- Time: 7 steps

**Parallel (4 processors):**
```
Step 1: P0: A[0]+A[1]  P1: A[2]+A[3]  P2: A[4]+A[5]  P3: A[6]+A[7]
        (4 instructions execute simultaneously)
Step 2: P0: sum01+sum23  P1: sum45+sum67
        (2 instructions execute simultaneously)
Step 3: P0: final sum
        (1 instruction)

Total instructions executed: 7
Time complexity: 3 steps (NOT 7!)
```

**Key Difference:**
```
Total Work = Number of instructions executed = 7
Time Complexity = Number of parallel steps = 3
```

**Formula:**
```
Time Complexity ≤ Total Work / P
```

**Why it matters:**
- PRAM measures **parallel time** (wall-clock time)
- Multiple instructions in the same step count as **1 time unit**
- This is the whole point of parallel computing: reduce time by executing multiple instructions simultaneously

---

### B. According to the definitions we have seen during the PRAM lecture, how could the speedup be put in relation to the efficiency? (1 point)

**Answer:**

**Speedup and Efficiency are related by:**

```
Efficiency = Speedup / P
```

Or equivalently:

```
Speedup = Efficiency × P
```

**Definitions:**

**Speedup:**
```
SU_P(n) = T*(n) / T_P(n)
```
Where:
- T*(n) = best sequential time (1 processor, best sequential algorithm)
- T_P(n) = parallel time with P processors

**Efficiency:**
```
E_P(n) = T_1(n) / (P × T_P(n))
```
Where:
- T_1(n) = time using parallel algorithm on 1 processor

**Relationship Derivation:**

When the parallel algorithm is also good sequentially (T*(n) ≈ T_1(n)):

```
E_P = T_1 / (P × T_P)
    = (T_1 / T_P) / P
    = SU_P / P
```

**Interpretation:**

| Scenario | Speedup | Efficiency | Meaning |
|----------|---------|-----------|---------|
| **Ideal** | SU = P | E = 1 (100%) | Perfect linear speedup |
| **Good** | SU = 0.8P | E = 0.8 (80%) | Good processor utilization |
| **Poor** | SU = 0.2P | E = 0.2 (20%) | Wasted processors |
| **Super-linear** | SU > P | E > 1 | Better than linear (rare, cache effects) |

**Example:**
```
P = 10 processors
T_1 = 100 seconds
T_P = 15 seconds

Speedup = 100/15 = 6.67
Efficiency = 6.67/10 = 0.667 (66.7%)

Or: Efficiency = 100/(10×15) = 100/150 = 0.667 ✓
```

**Key Insight:** Efficiency tells you what **fraction of ideal speedup** you achieved. High efficiency = processors are well-utilized.

---

### C. Please describe how the matrix vector multiplication can be parallelized on a PRAM model. Please report also speedup, efficiency, work and cost. (1 point)

**Answer:**

**Problem:** Multiply matrix A (n×n) by vector x (n×1) to produce vector y (n×1)

```
y[i] = Σ(A[i][j] × x[j]) for j=0 to n-1
```

**Parallel Algorithm (PRAM):**

Using **P = n² processors**:

```
Algorithm: Matrix-Vector Multiply
Input: Matrix A[n][n], vector x[n]
Output: Vector y[n]

Step 1: (Parallel multiplication)
  For each i,j where 0 ≤ i,j < n (n² processors):
    Processor P[i][j] computes: temp[i][j] = A[i][j] × x[j]
  Time: O(1)

Step 2: (Parallel reduction for each row)
  For each row i (n separate reductions):
    Reduce temp[i][0..n-1] using tree reduction
    Store result in y[i]
  Time: O(log n)

Total Time: O(1) + O(log n) = O(log n)
```

**Performance Metrics:**

**Sequential Time:**
```
T*(n) = n² operations (n rows, each requires n multiplications and n-1 additions)
      ≈ 2n² - n ≈ O(n²)
```

**Parallel Time:**
```
T_P = O(log n)  (with P = n² processors)
```

**Work:**
```
W = P × T_P
  = n² × log n
  = O(n² log n)
```

**Speedup:**
```
SU_P = T* / T_P
     = n² / log n
     = O(n² / log n)
```

**Efficiency:**
```
E_P = SU_P / P
    = (n²/log n) / n²
    = 1 / log n
    = O(1/log n)
```

**Cost:**
```
Cost = P × T_P
     = n² × log n
     = O(n² log n)
```

**Note:** Cost > optimal work (n²), so this is **not work-optimal**.

**Work-Optimal Version (P = n processors):**

```
Each processor handles one row:
  Processor i computes y[i] = Σ A[i][j] × x[j]
  Time: O(n)

T_P = O(n)
Work = n × n = O(n²)  ✓ Optimal!
Speedup = n²/n = n
Efficiency = n/n = 1 (100%)
```

**Summary Table:**

| Metric | P = n² (fully parallel) | P = n (work-optimal) |
|--------|-------------------------|---------------------|
| **Time** | O(log n) | O(n) |
| **Work** | O(n² log n) | O(n²) |
| **Speedup** | O(n²/log n) | O(n) |
| **Efficiency** | O(1/log n) | O(1) |
| **Cost** | O(n² log n) | O(n²) |

**Trade-off:** More processors → faster time but lower efficiency.

---

### D. Among the laws we have seen during the course, which one is classified to address the weak scaling? Please also describe the assumptions that come with the identified law. (1 point)

**Answer:**

**Gustafson's Law** addresses **weak scaling**.

**Formula:**
```
Speedup(P) = s + P(1-s)
```

Where:
- P = number of processors
- s = fraction of time spent on serial portion

**Weak Scaling Definition:**
- **Problem size grows** proportionally with the number of processors
- **Execution time remains constant**
- Measures: "How much bigger problem can we solve with more processors in the same time?"

**Assumptions of Gustafson's Law:**

### **1. Fixed Execution Time**
- Total runtime T is **constant** (fixed)
- When adding processors, we solve a **larger problem** in the **same time**
- Not trying to speed up a fixed problem (that's strong scaling/Amdahl)

### **2. Fixed Serial Time**
- Serial portion takes **s × T** time (absolute time is fixed)
- Serial time does **not grow** with problem size
- Example: Initialization, I/O, final reduction remain constant

### **3. Scalable Parallel Portion**
- Parallel portion takes **(1-s) × T** time on P processors
- If run sequentially, would take **P × (1-s) × T** time
- Parallel work **grows linearly** with number of processors

### **4. Perfect Load Balance**
- Work is evenly distributed among processors
- No idle time or synchronization overhead
- All processors contribute equally

### **5. No Communication Overhead**
- Does not account for communication/synchronization costs
- Assumes inter-processor communication is negligible

**Derivation:**

```
Fixed time T split into:
- Serial: s × T
- Parallel on P processors: (1-s) × T

If this parallel work were done sequentially:
T_seq = s×T + P×(1-s)×T

Speedup = T_seq / T
        = [s×T + P×(1-s)×T] / T
        = s + P(1-s)
```

**Implications:**

```
If s is small (say s=0.05):
  Speedup(100) = 0.05 + 100×0.95 = 95.05
  Near-linear scaling! ✓
```

**Example:**

**Weather Simulation:**
- 1 processor: Simulate with 10km grid resolution (takes 1 hour)
- 100 processors: Simulate with 1km grid resolution (still takes 1 hour)
- **Same time, 100× more detailed simulation** ← Weak scaling

**Contrast with Amdahl's Law (Strong Scaling):**

| Aspect | **Amdahl (Strong)** | **Gustafson (Weak)** |
|--------|-------------------|---------------------|
| **Fixed** | Problem size | Execution time |
| **Variable** | Time (decreases) | Problem size (increases) |
| **Question** | "How much faster?" | "How much bigger?" |
| **Scaling** | Strong scaling | Weak scaling |
| **Outlook** | Pessimistic | Optimistic |

**Key Insight:** Gustafson's Law shows that if your problem can scale (more data, higher resolution, larger simulation), adding processors remains beneficial, unlike Amdahl's Law which suggests diminishing returns.

---

## Exercise 2: Parallel Execution (4 points)

### A. Briefly describe what "Divergent execution" means. Provide also an example to support the description. (1 point)

**Answer:**

**Divergent Execution** (also called **Control Flow Divergence** or **Branch Divergence**) occurs when threads in a **SIMD execution unit** take **different execution paths** due to conditional branches, forcing the hardware to **serialize** execution.

**Context:**
- Occurs in **SIMD/SIMT** architectures (GPUs, vector processors)
- **Same instruction** must execute on **all threads** in a unit (warp)
- When threads need different instructions, hardware must handle it

**How SIMD/SIMT Works:**

**No Divergence (Good):**
```
All threads execute same instruction:
Thread 0-31: ADD r1, r2, r3
→ All execute simultaneously ✓
```

**With Divergence (Bad):**
```cuda
if (threadIdx.x < 16) {
    A;  // Threads 0-15 need this
} else {
    B;  // Threads 16-31 need this
}
```

**Execution becomes:**
```
Step 1: Execute A for threads 0-15 (threads 16-31 are MASKED/IDLE)
Step 2: Execute B for threads 16-31 (threads 0-15 are MASKED/IDLE)
→ Serialized! Takes 2× time
```

**Example: CUDA Warp Divergence**

**Bad (Divergent):**
```cuda
__global__ void divergentKernel(float* data) {
    int idx = threadIdx.x;

    if (idx % 2 == 0) {
        // Expensive computation (100 operations)
        data[idx] = compute_expensive(data[idx]);
    } else {
        // Simple operation (1 operation)
        data[idx] = data[idx] + 1;
    }
}

// Warp has 32 threads:
// - Threads 0,2,4,...,30 execute expensive (16 threads active, 16 idle)
// - Threads 1,3,5,...,31 execute simple (16 threads active, 16 idle)
// Total time = expensive_time + simple_time (serialized!)
```

**Good (No Divergence):**
```cuda
__global__ void coherentKernel(float* data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < N/2) {
        // First half of threads all execute this
        data[idx] = compute_expensive(data[idx]);
    } else {
        // Second half of threads all execute this
        data[idx] = data[idx] + 1;
    }
}

// Warps 0-15: ALL threads do expensive (no divergence)
// Warps 16-31: ALL threads do simple (no divergence)
// Much better performance! ✓
```

**Performance Impact:**

```
Worst case: All 32 threads take different paths
→ 32× slowdown (completely serialized)

Best case: All 32 threads take same path
→ No slowdown (fully parallel)
```

**Key Insight:** In SIMD/SIMT architectures, keep threads in the same execution unit (warp) on the same code path to maximize parallelism and avoid serialization.

---

### B. Describe the difference between the Message Passing and Shared Address Space programming model. (1 point)

**Answer:**

| Aspect | **Shared Address Space** | **Message Passing** |
|--------|-------------------------|-------------------|
| **Memory Model** | Single global address space | Separate private address spaces |
| **Communication** | Implicit (read/write shared variables) | Explicit (send/receive messages) |
| **Data Access** | Direct memory access | Must copy data via messages |
| **Synchronization** | Locks, barriers, atomics, semaphores | Message ordering, blocking calls |
| **Hardware** | Requires cache coherence | Only needs interconnect network |
| **Scalability** | Limited (coherence overhead) | Excellent (no shared state) |
| **Programming** | Easier (familiar memory model) | More explicit control required |
| **Debugging** | Harder (race conditions, deadlocks) | Easier (explicit data flow) |
| **Examples** | Pthreads, OpenMP, CUDA | MPI |

**Shared Address Space:**

**Characteristics:**
- All threads/processes can access same memory locations
- Communication through reading/writing shared variables
- Need explicit synchronization to avoid race conditions
- Hardware maintains **cache coherence** (expensive)

**Example:**
```c
// Shared variable
int counter = 0;
pthread_mutex_t lock;

// Thread A
pthread_mutex_lock(&lock);
counter++;  // Implicit communication
pthread_mutex_unlock(&lock);

// Thread B reads the updated value
pthread_mutex_lock(&lock);
int val = counter;  // Sees Thread A's write
pthread_mutex_unlock(&lock);
```

**Advantages:**
- Natural programming model
- Fine-grained sharing possible
- Low latency for small data

**Disadvantages:**
- Cache coherence overhead
- Race conditions if not careful
- Doesn't scale to large systems

---

**Message Passing:**

**Characteristics:**
- Each process has its **own private memory**
- Cannot directly access another process's memory
- Communication through **explicit send/receive** operations
- Data is **copied** between processes

**Example:**
```c
// MPI example
int data = 42;

// Process 0: Send
if (rank == 0) {
    MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    // Explicit: "Send this data to process 1"
}

// Process 1: Receive
if (rank == 1) {
    int received;
    MPI_Recv(&received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    // Explicit: "Receive data from process 0"
    printf("Received: %d\n", received);
}
```

**Advantages:**
- No cache coherence needed
- Scales to thousands of nodes
- Explicit data flow (easier to reason about)
- Works on commodity hardware/clusters

**Disadvantages:**
- More verbose code
- Communication overhead (copying data)
- Coarse-grained communication
- Must explicitly manage data distribution

---

**When to Use:**

**Shared Address Space:**
- Multi-core CPUs (2-64 cores)
- GPUs (threads within a block)
- Tight coupling, frequent communication
- Small to medium scale parallelism

**Message Passing:**
- Distributed systems, clusters
- Supercomputers (1000s of nodes)
- Loose coupling
- Large-scale parallelism

**Hybrid Approach (Modern Systems):**
```
MPI between nodes (message passing)
  └─ OpenMP within node (shared memory)
```

**Key Difference:** Shared address space hides communication (implicit), message passing exposes it (explicit).

---

### C. Can a program with many arithmetic operations and a small number of memory accesses be a problem for a multi-threaded processor? True/False. Why? (1 point)

**Answer: False**

**Explanation:**

A program with **many arithmetic operations** and **few memory accesses** is **NOT a problem** for a multi-threaded processor. In fact, it's **ideal**!

**Why:**

**Purpose of Multi-threading:**
Multi-threading is designed to **hide memory latency** by switching between threads when one thread stalls on memory access.

**Problem Type Analysis:**

### **Compute-Bound Program (Many arithmetic ops, few memory accesses):**

**Characteristics:**
- High arithmetic intensity (operations/byte)
- ALUs (Arithmetic Logic Units) are the bottleneck
- Few memory stalls
- Doesn't need multi-threading to hide latency

**Behavior:**
```
Thread executes:
[ALU][ALU][ALU][ALU][ALU][ALU][MEM]-----[ALU][ALU]...
                              ↑ Rare stall

Multi-threading helps a bit during rare stalls, but:
- Thread mostly keeps ALUs busy
- No significant idle time to hide
- Single thread achieves high utilization already
```

**Result:**
- ✓ Not a problem
- Multi-threading provides minimal benefit (doesn't hurt, just not necessary)
- Other optimizations matter more (SIMD, superscalar)

---

### **Memory-Bound Program (Few arithmetic ops, many memory accesses):**

**Characteristics:**
- Low arithmetic intensity
- Memory bandwidth is the bottleneck
- Frequent memory stalls
- **This is where multi-threading shines!**

**Behavior:**
```
Thread 0: [ALU][MEM]----------(stall)----------[ALU][MEM]----------
Thread 1:      [ALU][MEM]----------(stall)----------[ALU]
Thread 2:           [ALU][MEM]----------(stall)----------
Thread 3:                [ALU][MEM]----------(stall)

Processor switches between threads during stalls:
Keeps ALUs busy! High utilization ✓
```

**Result:**
- ✓ Multi-threading very beneficial
- Hides memory latency effectively
- Can achieve 2-10× speedup

---

**Formula for Threads Needed:**

```
Threads = (Compute_cycles + Memory_latency) / Compute_cycles
```

**Example:**

**Compute-bound:**
```
100 ALU cycles, 10 memory cycles (latency 50)
Threads needed = (100 + 10×50) / 100 = 600/100 = 6 threads

With 1 thread: 100 + 500 stall = 600 cycles
With 6 threads: ~100 cycles (stalls hidden)
```

**Memory-bound:**
```
10 ALU cycles, 100 memory cycles (latency 50)
Threads needed = (10 + 100×50) / 10 = 5010/10 = 501 threads!

Needs MANY threads to hide latency
```

---

**Key Insight:**

Multi-threaded processors **solve the problem** of memory stalls:

| Program Type | Memory Stalls | Multi-threading Benefit |
|-------------|---------------|----------------------|
| **Compute-bound** | Few | Low (already efficient) |
| **Memory-bound** | Many | High (hides latency) |

**Conclusion:** A compute-bound program (many arithmetic, few memory) is **the GOOD case** for any processor, not a problem. It runs efficiently even without multi-threading. Multi-threading is designed to help the **opposite case** (memory-bound programs).

---

### D. Please compare Coarse-grain multithreading with Simultaneous multithreading. (1 point)

**Answer:**

| Aspect | **Coarse-Grain Multithreading** | **Simultaneous Multithreading (SMT)** |
|--------|--------------------------------|-------------------------------------|
| **Thread Switching** | On **expensive stalls** only (cache miss, etc.) | **Every cycle** (fine-grained) |
| **Execution** | One thread at a time | **Multiple threads simultaneously** |
| **Issue Width** | All issue slots for one thread | Issue slots **shared** among threads |
| **Hardware Cost** | Low (simple context switching) | High (complex scheduling) |
| **Utilization** | Moderate | Highest |
| **Latency Tolerance** | High-latency stalls (100+ cycles) | Any unused slots (1+ cycles) |
| **Example** | Early SPARC | Intel Hyper-Threading, IBM POWER |

---

### **Coarse-Grain Multithreading**

**Operation:**
- Switch threads only on **long-latency events** (cache miss, synchronization)
- **One thread executes** until it stalls
- Context switch takes several cycles
- Next thread runs until it stalls

**Timeline:**
```
Cycles: 0    10   20   30   40   50   60   70
Thread A: [execute]----[STALL---------]
Thread B:                  [execute]----[STALL-----]
Thread C:                                 [execute]--
         ↑                ↑               ↑
      Running T0      Switch to T1    Switch to T2
```

**Characteristics:**
- Hides **long latencies** (50-500 cycles)
- Doesn't help with **short idle times**
- Simple hardware (minimal overhead)
- Wasted cycles during switch

**Best For:**
- Memory-bound applications
- Predictable long stalls
- Simple processor designs

---

### **Simultaneous Multithreading (SMT)**

**Operation:**
- **Multiple threads issue instructions every cycle**
- Dynamically fills issue slots from any ready thread
- No context switching needed
- Threads compete for resources every cycle

**Timeline (4-wide superscalar with 2 threads):**
```
Cycle | Slot 0 | Slot 1 | Slot 2 | Slot 3
------|--------|--------|--------|--------
  1   |  T0    |  T0    |  T1    |  T1    (2 from each)
  2   |  T0    |  T1    |  T1    |  T1    (1 + 3)
  3   |  T0    |  T0    |  T0    |  T0    (T1 stalled)
  4   |  T1    |  T1    |  T1    |  T1    (T0 stalled)
  5   |  T0    |  T1    |  T0    |  T1    (mixed)

Fills all slots every cycle! Maximum utilization ✓
```

**Characteristics:**
- Hides **any idle cycles** (even 1 cycle)
- Multiple threads make progress simultaneously
- Complex scheduling logic required
- Shares execution units, caches, TLB

**Best For:**
- Maximize throughput
- Fill unused execution slots
- Mixed workloads (some threads compute, some wait)

---

### **Direct Comparison Example:**

**Scenario:** 4-wide superscalar processor, single thread uses 2 slots on average

**Without Multithreading:**
```
Utilization: 2/4 = 50%
Wasted: 2 slots per cycle
```

**Coarse-Grain MT (2 threads):**
```
Thread A runs until stall (say 100 cycles)
Switch to Thread B (penalty: 5 cycles)
Thread B runs until stall (100 cycles)
Switch back to Thread A (penalty: 5 cycles)

Effective utilization: ~45-50% (switch overhead)
Hides long stalls ✓
```

**Simultaneous MT (2 threads):**
```
Every cycle: Issue from both threads
Thread A: uses 2 slots
Thread B: uses 2 slots
Total: 4/4 slots used

Utilization: ~95-100%
No switch overhead ✓
Hides all idle time ✓
```

---

### **Trade-offs:**

**Coarse-Grain:**
- ✓ Simple hardware
- ✓ Low overhead
- ✓ Works for memory-bound apps
- ✗ Wastes cycles during short idle times
- ✗ Switch latency
- ✗ One thread blocks others

**Simultaneous:**
- ✓ Maximum utilization
- ✓ Hides all latencies
- ✓ No switch overhead
- ✓ Multiple threads progress simultaneously
- ✗ Complex hardware
- ✗ Resource contention (cache, TLB)
- ✗ Security issues (speculative execution attacks)

---

### **Modern Usage:**

**Coarse-Grain:** Rarely used (outdated)

**Simultaneous (SMT/Hyper-Threading):**
- Intel: 2 threads per core (Hyper-Threading)
- AMD: 2 threads per core (SMT)
- IBM POWER: 4-8 threads per core (SMT4/SMT8)
- GPUs: Implicit fine-grained MT (warps)

**Key Insight:** SMT is superior for throughput, but requires complex hardware. Coarse-grain is simpler but less effective.

---

## Exercise 3: CUDA (4 points)

### A. Describe the distinct types of address spaces visible to kernels in a CUDA/GPU based environment. How many? How much is shared? How fast are they? (2 points)

**Answer:**

CUDA provides **6 distinct memory spaces** visible to kernels:

---

### **1. Global Memory**

**Scope:** All threads in all blocks (entire grid)

**Size:** Largest (4-80 GB on modern GPUs)

**Speed:** **Slowest** (400-800 GB/s bandwidth, 400-800 cycle latency)

**Lifetime:** Persistent across kernel launches

**Declaration:**
```cuda
__device__ int globalVar;  // Global memory

__global__ void kernel(int* data) {
    data[idx] = 42;  // Global memory access
}
```

**Characteristics:**
- Accessible by all threads
- Survives kernel completion
- Can be read/written from host (CPU)
- Cached in L2 and L1 (on newer GPUs)
- **Slowest but largest**

---

### **2. Shared Memory**

**Scope:** All threads within **one block**

**Size:** **48-96 KB per block** (V100: 96 KB per SM)

**Speed:** **Very fast** (~19 TB/s, ~1-2 cycle latency)

**Lifetime:** Exists only during block execution

**Declaration:**
```cuda
__shared__ float sharedData[256];

__global__ void kernel() {
    __shared__ int temp[16][16];  // Per-block shared memory
    temp[threadIdx.y][threadIdx.x] = ...;
    __syncthreads();  // Synchronize access
}
```

**Characteristics:**
- Shared among threads in a block
- Requires `__syncthreads()` for coordination
- Programmer-managed cache
- Used for tiling/blocking optimizations
- **Fast but limited size**

---

### **3. Local Memory**

**Scope:** **Private per thread** (despite name!)

**Size:** Spills from registers when exhausted

**Speed:** **Same as global** (actually stored in global memory)

**Lifetime:** Thread lifetime

**Declaration:**
```cuda
__global__ void kernel() {
    int largeArray[1000];  // Too big for registers → local memory
    // Automatic: compiler decides
}
```

**Characteristics:**
- Not actually "local" - misnomer!
- Used when:
  - Too many registers needed
  - Arrays indexed dynamically
  - Large structures
- Private to each thread but **slow** (stored in DRAM)

---

### **4. Registers**

**Scope:** **Private per thread**

**Size:** **64K registers per SM** (limited per thread)
- V100: Max 255 registers per thread

**Speed:** **Fastest** (~128 TB/s, <1 cycle latency)

**Lifetime:** Thread lifetime

**Declaration:**
```cuda
__global__ void kernel() {
    int x = 5;           // Register
    float temp = x * 2;  // Register
    // Automatic: scalars typically in registers
}
```

**Characteristics:**
- Fastest memory
- Limited quantity
- Managed automatically by compiler
- Register pressure affects occupancy
- **Fastest but most limited**

---

### **5. Constant Memory**

**Scope:** **Read-only** for all threads (entire grid)

**Size:** **64 KB total**

**Speed:** **Fast when cached** (cached in constant cache)
- Broadcast: One read serves entire warp if same address

**Lifetime:** Persistent (set by host)

**Declaration:**
```cuda
__constant__ float constantData[1024];  // Max 64 KB

// Host sets values:
cudaMemcpyToSymbol(constantData, hostData, size);

__global__ void kernel() {
    float val = constantData[5];  // Fast read
}
```

**Characteristics:**
- Read-only by kernels (written from host)
- **Very fast** when all threads read same address
- Cached in dedicated constant cache
- Slower if threads read different addresses
- Limited to 64 KB

---

### **6. Texture Memory**

**Scope:** **Read-only** for all threads (entire grid)

**Size:** Uses global memory (but with special cache)

**Speed:** Cached in texture cache, optimized for 2D/3D locality

**Lifetime:** Persistent

**Declaration:**
```cuda
texture<float, 2> texRef;  // 2D texture

__global__ void kernel() {
    float val = tex2D(texRef, x, y);  // Texture fetch
}
```

**Characteristics:**
- Read-only
- Optimized for spatial locality (2D/3D)
- Hardware filtering/interpolation
- Cached separately from global memory
- Used for image processing

---

### **Summary Table:**

| Memory Type | Scope | Size | Speed (Latency) | Bandwidth | Shared? | Managed By |
|-------------|-------|------|-----------------|-----------|---------|-----------|
| **Registers** | Thread | ~255/thread | Fastest (<1 cycle) | ~128 TB/s | ✗ No | Compiler |
| **Local** | Thread | Spillover | Slow (400 cycles) | ~800 GB/s | ✗ No | Compiler |
| **Shared** | Block | 48-96 KB/block | Very Fast (1-2 cycles) | ~19 TB/s | ✓ Yes (block) | Programmer |
| **Global** | Grid | 4-80 GB | Slow (400 cycles) | ~800 GB/s | ✓ Yes (all) | Programmer |
| **Constant** | Grid (RO) | 64 KB total | Fast (cached) | High (if uniform) | ✓ Yes (all) | Host |
| **Texture** | Grid (RO) | Uses global | Medium (cached) | Medium | ✓ Yes (all) | Host |

---

### **Speed Hierarchy (Fastest → Slowest):**

```
1. Registers          < 1 cycle    [Fastest]
2. Shared Memory      1-2 cycles
3. L1 Cache           ~80 cycles   (if hit)
4. Constant Cache     ~80 cycles   (if hit, uniform access)
5. Texture Cache      ~100 cycles  (if hit)
6. L2 Cache           ~200 cycles  (if hit)
7. Global Memory      400-800 cycles [Slowest]
8. Local Memory       400-800 cycles (same as global)
```

---

### **Usage Guidelines:**

**Optimize Memory Hierarchy:**
1. Keep frequently accessed data in **registers**
2. Use **shared memory** for block-level cooperation
3. Use **constant memory** for read-only, uniform data
4. Minimize **global memory** access
5. Avoid **local memory** (reduce register pressure)

**Key Insight:** Performance in CUDA depends heavily on memory hierarchy. Moving data from global → shared → registers can yield 100-1000× speedup!

---

### **B. Briefly describe how CUDA threads-block are assigned to hardware considering the V100 SM processor architecture discussed during the first part of the course. (2 points)

**Answer:**

**NVIDIA V100 SM Architecture:**

The V100 GPU has:
- **80 SMs** (Streaming Multiprocessors)
- Each SM can execute **multiple blocks concurrently**
- Blocks are assigned at kernel launch and remain on assigned SM until completion

---

### **Block-to-SM Assignment Process:**

### **1. Launch Configuration**

Kernel launch specifies:
```cuda
kernel<<<numBlocks, threadsPerBlock>>>(...);
// Example: kernel<<<1024, 256>>>()
// Creates 1024 blocks, each with 256 threads
```

### **2. Assignment Rules**

**Rule 1: Entire block goes to ONE SM**
- A block **cannot be split** across multiple SMs
- All threads in a block must execute on the same SM
- Reason: Shared memory and `__syncthreads()` require same SM

**Rule 2: Multiple blocks can share ONE SM**
- One SM can execute **multiple blocks concurrently**
- Limited by SM resources (registers, shared memory, thread slots)

**Rule 3: Assignment is done by hardware scheduler**
- Runtime assigns blocks to SMs dynamically
- No programmer control over which block goes where
- Blocks assigned to SMs with available resources

---

### **V100 SM Resource Limits:**

Each SM has:

| Resource | V100 Limit per SM |
|----------|------------------|
| **Max blocks** | 32 blocks |
| **Max warps** | 64 warps (2048 threads) |
| **Max threads** | 2048 threads |
| **Registers** | 65,536 (64K) registers |
| **Shared memory** | 96 KB (configurable with L1) |

---

### **3. Assignment Algorithm**

**Step 1: Check if block fits on SM**
```
For each block:
  Can it fit on an available SM?
  Check:
    - Threads: block_threads ≤ remaining_thread_slots
    - Warps: block_warps ≤ remaining_warp_slots
    - Registers: block_registers ≤ remaining_registers
    - Shared memory: block_shmem ≤ remaining_shmem
    - Block count: current_blocks < 32
```

**Step 2: Assign to SM if all resources available**
```
If all checks pass:
  Assign block to SM
  Reserve resources
  Create warps (groups of 32 threads)
  Load context (registers, shared memory)
```

**Step 3: Execute warps**
```
SM scheduler:
  - Picks ready warps to execute
  - Issues instructions to execution units
  - Warps execute independently
  - Continues until all blocks complete
```

---

### **Example: Resource Calculation**

**Kernel Configuration:**
```cuda
__global__ void kernel() {
    __shared__ int shmem[1024];  // 4 KB shared memory
    // Uses 32 registers per thread (compiler determined)
}

kernel<<<1024, 256>>>();  // 1024 blocks, 256 threads each
```

**Per-Block Resources:**
- Threads: 256
- Warps: 256/32 = 8 warps
- Registers: 256 threads × 32 regs = 8,192 registers
- Shared memory: 4 KB

**How many blocks per SM?**

Check each limit:
1. **Thread limit:** 2048 / 256 = 8 blocks ✓
2. **Warp limit:** 64 / 8 = 8 blocks ✓
3. **Register limit:** 65,536 / 8,192 = 8 blocks ✓
4. **Shared memory limit:** 96 KB / 4 KB = 24 blocks ✓
5. **Block count limit:** 32 blocks ✓

**Bottleneck:** Threads, Warps, and Registers all limit to **8 blocks per SM**

**Result:**
- Each SM runs **8 blocks concurrently**
- 80 SMs × 8 blocks = 640 blocks executing simultaneously
- Remaining 384 blocks (1024-640) wait in queue
- As blocks complete, new blocks are assigned

---

### **4. Occupancy**

**Definition:** Ratio of active warps to maximum warps

```
Occupancy = Active_warps / Max_warps
          = (8 blocks × 8 warps) / 64
          = 64 / 64
          = 100% ✓ Full occupancy!
```

**Impact on Performance:**
- Higher occupancy → better latency hiding
- More warps available to switch between
- But: Not always need 100% occupancy

---

### **5. Dynamic Assignment Example**

**Timeline:**
```
Time T0: Kernel launched with 1024 blocks
  SM 0-79: Each gets assigned blocks (8 each if resources allow)
  Total: 640 blocks executing, 384 queued

Time T1: SM 0 completes one block
  Hardware: Assigns next queued block to SM 0
  SM 0 now has 8 blocks again

Time T2: More blocks complete
  Continues until all 1024 blocks processed
```

---

### **6. Load Balancing**

**Hardware ensures load balancing:**
- Blocks assigned dynamically as SMs become available
- No manual distribution needed
- Automatically adapts to varying block execution times

**Example:**
```
Fast blocks on SM 0: finish quickly → gets more blocks
Slow blocks on SM 1: takes longer → gets fewer new blocks
→ Automatic load balancing ✓
```

---

### **Key Design Principles:**

1. **Independence:** Blocks execute independently (can run in any order)
2. **Scalability:** Same kernel works on GPUs with different SM counts
3. **Resource-driven:** Assignment based on available resources
4. **Transparency:** Programmer doesn't control assignment (hardware decides)

---

### **Optimization Tips:**

**1. Choose block size wisely:**
```
Too small (32 threads): Underutilizes SM, low occupancy
Sweet spot (128-256): Good occupancy, resource balance
Too large (1024): May limit blocks per SM
```

**2. Minimize resource usage:**
```
Fewer registers per thread → more blocks per SM
Less shared memory → more blocks per SM
```

**3. Aim for good occupancy:**
```
Target: 50-100% occupancy
Use CUDA Occupancy Calculator or nvcc --ptxas-options=-v
```

---

**Summary:**

V100 block assignment:
- **80 SMs** available
- Each SM executes **multiple blocks concurrently** (up to 32)
- Limited by **resources** (threads, warps, registers, shared memory)
- **Dynamic assignment** by hardware scheduler
- Blocks remain on **one SM** (cannot split)
- **Occupancy** = ratio of active warps to max warps
- Goal: High occupancy for latency hiding

---

## Exercise 4: OpenMP (4 points)

### A. What happens when a variable is declared as firstprivate in an OpenMP pragma? (1 point)

**Answer:**

**firstprivate** creates a **private copy** of the variable for each thread, and **initializes** each copy with the value from the **master thread before** the parallel region.

**Behavior:**

1. **Private:** Each thread gets its own copy (like `private`)
2. **Initialized:** Copies are initialized with the original value from master thread
3. **Independent:** Each thread can modify its copy without affecting others
4. **No write-back:** Values are NOT copied back after the parallel region

**Example:**

```c
int x = 10;

#pragma omp parallel firstprivate(x)
{
    // Each thread gets its own copy of x initialized to 10
    printf("Thread %d: x = %d\n", omp_get_thread_num(), x);
    x += omp_get_thread_num();  // Modify private copy
    printf("Thread %d: x = %d\n", omp_get_thread_num(), x);
}

printf("After parallel region: x = %d\n", x);  // Still 10!
```

**Output (4 threads):**
```
Thread 0: x = 10
Thread 1: x = 10
Thread 2: x = 10
Thread 3: x = 10
Thread 0: x = 10    (10 + 0)
Thread 1: x = 11    (10 + 1)
Thread 2: x = 12    (10 + 2)
Thread 3: x = 13    (10 + 3)
After parallel region: x = 10  ← Original unchanged
```

**Comparison with Other Clauses:**

| Clause | Private Copy? | Initialized? | Written Back? |
|--------|--------------|--------------|---------------|
| **shared** | ✗ No (single copy) | N/A | N/A |
| **private** | ✓ Yes | ✗ No (uninitialized) | ✗ No |
| **firstprivate** | ✓ Yes | ✓ Yes (from master) | ✗ No |
| **lastprivate** | ✓ Yes | ✗ No | ✓ Yes (from last iteration) |

**When to Use:**

Use `firstprivate` when:
- Need a private copy for each thread
- Want initialization from master thread's value
- Don't need to write results back

**Common Use Case:**
```c
int initial_value = get_initial();

#pragma omp parallel for firstprivate(initial_value)
for (int i = 0; i < n; i++) {
    // Each iteration starts with initial_value
    result[i] = compute(data[i], initial_value);
}
```

---

### B. What is the difference between a static and a dynamic schedule in the OpenMP for construct? (1 point)

**Answer:**

**Static vs Dynamic Scheduling** determines how loop iterations are distributed among threads.

---

### **Static Schedule**

**Definition:** Iterations are **divided into chunks** and **assigned to threads at compile/start time** in a predetermined manner.

**Syntax:**
```c
#pragma omp parallel for schedule(static, chunk_size)
```

**Behavior:**

**Default (no chunk_size):** Divide iterations evenly
```c
#pragma omp parallel for schedule(static)
for (int i = 0; i < 100; i++) { ... }

// With 4 threads:
// Thread 0: iterations 0-24
// Thread 1: iterations 25-49
// Thread 2: iterations 50-74
// Thread 3: iterations 75-99
```

**With chunk_size:** Assign chunks in round-robin
```c
#pragma omp parallel for schedule(static, 10)
for (int i = 0; i < 100; i++) { ... }

// With 4 threads, chunk=10:
// Thread 0: 0-9, 40-49, 80-89
// Thread 1: 10-19, 50-59, 90-99
// Thread 2: 20-29, 60-69
// Thread 3: 30-39, 70-79
```

**Characteristics:**
- ✓ **Low overhead** (assignment done once)
- ✓ **Predictable** (deterministic assignment)
- ✓ **Good cache locality** (consecutive iterations → same thread)
- ✗ **Load imbalance** if iterations have varying work

**Best For:**
- Uniform iteration workload
- Simple loops
- Good cache locality desired

---

### **Dynamic Schedule**

**Definition:** Iterations are assigned to threads **at runtime** as threads become available (work queue approach).

**Syntax:**
```c
#pragma omp parallel for schedule(dynamic, chunk_size)
```

**Behavior:**

**Default chunk_size = 1:**
```c
#pragma omp parallel for schedule(dynamic)
for (int i = 0; i < 100; i++) { ... }

// Runtime assignment (example):
// Thread 0: finished iter 0 → gets iter 4 → gets iter 9 → ...
// Thread 1: finished iter 1 → gets iter 5 → gets iter 7 → ...
// Thread 2: finished iter 2 → gets iter 6 → gets iter 11 → ...
// Thread 3: finished iter 3 → gets iter 8 → gets iter 10 → ...
// Assignment depends on which thread finishes first!
```

**With chunk_size:**
```c
#pragma omp parallel for schedule(dynamic, 10)
for (int i = 0; i < 100; i++) { ... }

// Assigns chunks of 10 iterations dynamically:
// Thread 0: gets chunk 0-9, when done gets next available chunk
// Thread 1: gets chunk 10-19, when done gets next available chunk
// etc.
```

**Characteristics:**
- ✓ **Load balancing** (threads with less work get more iterations)
- ✓ **Handles variable workload** well
- ✗ **Higher overhead** (runtime scheduling)
- ✗ **Non-deterministic** (different runs may assign differently)
- ✗ **Worse cache locality**

**Best For:**
- Variable iteration workload
- Load balancing crucial
- Unpredictable computation time per iteration

---

### **Comparison Example:**

**Scenario:** 100 iterations, 4 threads, iteration i takes i milliseconds

```c
// Iterations: 0(0ms), 1(1ms), 2(2ms), ..., 99(99ms)
// Total work: 0+1+2+...+99 = 4950 ms
```

**Static Schedule (default):**
```
Thread 0: iters 0-24   → time = 0+1+...+24   = 300 ms
Thread 1: iters 25-49  → time = 25+26+...+49 = 925 ms
Thread 2: iters 50-74  → time = 50+51+...+74 = 1550 ms
Thread 3: iters 75-99  → time = 75+76+...+99 = 2175 ms

Total time: 2175 ms (Thread 3 is bottleneck)
Load imbalance: Thread 3 does 7× more work than Thread 0!
```

**Dynamic Schedule (chunk=1):**
```
Each thread gets next iteration when free:
Thread 0: 0, 4, 8, 12, ... (roughly equal work)
Thread 1: 1, 5, 9, 13, ...
Thread 2: 2, 6, 10, 14, ...
Thread 3: 3, 7, 11, 15, ...

Total time: ~1240 ms (much better!)
Better load balance ✓
```

---

### **Summary Table:**

| Aspect | **Static** | **Dynamic** |
|--------|-----------|------------|
| **Assignment** | Compile/start time | Runtime |
| **Overhead** | Very low | Higher |
| **Load Balance** | Poor (if varying work) | Excellent |
| **Cache Locality** | Good (consecutive) | Poor (scattered) |
| **Deterministic** | Yes | No |
| **Use When** | Uniform workload | Variable workload |

---

### **Other Schedules (Bonus):**

**Guided:**
```c
schedule(guided, chunk_size)
// Starts with large chunks, decreases to chunk_size
// Balances between static (low overhead) and dynamic (load balance)
```

**Auto:**
```c
schedule(auto)
// Compiler/runtime decides best schedule
```

**Runtime:**
```c
schedule(runtime)
// Read from OMP_SCHEDULE environment variable
```

---

### **C. Write a small example of nested parallelism in OpenMP and clearly indicate how many threads execute each region. (2 points)

**Answer:**

**Example: Nested Parallel Regions**

```c
#include <omp.h>
#include <stdio.h>

int main() {
    // Enable nested parallelism
    omp_set_nested(1);

    // Set outer level threads
    omp_set_num_threads(2);

    printf("=== Before parallel region ===\n");
    printf("Main thread: %d\n\n", omp_get_thread_num());
    // Threads executing: 1 (main thread)

    // OUTER PARALLEL REGION
    #pragma omp parallel num_threads(2)
    {
        int outer_id = omp_get_thread_num();
        printf("OUTER region - Thread %d (of %d)\n",
               outer_id, omp_get_num_threads());

        // Threads executing outer region: 2 threads

        // INNER PARALLEL REGION (nested)
        #pragma omp parallel num_threads(3)
        {
            int inner_id = omp_get_thread_num();
            printf("  INNER region - Outer thread %d, Inner thread %d (of %d)\n",
                   outer_id, inner_id, omp_get_num_threads());

            // Threads executing inner region:
            // 2 outer threads × 3 inner threads = 6 threads total

            // Some work
            #pragma omp critical
            {
                printf("    Working: Outer[%d] Inner[%d]\n", outer_id, inner_id);
            }
        }
        // After inner region: back to 2 threads (outer level)

        printf("OUTER region - Thread %d finished inner\n", outer_id);
    }
    // After outer region: back to 1 thread (main)

    printf("\n=== After parallel region ===\n");
    printf("Main thread: %d\n", omp_get_thread_num());
    // Threads executing: 1 (main thread)

    return 0;
}
```

**Output (typical):**
```
=== Before parallel region ===
Main thread: 0

OUTER region - Thread 0 (of 2)
OUTER region - Thread 1 (of 2)
  INNER region - Outer thread 0, Inner thread 0 (of 3)
  INNER region - Outer thread 0, Inner thread 1 (of 3)
  INNER region - Outer thread 0, Inner thread 2 (of 3)
  INNER region - Outer thread 1, Inner thread 0 (of 3)
  INNER region - Outer thread 1, Inner thread 1 (of 3)
  INNER region - Outer thread 1, Inner thread 2 (of 3)
    Working: Outer[0] Inner[0]
    Working: Outer[0] Inner[1]
    Working: Outer[0] Inner[2]
    Working: Outer[1] Inner[0]
    Working: Outer[1] Inner[1]
    Working: Outer[1] Inner[2]
OUTER region - Thread 0 finished inner
OUTER region - Thread 1 finished inner

=== After parallel region ===
Main thread: 0
```

---

### **Thread Count Breakdown:**

**1. Before parallel region:**
```
Threads: 1 (main thread)
```

**2. Outer parallel region:**
```
#pragma omp parallel num_threads(2)

Threads: 2 (Outer threads 0 and 1)
```

**3. Inner parallel region (nested):**
```
Each outer thread spawns 3 inner threads:

Outer Thread 0:
  └─ Inner threads: 0, 1, 2  (3 threads)

Outer Thread 1:
  └─ Inner threads: 0, 1, 2  (3 threads)

Total active threads: 2 × 3 = 6 threads
```

**4. After inner region (still in outer):**
```
Threads: 2 (back to outer level)
```

**5. After outer region:**
```
Threads: 1 (back to main thread)
```

---

### **Visual Thread Hierarchy:**

```
Main Thread
    │
    ├─── Outer Thread 0
    │         ├─── Inner Thread 0
    │         ├─── Inner Thread 1
    │         └─── Inner Thread 2
    │
    └─── Outer Thread 1
              ├─── Inner Thread 0
              ├─── Inner Thread 1
              └─── Inner Thread 2

Total threads at deepest nesting: 6
```

---

### **Another Example: Nested Loops**

```c
#include <omp.h>
#include <stdio.h>

int main() {
    int matrix[4][6];

    omp_set_nested(1);
    omp_set_max_active_levels(2);

    // Outer loop: 4 threads (one per row)
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 4; i++) {
        printf("Outer loop - Thread %d processing row %d\n",
               omp_get_thread_num(), i);

        // Threads executing: 4 (one per row)

        // Inner loop: 2 threads per row (process columns)
        #pragma omp parallel for num_threads(2)
        for (int j = 0; j < 6; j++) {
            printf("  Inner loop - Row %d, Col %d, Thread %d\n",
                   i, j, omp_get_thread_num());

            // Threads executing: 4 outer × 2 inner = 8 threads total

            matrix[i][j] = i * 10 + j;
        }
    }

    return 0;
}
```

**Thread Execution:**
```
Outer level: 4 threads (rows 0, 1, 2, 3)
Inner level per outer thread: 2 threads (columns)
Total concurrent threads: 4 × 2 = 8 threads
```

---

### **Important Notes:**

**1. Enable Nested Parallelism:**
```c
omp_set_nested(1);  // Or export OMP_NESTED=true
```

**2. Set Maximum Nesting Levels:**
```c
omp_set_max_active_levels(2);  // Allow 2 levels
```

**3. Thread Oversubscription:**
```
If total threads > physical cores:
  → OS time-shares (context switching overhead)
  → Performance may degrade
```

**4. Use Cases for Nested Parallelism:**
- Matrix operations (parallel rows, parallel columns)
- Recursive algorithms (merge sort, quicksort)
- Multi-level simulations
- Outer loop over tasks, inner loop over data

---

**Summary:** The example shows nested parallelism with:
- **Outer region:** 2 threads
- **Inner region:** 3 threads per outer thread
- **Total active threads at deepest level:** 6 threads
- Threads are created and destroyed as regions are entered/exited

---

## Summary

All answers are now based on the course material from parallel-computing.pdf and use correct terminology and concepts for the exam!
