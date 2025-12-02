# Parallel Computing - Exam Answers
## PC-1st-part-23-06-08.pdf - June 8th, 2023

---

## Exercise 1: PRAM Models (4 points)

### A. Describe the steps followed by a PRAM machine (1 point)

**Answer:**

A PRAM (Parallel Random Access Machine) follows a **synchronous five-phase computation model**:

### **Phase 1: Read from Input**
- Processors read from input cells X⁽¹⁾, X⁽²⁾, ...
- Input data is loaded into processor registers
- All processors can read simultaneously

### **Phase 2: Read from Shared Memory**
- Processors read from shared memory cells A⁽¹⁾, A⁽²⁾, ...
- Access depends on PRAM model:
  - **ER (Exclusive Read):** Only distinct locations
  - **CR (Concurrent Read):** Same location allowed

### **Phase 3: Internal Computation**
- Each processor performs local computations
- Uses data from input and shared memory
- Operations on local registers only
- No communication during this phase

### **Phase 4: Write to Output**
- Processors may write to output cells Y⁽¹⁾, Y⁽²⁾, ...
- Results are produced
- Optional phase (not all algorithms need output every step)

### **Phase 5: Write to Shared Memory**
- Processors may write to shared memory cells A⁽¹⁾, A⁽²⁾, ...
- Access depends on PRAM model:
  - **EW (Exclusive Write):** Only distinct locations
  - **CW (Concurrent Write):** Same location with conflict resolution
- Used for inter-processor communication

**Key Characteristics:**

1. **Synchronous:** All processors execute the same phase simultaneously
2. **Lock-step:** Phases execute in strict order
3. **Unit Time:** Each phase takes one time unit
4. **No Explicit Communication:** Communication only through shared memory

**Example: Parallel Sum (one step)**
```
Phase 1: Each processor i reads X[i]
Phase 2: Processor i reads shared variable partial_sum
Phase 3: Processor i computes: temp = X[i] + partial_sum
Phase 4: (skip)
Phase 5: Processor i writes temp to shared memory
```

**Time Complexity:** Number of complete 5-phase cycles executed

---

### B. All processors in a PRAM model can have non-uniform memory access. True or False? (1 point)

**Answer: False**

**Explanation:**

In the **PRAM model**, all processors have **uniform memory access** (UMA). This is a fundamental assumption of the PRAM abstraction.

**PRAM Memory Access Properties:**

1. **Unit Time Access:**
   - All processors can access **any memory cell** in **unit time**
   - No distance-dependent latency
   - No bandwidth limitations

2. **Uniform Cost:**
   - Access to memory cell A[1] takes same time as A[1000000]
   - All processors experience same access time
   - No hierarchy (no cache, no NUMA)

3. **Idealized Model:**
   - Abstracts away real hardware complexities
   - Simplifies algorithm design and analysis
   - Not physically realizable at large scale

**Why False:**

The PRAM model **explicitly assumes uniform access**. Non-uniform memory access (NUMA) is a characteristic of **real systems**, not the PRAM abstraction.

**Real Systems vs PRAM:**

| Aspect | **PRAM Model** | **Real Systems (NUMA)** |
|--------|---------------|------------------------|
| **Access Time** | Uniform (same for all) | Non-uniform (varies by location) |
| **Memory Location** | Doesn't matter | Local vs remote memory |
| **Latency** | Always 1 unit | Varies (10-100+ cycles) |
| **Bandwidth** | Infinite | Limited |

**NUMA in Real Systems:**

Real multi-processor systems often have NUMA:
```
Processor 0 → Local Memory (fast, 50ns)
           → Remote Memory (slow, 150ns)

Processor 1 → Local Memory (fast, 50ns)
           → Remote Memory (slow, 150ns)
```

**PRAM Abstraction:**
```
All processors → Any memory cell (same time)
```

**Key Point:** PRAM is an **idealized model** with uniform access. It's used for algorithm design and complexity analysis, not to model real hardware accurately.

---

### C. Complete the following sentence and explain what it means: "A write conflict occurs when..." (1 point)

**Answer:**

**"A write conflict occurs when two or more processors attempt to write to the same memory location simultaneously."**

**Explanation:**

### **What is a Write Conflict?**

In a PRAM model, during **Phase 5 (Write to Shared Memory)**, multiple processors may try to write to the same memory address at the same time. This creates ambiguity about which value should be stored.

**Example:**
```
Time Step T:
  Processor 0: writes value 10 to A[5]
  Processor 1: writes value 20 to A[5]
  Processor 2: writes value 30 to A[5]

Question: What value should A[5] contain?
→ This is a write conflict!
```

### **How Different PRAM Models Handle Write Conflicts:**

**1. EREW/CREW (Exclusive Write):**
```
Write conflicts are FORBIDDEN
- Algorithm must ensure no simultaneous writes to same location
- Responsibility of the algorithm designer
- If conflict occurs → undefined behavior
```

**2. CRCW (Concurrent Write) with Conflict Resolution:**

Different CRCW variants handle conflicts differently:

**a) Priority CRCW:**
```
Processor with lowest ID wins:
  P0 writes 10
  P1 writes 20  } → A[5] = 10 (P0 has priority)
  P2 writes 30
```

**b) Common CRCW:**
```
Write succeeds ONLY if all values are identical:
  P0 writes 10
  P1 writes 10  } → A[5] = 10 ✓ (all same)
  P2 writes 10

  P0 writes 10
  P1 writes 20  } → FAIL ✗ (values differ)
  P2 writes 30
```

**c) Arbitrary CRCW:**
```
Random processor wins:
  P0 writes 10
  P1 writes 20  } → A[5] = ? (could be 10, 20, or 30)
  P2 writes 30
```

### **Example Algorithm with Write Conflict:**

**OR operation using Common CRCW:**
```
Algorithm: Parallel OR
Input: Array A[n] of boolean values

Initialize: result = 0

For each processor i (in parallel):
  If A[i] = 1:
    Write 1 to result  // Multiple processors may write 1
                       // Common CRCW allows this (all write same value)
```

**Write Conflict:**
- If A[3] = 1 and A[7] = 1:
  - P3 writes 1 to result
  - P7 writes 1 to result
  - Conflict! But Common CRCW allows (both write 1)

### **Why Write Conflicts Matter:**

1. **Algorithm Design:**
   - Must choose appropriate PRAM model
   - EREW requires careful coordination
   - CRCW allows more flexible algorithms

2. **Complexity:**
   - CRCW can solve some problems faster
   - Example: OR in O(1) on CRCW, O(log n) on EREW

3. **Hardware Realization:**
   - EREW/CREW easier to implement
   - CRCW requires special hardware support

**Key Insight:** Write conflicts are fundamental to understanding PRAM model variants and their capabilities.

---

### D. Amdahl's model is a fixed-time model. True/False? (1 point)

**Answer: False**

**Explanation:**

**Amdahl's Law** is a **fixed-size model** (also called **fixed-work model**), NOT a fixed-time model.

### **Amdahl's Law (Fixed-Size):**

**Assumption:** Problem size N is **constant**

**Question:** "How much faster can we solve this problem with P processors?"

**Formula:**
```
Speedup(P) = 1 / [(1-f) + f/P]

Where:
- f = parallelizable fraction
- (1-f) = serial fraction
- N = constant (fixed problem size)
```

**Behavior:**
- **Fixed work:** Same problem regardless of P
- **Variable time:** Time decreases with more processors
- **Diminishing returns:** Speedup saturates due to serial portion

**Example:**
```
Process 1 million elements:
- 1 processor:  100 seconds
- 10 processors: 15 seconds  (fixed work, less time)
- 100 processors: 11 seconds (approaching limit)
```

---

### **Gustafson's Law (Fixed-Time):**

**Assumption:** Execution time T is **constant**

**Question:** "How much bigger problem can we solve with P processors in the same time?"

**Formula:**
```
Speedup(P) = s + P(1-s)

Where:
- s = serial time fraction
- T = constant (fixed time)
- Problem size scales with P
```

**Behavior:**
- **Fixed time:** Same execution time regardless of P
- **Variable work:** Problem size increases with processors
- **Linear scaling:** Near-linear speedup possible

**Example:**
```
Run simulation for 100 seconds:
- 1 processor:  1 million elements
- 10 processors: 9.5 million elements  (more work, same time)
- 100 processors: 95 million elements
```

---

### **Comparison:**

| Aspect | **Amdahl (Fixed-Size)** | **Gustafson (Fixed-Time)** |
|--------|------------------------|---------------------------|
| **Fixed** | Problem size (N) | Execution time (T) |
| **Variable** | Time (decreases) | Problem size (increases) |
| **Scaling Type** | Strong scaling | Weak scaling |
| **Outlook** | Pessimistic | Optimistic |
| **Question** | "How much faster?" | "How much bigger?" |

---

### **Why the Distinction Matters:**

**Amdahl's Law (Fixed-Size):**
```
Analyze existing applications:
- Same input, more processors
- Desktop/workstation applications
- Benchmark suites
```

**Gustafson's Law (Fixed-Time):**
```
Design scalable HPC systems:
- Bigger simulations, same deadline
- Scientific computing
- Weather modeling, molecular dynamics
```

**Key Insight:** Amdahl is **not** fixed-time; it's fixed-work. Gustafson is the fixed-time model.

---

## Exercise 2: Parallel Execution (4 points)

### A. Please summarize the main differences between SIMD and Multi-core. (1 point)

**Answer:**

| Aspect | **SIMD** | **Multi-core** |
|--------|----------|----------------|
| **Parallelism Type** | Data-level parallelism | Thread-level parallelism |
| **Instruction Stream** | **Single** instruction | **Multiple** independent instructions |
| **Data Stream** | **Multiple** data elements | Multiple independent data |
| **Control** | One control unit | Multiple independent control units |
| **Execution** | Same operation on all elements | Different operations per core |
| **Programming** | Vectorization (compiler/explicit) | Threading (explicit: pthreads, OpenMP) |
| **Synchronization** | Implicit (lock-step) | Explicit (locks, barriers) |
| **Best For** | Regular, data-parallel operations | Independent tasks |
| **Examples** | SSE, AVX, AVX-512, GPU warps | Intel Core i7, AMD Ryzen |

---

### **SIMD (Single Instruction, Multiple Data):**

**Characteristics:**
- **One instruction** operates on **multiple data elements** simultaneously
- Single control unit broadcasts instruction to many ALUs
- All units execute same operation in lock-step
- Explicit data parallelism

**Example:**
```c
// Scalar (sequential):
for (int i = 0; i < 16; i++)
    C[i] = A[i] + B[i];  // 16 separate operations

// SIMD (AVX-512):
// One instruction: "add 16 floats"
__m512 a = _mm512_load_ps(A);
__m512 b = _mm512_load_ps(B);
__m512 c = _mm512_add_ps(a, b);  // 16 additions simultaneously!
_mm512_store_ps(C, c);
```

**Execution:**
```
Cycle 1: ADD instruction
  ALU 0: A[0] + B[0]
  ALU 1: A[1] + B[1]
  ALU 2: A[2] + B[2]
  ...
  ALU 15: A[15] + B[15]

All ALUs execute same operation (ADD) on different data
```

**Hardware:**
- One instruction decoder
- Multiple ALUs (4-16+)
- Vector registers (128-512 bits)

**Pros:**
- ✓ Amortizes instruction fetch/decode
- ✓ High throughput for data-parallel ops
- ✓ Power efficient (one control unit)

**Cons:**
- ✗ Divergence problems (if/else)
- ✗ Requires vectorizable code
- ✗ Same operation for all

---

### **Multi-core:**

**Characteristics:**
- **Multiple complete CPUs** on one chip
- Each core has its own control unit
- Cores execute **different instructions** independently
- Explicit thread parallelism

**Example:**
```c
// Multi-core (OpenMP):
#pragma omp parallel
{
    int id = omp_get_thread_num();

    // Core 0 does task A
    if (id == 0) processImages();

    // Core 1 does task B
    if (id == 1) compressAudio();

    // Core 2 does task C
    if (id == 2) networkIO();

    // Different tasks on different cores!
}
```

**Execution:**
```
Cycle 1:
  Core 0: LOAD instruction (task A)
  Core 1: ADD instruction (task B)
  Core 2: BRANCH instruction (task C)
  Core 3: MUL instruction (task D)

Each core executes different instruction independently
```

**Hardware:**
- Multiple complete processors
- Each with fetch, decode, execute
- Separate register files
- May share cache (L2/L3)

**Pros:**
- ✓ True independent execution
- ✓ Different programs simultaneously
- ✓ Flexible task assignment
- ✓ Handles diverse workloads

**Cons:**
- ✗ Higher hardware cost
- ✗ Synchronization overhead
- ✗ Cache coherence complexity

---

### **Direct Comparison Example:**

**Task:** Add 1 million numbers

**SIMD Approach:**
```
One core with AVX-512:
- Processes 16 numbers per instruction
- 1,000,000 / 16 = 62,500 iterations
- Sequential across iterations
- Data-parallel within iteration
```

**Multi-core Approach:**
```
4 cores, each adds 250,000 numbers:
- Core 0: sum(numbers[0..249999])
- Core 1: sum(numbers[250000..499999])
- Core 2: sum(numbers[500000..749999])
- Core 3: sum(numbers[750000..999999])
- Then combine results
- Task-parallel
```

**Best: Combine both!**
```
4 cores, each using AVX-512:
- Each core processes 250K numbers with SIMD
- Data-parallel (SIMD) + Thread-parallel (multi-core)
- Maximum performance!
```

---

### **Key Differences Summary:**

**SIMD:**
- **Same** instruction, **different** data
- Data-level parallelism
- Single control flow
- Example: Vector addition

**Multi-core:**
- **Different** instructions, **different** data
- Thread-level parallelism
- Multiple control flows
- Example: Running multiple applications

**Modern Processors:** Use **both**!
- Multi-core: 4-64 cores
- Each core has SIMD: AVX-512 (16-wide)
- Total parallelism: cores × SIMD width

---

### B. Define what a memory stall is and why caches reduce the length of stalls. (1 point)

**Answer:**

### **Memory Stall:**

A **memory stall** is a period when the **processor is idle**, waiting for data to arrive from memory because the requested data is not immediately available.

**Causes:**
1. **Cache miss:** Data not in cache, must fetch from DRAM
2. **Cache line fill:** Waiting for cache line to load
3. **Memory latency:** Slow DRAM access time

**Impact:**
```
Processor cycle: 0.3 ns (3 GHz CPU)
DRAM latency: 100 ns

Memory stall: ~300 cycles of wasted time!
```

**Example:**
```c
int x = array[1000000];  // Cache miss!

Cycles 1-5:   Fetch instruction ✓
Cycle 6:      Decode instruction ✓
Cycle 7:      Execute: request memory load ✓
Cycles 8-307: STALL (waiting for DRAM)
              → Processor idle, doing nothing!
Cycle 308:    Data arrives, continue execution ✓
```

**Performance Impact:**
```
Without stalls: Execute 1 instruction/cycle
With stalls:    Execute 1 instruction/300 cycles
→ 300× slower!
```

---

### **Why Caches Reduce Stall Length:**

Caches are **small, fast memory** placed between the processor and main memory (DRAM).

### **1. Lower Latency:**

**DRAM vs Cache Access Times:**
```
L1 Cache:    ~1 ns  (~4 cycles)
L2 Cache:    ~3 ns  (~12 cycles)
L3 Cache:    ~10 ns (~40 cycles)
DRAM:        ~100 ns (~300 cycles)
```

**Cache Hit:**
```
int x = array[100];  // Data in L1 cache

Cycles 1-5: Fetch/decode
Cycle 6:    Execute: load from L1
Cycle 7-10: Wait for L1 (4 cycles)  ← Short stall
Cycle 11:   Data arrives, continue

Stall length: 4 cycles (not 300!)
```

---

### **2. Temporal Locality:**

**Principle:** Recently accessed data likely to be accessed again

**Example:**
```c
for (int i = 0; i < 1000; i++) {
    sum += array[i];  // First access: miss, loads to cache
                      // Same cache line used for array[i+1], array[i+2], ...
}

First access to array[0]: 300-cycle stall (load from DRAM)
Next 7 accesses (array[1-7]): 0-cycle stall (already in cache line!)
```

**Cache Line:**
```
Load array[0] → brings entire cache line (64 bytes) to cache
  [array[0], array[1], ..., array[15]]  (16 ints × 4 bytes)

Next 15 accesses: cache hits (no stall!)
```

---

### **3. Spatial Locality:**

**Principle:** Nearby data likely to be accessed together

**Example:**
```c
// Sequential access
for (int i = 0; i < N; i++)
    sum += array[i];

Cache line prefetching:
- Access array[0] → loads array[0-15] to cache
- Access array[1-15]: All hits! (no stall)
- Access array[16] → loads array[16-31] to cache
- Access array[17-31]: All hits!

Stall frequency: 1 miss per 16 accesses (6.25%)
```

**Random Access (Poor Locality):**
```c
// Random access
for (int i = 0; i < N; i++)
    sum += array[random()];

Every access may miss cache:
- No spatial locality (not sequential)
- No temporal locality (not reused)
- Stall frequency: ~90% misses
```

---

### **4. Hierarchy Effect:**

**Multi-level Cache Hierarchy:**
```
Request data:
  Check L1 (1ns) → Miss
  Check L2 (3ns) → Miss
  Check L3 (10ns) → Hit! ✓
  Return data

Stall: 10ns instead of 100ns (10× reduction)
```

**Hit Rate Impact:**
```
Assume:
- L1 hit rate: 95%
- L2 hit rate: 90% (of L1 misses)
- L3 hit rate: 80% (of L2 misses)

Average access time:
  0.95 × 1ns (L1 hit)
+ 0.05 × 0.90 × 3ns (L2 hit)
+ 0.05 × 0.10 × 0.80 × 10ns (L3 hit)
+ 0.05 × 0.10 × 0.20 × 100ns (DRAM)
= 0.95 + 0.135 + 0.04 + 0.1
= 1.225 ns average

Without cache: 100 ns
With cache: 1.225 ns
→ 80× faster!
```

---

### **Summary:**

**Memory Stall:**
- Processor waits for data from memory
- Can last 100-300 cycles
- Severely degrades performance

**Caches Reduce Stalls By:**
1. **Lower latency:** 4-40 cycles vs 300 cycles
2. **Temporal locality:** Reuse cached data (0-cycle access)
3. **Spatial locality:** Prefetch nearby data (amortize miss cost)
4. **Hit rate:** 90-99% of accesses hit cache (avoid DRAM)

**Result:** Average memory access time reduced by 10-100×

---

### C. Explain why "A processor with multiple hardware threads has the ability to avoid stalls." (1 point)

**Answer:**

A processor with **multiple hardware threads** (multithreading) can **avoid stalls** by **switching to another thread** when one thread is waiting for memory, keeping the processor busy.

---

### **How Multithreading Avoids Stalls:**

### **Single-Threaded Processor (Stalls):**

```
Thread A execution:
Cycles 1-10:  Execute instructions (ALU busy) ✓
Cycle 11:     Load from memory (cache miss)
Cycles 12-311: STALL (waiting for memory)
               → Processor IDLE, wasted cycles!
Cycle 312:    Data arrives, resume execution ✓

Utilization: 10/(10+300) = 3.2%  (terrible!)
```

---

### **Multi-Threaded Processor (No Stalls):**

```
Thread A:
Cycles 1-10:  Execute instructions ✓
Cycle 11:     Load from memory (cache miss)
              → Thread A blocks, switch to Thread B

Thread B:
Cycles 12-111: Execute instructions (100 cycles) ✓
Cycle 112:    Load from memory (cache miss)
              → Thread B blocks, switch to Thread C

Thread C:
Cycles 113-212: Execute instructions (100 cycles) ✓
Cycle 213:    Load from memory
              → Thread C blocks, switch to Thread A

Thread A:
Cycle 214:    Data arrived! Resume execution ✓

Utilization: 300/312 = 96%  (excellent!)
```

---

### **Key Mechanism: Context Switching**

**Hardware maintains multiple contexts:**
```
Context = {
  Program counter (PC)
  Registers (32-64 registers)
  Stack pointer
  Processor state
}

Thread 0: Context 0 (stored in hardware)
Thread 1: Context 1 (stored in hardware)
Thread 2: Context 2 (stored in hardware)
Thread 3: Context 3 (stored in hardware)
```

**When stall occurs:**
```
1. Thread A stalls on memory access
2. Hardware saves state (already in registers, no copy needed!)
3. Hardware switches to Thread B (PC ← Thread B's PC)
4. Thread B executes immediately (no overhead!)
5. When Thread A's data arrives, switch back
```

**Fast Switching:**
- Context already in hardware registers
- Switch time: **0-1 cycles** (near-instant)
- No OS involvement
- No cache pollution

---

### **Why This Works:**

### **1. Latency Hiding:**

**Memory latency:** 300 cycles
**Thread switch:** 1 cycle

```
If we have 4 threads:
- Thread 0 stalls (300 cycles)
- Threads 1, 2, 3 execute (300 cycles total)
- By the time thread 3 finishes, thread 0's data ready!
- No idle cycles!
```

**Formula for Threads Needed:**
```
Num_threads = Memory_latency / Compute_cycles

Example:
Memory latency = 300 cycles
Compute per thread = 100 cycles
Threads needed = 300/100 = 3 threads
```

---

### **2. Independent Execution:**

**Threads are independent:**
```
Thread A: Processing image 0
Thread B: Processing image 1
Thread C: Processing image 2
Thread D: Processing image 3

No dependencies → can switch freely
```

**When Thread A stalls:**
```
Threads B, C, D continue their work
Processor stays busy!
```

---

### **Example: GPU Warp Scheduling**

**GPUs use extreme multithreading:**
```
NVIDIA V100 SM:
- 64 warps (32 threads each = 2048 threads!)
- When warp 0 stalls on memory:
  - Switch to warp 1 (1 cycle)
  - Switch to warp 2 (1 cycle)
  - ...
  - Switch to warp 63 (1 cycle)
- By the time we cycle through 64 warps, warp 0's data ready!
- Near 100% utilization!
```

**Each Cycle:**
```
Cycle N:   Warp 5 executes (warp 0-4 stalled on memory)
Cycle N+1: Warp 12 executes (warp 5 now stalled)
Cycle N+2: Warp 23 executes
...

Always a ready warp to execute!
```

---

### **Types of Multithreading:**

**1. Fine-Grained (Switch every cycle):**
```
Cycle 1: Thread A
Cycle 2: Thread B
Cycle 3: Thread C
Cycle 4: Thread D
Cycle 5: Thread A (back to start)

Always switching, maximum latency hiding
```

**2. Coarse-Grained (Switch on long stalls):**
```
Cycles 1-100: Thread A executes
Cycle 101: Thread A stalls → Switch to Thread B
Cycles 102-200: Thread B executes
Cycle 201: Thread B stalls → Switch to Thread A

Only switch when necessary
```

**3. Simultaneous (SMT/Hyper-Threading):**
```
Multiple threads execute simultaneously:
Cycle 1: Issue 2 instructions from Thread A + 2 from Thread B
Cycle 2: Issue 1 instruction from Thread A + 3 from Thread B

Fill all execution slots every cycle
```

---

### **Benefits:**

**1. Hide Memory Latency:**
```
Without threading: Stall 300 cycles
With threading: Switch to other thread (0 cycle penalty)
```

**2. Increase Throughput:**
```
Single thread: 1 instruction per 300 cycles (stalled)
4 threads: 4 instructions per cycle (interleaved)
→ 1200× improvement!
```

**3. Improve Utilization:**
```
Single thread: 3% ALU utilization
4 threads: 95% ALU utilization
```

---

### **Trade-offs:**

**Advantages:**
- ✓ Hides memory latency
- ✓ High throughput
- ✓ Simple hardware (just extra registers)
- ✓ No software changes needed

**Disadvantages:**
- ✗ Each thread runs slower (shares resources)
- ✗ Need many threads (pressure on registers, cache)
- ✗ Doesn't reduce latency for single thread

---

### **Summary:**

**How Multithreading Avoids Stalls:**

1. **Multiple contexts** stored in hardware
2. When thread **stalls on memory** → switch to another thread
3. **Other thread executes** while first waits
4. **Fast switching** (0-1 cycles)
5. By the time we cycle through threads, **data is ready**
6. Processor **never idle**

**Key Insight:** Multiple threads turn processor stalls into useful work time by interleaving execution.

---

### D. Write a short description of "Simultaneous multi-threading." (1 point)

**Answer:**

**Simultaneous Multithreading (SMT)** is a technique where **multiple threads execute concurrently** on a **single processor core**, sharing execution resources and issuing instructions from **different threads in the same cycle**.

---

### **Key Characteristics:**

### **1. Multiple Threads, One Core:**
- Single physical core
- Multiple hardware threads (typically 2-8)
- Each thread has separate architectural state (PC, registers)
- Shared execution units (ALUs, FPUs, caches)

### **2. Simultaneous Execution:**
- Instructions from **multiple threads** issued **in the same cycle**
- Dynamic resource sharing
- Fills execution slots from any ready thread

**Example (4-wide superscalar with 2 threads):**
```
Cycle N: Issue slots available: [_ _ _ _]
         Thread A has 2 instructions ready
         Thread B has 2 instructions ready

         Issue: [A A B B]  ← Both threads execute simultaneously!
```

---

### **How It Works:**

### **Hardware Support:**

**Per-Thread State (Duplicated):**
```
Thread 0: PC₀, Registers₀, Stack₀
Thread 1: PC₁, Registers₁, Stack₁
```

**Shared Resources:**
```
- Execution units (ALUs, FPUs, Load/Store)
- Caches (L1, L2, L3)
- TLB (Translation Lookaside Buffer)
- Branch predictor
```

---

### **Execution Example:**

**4-wide superscalar core, 2 threads:**

```
Cycle 1:
  Thread A: ADD, SUB, LOAD, (stalled on branch)
  Thread B: MUL, DIV, (stalled)

  Issue: [A:ADD][A:SUB][A:LOAD][B:MUL]
         ↑ 3 from Thread A, 1 from Thread B
         All 4 slots filled!

Cycle 2:
  Thread A: (stalled on LOAD miss)
  Thread B: ADD, ADD, SUB, XOR

  Issue: [B:ADD][B:ADD][B:SUB][B:XOR]
         ↑ All from Thread B (Thread A stalled)

Cycle 3:
  Thread A: STORE, ADD
  Thread B: LOAD, MUL

  Issue: [A:STORE][A:ADD][B:LOAD][B:MUL]
         ↑ Mixed! 2 from each thread
```

**Key Point:** Dynamically fills all available execution slots from any thread with ready instructions.

---

### **Comparison with Other Multithreading:**

| Type | Thread Switching | Simultaneous? | Example |
|------|-----------------|---------------|---------|
| **Fine-Grained MT** | Every cycle (round-robin) | ✗ One at a time | Early GPUs |
| **Coarse-Grained MT** | On long stalls only | ✗ One at a time | Early SPARC |
| **Simultaneous MT (SMT)** | Every cycle (dynamic) | ✓ Multiple together | Intel HT, AMD SMT |

---

### **Benefits:**

### **1. Maximum Resource Utilization:**

**Without SMT (single thread):**
```
4-wide core, thread has 2 instructions ready:
Issue: [T0:ADD][T0:MUL][____][____]
Utilization: 50% (2/4 slots used)
```

**With SMT (2 threads):**
```
Thread 0: 2 instructions ready
Thread 1: 2 instructions ready
Issue: [T0:ADD][T0:MUL][T1:SUB][T1:DIV]
Utilization: 100% (4/4 slots used)
```

### **2. Hide Latency:**

```
Thread A stalls on memory:
  → Issue from Thread B instead
  → No idle cycles!
```

### **3. Throughput Improvement:**

**Typical improvements:**
- 1.2-1.5× throughput vs single-threaded
- Better utilization of wide superscalar cores
- Hides memory latency, branch mispredictions

---

### **Examples:**

### **Intel Hyper-Threading:**
```
Each physical core → 2 logical processors
Core i7: 8 cores × 2 threads = 16 logical processors
OS sees 16 CPUs, but only 8 physical cores
```

### **AMD SMT:**
```
Ryzen: 8 cores × 2 threads = 16 threads
Similar to Intel Hyper-Threading
```

### **IBM POWER:**
```
POWER9: 4-8 threads per core (SMT4/SMT8)
Extreme multithreading for server workloads
```

---

### **Trade-offs:**

**Advantages:**
- ✓ Higher throughput (1.2-1.5×)
- ✓ Better resource utilization
- ✓ Hides latencies (memory, cache miss)
- ✓ No change to software (transparent)

**Disadvantages:**
- ✗ Resource contention (threads compete)
- ✗ Each thread runs slower than alone
- ✗ Complex hardware (scheduling, fairness)
- ✗ Security vulnerabilities (Spectre, Meltdown)
- ✗ Cache/TLB pressure

---

### **Performance Example:**

```
Single-threaded:
  Utilization: 40% (many idle slots)
  IPC: 1.6 instructions per cycle

SMT (2 threads):
  Utilization: 75% (more slots filled)
  IPC: 3.0 instructions per cycle
  Throughput: 1.875× (3.0/1.6)

Each thread runs slower (shares resources)
But total throughput increases!
```

---

### **Summary:**

**Simultaneous Multithreading (SMT):**
- **Multiple threads execute together** in same cycle
- **Dynamically share** execution units
- **Fill unused slots** with instructions from different threads
- **Maximize throughput** by improving resource utilization
- **Transparent** to software (OS sees multiple CPUs)

**Key Insight:** SMT converts vertical waste (idle execution slots) into horizontal parallelism (multiple threads) for better throughput.

**Intel calls it:** Hyper-Threading
**AMD calls it:** SMT
**IBM calls it:** SMT4/SMT8

---

## Exercise 3: CUDA (4 points)

### A. Is CUDA a data-parallel programming model? (1 point)

**Answer: Yes**

**Explanation:**

**CUDA is a data-parallel programming model** where the same operation (kernel) is applied to many data elements concurrently by many threads.

---

### **Why CUDA is Data-Parallel:**

### **1. SIMT (Single Instruction, Multiple Threads):**

**Concept:**
- Single kernel function
- Executed by thousands/millions of threads
- Each thread processes different data element
- Same code, different data (data parallelism!)

**Example:**
```cuda
// Vector addition kernel
__global__ void vectorAdd(float* A, float* B, float* C, int N) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N)
        C[i] = A[i] + B[i];  // Same operation, different data
}

// Launch with 1M threads
vectorAdd<<<4096, 256>>>(A, B, C, 1000000);

// Each thread executes same code (C[i] = A[i] + B[i])
// But on different data element (different i)
// → Data parallelism!
```

---

### **2. Many Threads, Same Kernel:**

**Data Distribution:**
```
1 million elements → 1 million threads

Thread 0:     C[0] = A[0] + B[0]
Thread 1:     C[1] = A[1] + B[1]
Thread 2:     C[2] = A[2] + B[2]
...
Thread 999999: C[999999] = A[999999] + B[999999]

Same operation (addition), different data indices
```

---

### **3. Warp Execution (SIMD-like):**

**Hardware Implementation:**
```
Warp = 32 threads executing in lock-step
All threads in warp execute same instruction
But on different data registers

Warp 0: Threads 0-31 execute "ADD r1, r2, r3"
  Thread 0: r1₀ = r2₀ + r3₀  (data₀)
  Thread 1: r1₁ = r2₁ + r3₁  (data₁)
  ...
  Thread 31: r1₃₁ = r2₃₁ + r3₃₁  (data₃₁)

Same instruction (ADD), different data
→ SIMD/Data parallelism at hardware level!
```

---

### **Characteristics of Data-Parallel Model:**

**1. Same Operation:**
```cuda
All threads execute same kernel code:
__global__ void kernel(float* data, int n) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    data[idx] = process(data[idx]);  // Same function!
}
```

**2. Different Data:**
```
Each thread has unique index (idx)
Accesses different memory location
Processes independent data element
```

**3. Independent Execution:**
```
Thread i's computation doesn't depend on thread j
Can execute in any order
Scalable parallelism
```

---

### **Classic Data-Parallel Patterns in CUDA:**

**Map:**
```cuda
// Apply function to each element
__global__ void map(float* input, float* output, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n)
        output[i] = f(input[i]);  // Data-parallel map
}
```

**Reduce:**
```cuda
// Parallel reduction (sum all elements)
// Each thread processes different portion of data
__global__ void reduce(float* input, float* output, int n) {
    // Tree-based reduction
    // Data-parallel at each level
}
```

**Stencil:**
```cuda
// Each thread updates one output element
// Based on neighboring input elements
__global__ void stencil(float* in, float* out, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    out[i] = (in[i-1] + in[i] + in[i+1]) / 3.0;
    // Data-parallel: all threads do same computation
}
```

---

### **Contrast with Other Models:**

**Data-Parallel (CUDA):**
```
Same operation, different data
Thousands of threads, one kernel
Example: Process each pixel of image
```

**Task-Parallel:**
```
Different operations, different data
Few threads, different functions
Example: Thread 1 renders, Thread 2 handles input
```

**CUDA vs Task-Parallel:**
```c
// Task-parallel (CPU/pthreads):
void* thread1_func(void* arg) { processImages(); }
void* thread2_func(void* arg) { compressAudio(); }
pthread_create(&t1, NULL, thread1_func, NULL);  // Different functions!

// Data-parallel (CUDA):
__global__ void processPixels(pixel* data, int n) { ... }
processPixels<<<10000, 256>>>(image, N);  // Same function, different data!
```

---

### **Why Data-Parallel is Perfect for GPUs:**

**1. Massive Parallelism:**
```
Need thousands of threads → data parallelism provides them
Same kernel → simple hardware (shared instruction cache)
```

**2. SIMT Execution:**
```
Warps execute same instruction
Perfect for data-parallel workloads
Divergence is penalty (when threads do different things)
```

**3. Scalability:**
```
Add more data → add more threads
Same kernel code works on any GPU (10 SMs or 80 SMs)
Automatic load balancing
```

---

### **Summary:**

**Yes, CUDA is data-parallel because:**
1. **Same kernel** executed by many threads
2. **Different data** processed by each thread
3. **SIMT execution:** threads grouped in warps execute in lock-step
4. **Scalable:** add more data → add more threads
5. **Independent:** thread computations are independent

**Key Formula:**
```
Data Parallelism = Same Operation × Many Data Elements
CUDA = One Kernel × Millions of Threads × Different Indices
```

---

### B. How scalar registers for CUDA thread are organized in an NVIDIA V100 SM "sub-core"? (1 point)

**Answer:**

In the **NVIDIA V100 SM architecture**, scalar registers are organized into a **large register file** that is **partitioned** and **shared dynamically** among all threads executing on that SM.

---

### **V100 SM Register Organization:**

### **1. Register File Structure:**

**Per SM:**
```
Total Registers: 65,536 registers (64K × 32-bit registers)
Organization: Unified register file
Allocation: Dynamic, per-warp basis
```

**Partition:**
```
V100 SM has 4 "sub-cores" (processing blocks):
- Each sub-core has: 16,384 registers (16K registers)
- 4 sub-cores × 16K = 64K total registers per SM
```

---

### **2. Register Allocation:**

**Per-Thread Basis:**
```
Each thread gets allocated a fixed number of registers
Determined at compile time (by compiler)
Maximum: 255 registers per thread (V100)
```

**Per-Warp Allocation:**
```
Warp = 32 threads
If each thread uses R registers:
  Warp needs: 32 × R registers

Example:
  Thread uses 32 registers
  Warp needs: 32 × 32 = 1,024 registers
```

---

### **3. Dynamic Partitioning:**

**Register File is Shared:**
```
Not dedicated per thread
Allocated dynamically when block/warp is scheduled
Released when block/warp completes
```

**Example:**
```
Scenario: Kernel uses 64 registers per thread

Sub-core has 16,384 registers
Per warp: 32 threads × 64 regs = 2,048 registers

Max warps per sub-core: 16,384 / 2,048 = 8 warps

If each warp needs 2,048 registers:
  Can fit 8 warps simultaneously
  8 warps × 32 threads = 256 threads active
```

---

### **4. Register File Access:**

**Banking:**
```
Register file is banked for parallel access
Multiple threads in warp can access registers simultaneously
Typically 32 banks (one per thread in warp)
```

**Conflict-Free Access:**
```
Thread 0 reads R0₀   Bank 0
Thread 1 reads R0₁   Bank 1
Thread 2 reads R0₂   Bank 2
...
Thread 31 reads R0₃₁ Bank 31

All access in parallel (1 cycle)
```

---

### **5. Register Pressure and Occupancy:**

**Register Pressure:**
```
More registers per thread → fewer warps per SM

Example:
Kernel A: 32 regs/thread → 8 warps/sub-core → 32 warps/SM
Kernel B: 64 regs/thread → 4 warps/sub-core → 16 warps/SM
Kernel C: 128 regs/thread → 2 warps/sub-core → 8 warps/SM

Higher register usage → lower occupancy!
```

**Occupancy:**
```
Occupancy = Active warps / Max warps
V100: Max 64 warps per SM (2048 threads)

If kernel uses 64 regs/thread:
  Max warps = 65,536 / (32 × 64) = 32 warps
  Occupancy = 32/64 = 50%
```

---

### **6. Register Spilling:**

**When registers are exhausted:**
```
If kernel needs more than 255 registers per thread:
  → Compiler spills registers to "local memory"
  → Local memory = DRAM (slow!)
  → Performance penalty: 100-300× slower

Example:
  int largeArray[1000];  // Too big for registers
  → Spilled to local memory
  → Access time: 400 cycles instead of 1 cycle
```

---

### **Architectural Details:**

**V100 SM Structure:**
```
SM (Streaming Multiprocessor)
├─ Sub-core 0 (Processing Block)
│  ├─ 16,384 registers
│  ├─ 8 FP64 cores
│  ├─ 16 FP32 cores
│  ├─ 16 INT32 cores
│  └─ 2 Tensor cores
├─ Sub-core 1
│  └─ 16,384 registers ...
├─ Sub-core 2
│  └─ 16,384 registers ...
└─ Sub-core 3
   └─ 16,384 registers ...

Total: 64K registers across 4 sub-cores
```

---

### **Register Allocation Example:**

**Kernel Configuration:**
```cuda
__global__ void kernel() {
    float a, b, c, d;  // 4 registers
    int i, j;          // 2 registers
    // Total: 6 registers per thread (compiler determined)
}

kernel<<<1000, 256>>>();  // 1000 blocks, 256 threads each
```

**Per Block:**
```
Threads per block: 256
Warps per block: 256/32 = 8 warps
Registers per thread: 6
Total registers needed: 8 × 32 × 6 = 1,536 registers
```

**SM Allocation:**
```
SM has 65,536 registers
Blocks per SM: 65,536 / 1,536 = 42 blocks (register-limited)

But V100 max: 32 blocks per SM
So: Limited by block count, not registers ✓
```

---

### **Optimization Tips:**

**1. Minimize Register Usage:**
```cuda
// Bad: Many variables
float a, b, c, d, e, f, g, h;  // 8 registers

// Good: Reuse variables
float temp;
temp = compute1();
output1 = temp;
temp = compute2();  // Reuse same register
output2 = temp;
```

**2. Check Register Usage:**
```bash
nvcc --ptxas-options=-v kernel.cu

Output:
  ptxinfo    : Used 32 registers, 1024 bytes smem

→ Shows registers per thread
```

**3. Limit Registers:**
```cuda
// Force max 32 registers per thread
__global__ void __launch_bounds__(256, 4) kernel() {
    // Compiler will use max 32 registers
}
```

---

### **Summary:**

**V100 SM Register Organization:**
- **65,536 registers per SM** (64K × 32-bit)
- **4 sub-cores**, each with **16,384 registers**
- **Shared dynamically** among all threads/warps
- **Allocated per-warp** (32 threads × R registers)
- **Banked** for parallel access (32 banks)
- **Max 255 registers per thread**
- **Register pressure** affects occupancy

**Key Point:** Registers are a limited, shared resource. Lower register usage → more warps → higher occupancy → better performance.

---

### C. Describe the distinct types of address spaces visible to kernels. How many? How much is shared? How faster are they? (2 points)

**Answer:**

CUDA kernels have access to **6 distinct memory/address spaces**, each with different **scope**, **size**, **speed**, and **sharing** characteristics.

---

### **1. Registers**

**Scope:** Private per thread

**Size:**
- V100: Max 255 registers per thread
- Total: 65,536 registers per SM

**Speed:** **Fastest** (~1 cycle latency, ~128 TB/s bandwidth)

**Shared?** ✗ No (private to each thread)

**Characteristics:**
- Automatic scalars, small arrays
- Fastest memory
- Limited quantity
- Register pressure affects occupancy

**Example:**
```cuda
__global__ void kernel() {
    int x = 5;        // Register
    float temp = 3.14; // Register
    // Fastest access!
}
```

---

### **2. Local Memory**

**Scope:** Private per thread (despite the name!)

**Size:** Spillover from registers (uses global memory)

**Speed:** **Same as global memory** (~400 cycle latency, ~800 GB/s)

**Shared?** ✗ No (private to thread, but stored in DRAM)

**Characteristics:**
- Used when register file exhausted
- Large arrays, dynamically indexed arrays
- Actually stored in global memory (cached in L1/L2)
- **Slow** despite "local" name (misnomer!)

**Example:**
```cuda
__global__ void kernel() {
    int bigArray[1000];  // Too big → local memory (slow!)
    int idx = threadIdx.x;
    int val = bigArray[idx];  // Dynamic index → local memory
}
```

---

### **3. Shared Memory**

**Scope:** Shared by all threads in a **block**

**Size:**
- V100: **96 KB per SM** (configurable with L1 cache)
- Per block: Up to 96 KB

**Speed:** **Very fast** (~1-2 cycle latency, ~19 TB/s bandwidth)

**Shared?** ✓ Yes (within block, ~256-1024 threads)

**Characteristics:**
- Programmer-managed cache
- Requires `__syncthreads()` for synchronization
- On-chip SRAM (fast!)
- Used for tiling/blocking optimizations
- Bank conflicts can reduce performance

**Example:**
```cuda
__shared__ float sharedData[256];  // Shared by all threads in block

__global__ void kernel() {
    int tid = threadIdx.x;
    sharedData[tid] = input[tid];  // Load to shared memory
    __syncthreads();               // Synchronize

    float val = sharedData[tid + 1];  // Fast access within block!
}
```

---

### **4. Global Memory**

**Scope:** All threads in **all blocks** (entire grid)

**Size:** **4-32 GB** (V100: 16 or 32 GB HBM2)

**Speed:** **Slow** (~400-800 cycle latency, ~800-900 GB/s bandwidth)

**Shared?** ✓ Yes (all threads can access)

**Characteristics:**
- Largest memory
- Persistent across kernel launches
- Accessible from host (CPU)
- Cached in L2 and L1 (on newer GPUs)
- Coalescing critical for performance

**Example:**
```cuda
__global__ void kernel(float* data, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    data[idx] = data[idx] * 2.0f;  // Global memory access (slow)
}
```

---

### **5. Constant Memory**

**Scope:** Read-only for all threads (entire grid)

**Size:** **64 KB total**

**Speed:** **Fast when cached** (~cache speed if uniform access)
- Broadcast: 1 read serves entire warp if same address
- Cached in constant cache (8-64 KB per SM)

**Shared?** ✓ Yes (read-only, all threads)

**Characteristics:**
- Set by host (CPU)
- Read-only in kernel
- **Very fast** if all threads read same address
- Slower if threads read different addresses (serialized)
- Limited to 64 KB

**Example:**
```cuda
__constant__ float constants[1024];  // 64 KB max

// Host sets:
cudaMemcpyToSymbol(constants, hostData, size);

__global__ void kernel() {
    float c = constants[5];  // Fast read (broadcast to warp)
}
```

---

### **6. Texture Memory**

**Scope:** Read-only for all threads (entire grid)

**Size:** Uses global memory (but with texture cache)

**Speed:** Cached in texture cache, optimized for 2D spatial locality

**Shared?** ✓ Yes (read-only, all threads)

**Characteristics:**
- Read-only
- Optimized for 2D/3D spatial locality
- Hardware filtering/interpolation
- Cached separately from global memory
- Used for image processing

**Example:**
```cuda
texture<float, 2> texRef;

__global__ void kernel() {
    float val = tex2D(texRef, x, y);  // Texture fetch with interpolation
}
```

---

### **Summary Table:**

| Memory Type | Scope | Size | Latency | Bandwidth | Shared? | Cached? |
|-------------|-------|------|---------|-----------|---------|---------|
| **Registers** | Thread | ~255/thread | <1 cycle | ~128 TB/s | ✗ No | N/A |
| **Local** | Thread | Spillover | ~400 cycles | ~800 GB/s | ✗ No | L1/L2 |
| **Shared** | Block | 96 KB/SM | 1-2 cycles | ~19 TB/s | ✓ Yes (block) | N/A (on-chip) |
| **Global** | Grid | 16-32 GB | 400-800 cycles | ~800 GB/s | ✓ Yes (all) | L2, L1 |
| **Constant** | Grid (RO) | 64 KB | ~80 cycles | High (if uniform) | ✓ Yes (all) | Const cache |
| **Texture** | Grid (RO) | Uses global | ~100 cycles | Medium | ✓ Yes (all) | Texture cache |

---

### **Speed Hierarchy (Fastest → Slowest):**

```
1. Registers           <1 cycle      [Fastest] 🚀
2. Shared Memory       1-2 cycles    [Very Fast] 🔥
3. L1 Cache            ~80 cycles    [Fast] (if hit)
4. Constant Cache      ~80 cycles    [Fast] (if uniform)
5. Texture Cache       ~100 cycles   [Medium]
6. L2 Cache            ~200 cycles   [Medium]
7. Global Memory       400-800 cycles [Slow] 🐌
8. Local Memory        400-800 cycles [Slow] 🐌
```

---

### **Bandwidth Comparison:**

```
Registers:       ~128,000 GB/s (per SM)
Shared Memory:   ~19,000 GB/s (per SM)
Global Memory:   ~800 GB/s (entire GPU)

→ Registers are 160× faster than global memory!
→ Shared memory is 24× faster than global memory!
```

---

### **Sharing Characteristics:**

**Not Shared (Private):**
- Registers: Each thread has its own
- Local Memory: Each thread has its own (but slow)

**Shared within Block:**
- Shared Memory: All threads in block can access

**Shared Globally:**
- Global Memory: All threads in all blocks
- Constant Memory: Read-only, all threads
- Texture Memory: Read-only, all threads

---

### **Usage Guidelines:**

**1. Maximize Register Usage:**
```cuda
Use registers for:
- Scalars (int, float)
- Frequently accessed variables
- Loop counters
```

**2. Use Shared Memory for Cooperation:**
```cuda
Use shared memory for:
- Data reuse within block
- Tiling/blocking (matrix multiply)
- Reduction within block
- Inter-thread communication
```

**3. Minimize Global Memory Access:**
```cuda
Minimize global memory by:
- Loading once to shared memory
- Reusing data from shared memory
- Coalescing accesses
- Using constant memory for read-only data
```

**4. Use Constant Memory for Parameters:**
```cuda
Use constant memory for:
- Algorithm parameters
- Lookup tables (if uniform access)
- Read-only configuration data
```

---

### **Performance Impact Example:**

**Bad (Global Memory):**
```cuda
__global__ void badKernel(float* data, int n) {
    int idx = threadIdx.x;
    for (int i = 0; i < 100; i++) {
        data[idx] += 1.0f;  // 100 global memory accesses (slow!)
    }
}
// Time: ~40,000 cycles (100 × 400 cycles)
```

**Good (Register + Shared Memory):**
```cuda
__shared__ float shared[256];

__global__ void goodKernel(float* data, int n) {
    int idx = threadIdx.x;
    float temp = data[idx];  // 1 global memory read

    for (int i = 0; i < 100; i++) {
        temp += 1.0f;  // Register access (fast!)
    }

    data[idx] = temp;  // 1 global memory write
}
// Time: ~1,000 cycles (2 × 400 + 100 × 1)
// → 40× faster!
```

---

### **Key Takeaways:**

**6 memory types** in CUDA:
1. **Registers** - fastest, private
2. **Local** - slow (misnomer), private
3. **Shared** - very fast, block-scoped
4. **Global** - slow, grid-scoped
5. **Constant** - fast (if uniform), read-only
6. **Texture** - cached, read-only

**Optimization strategy:**
- Keep data in **registers** (fastest)
- Use **shared memory** for block cooperation
- Minimize **global memory** access
- Use **constant memory** for read-only data

**Performance:** Registers are **160× faster** than global memory!

---

## Exercise 4: Memory and Heterogeneous Systems (4 points)

### A. Please briefly describe what DRAM burst means. (1 point)

**Answer:**

**DRAM burst** is a mode where **multiple consecutive data words** are transferred in a **single memory transaction** after one address is specified, significantly improving memory throughput.

---

### **How DRAM Burst Works:**

### **1. Traditional DRAM Access (No Burst):**

```
Read one word:
  1. Send address
  2. Activate row (RAS - Row Address Strobe)
  3. Access column (CAS - Column Address Strobe)
  4. Transfer data
  5. Precharge

Time: ~100 ns for 8 bytes
Bandwidth: 80 MB/s
```

### **2. DRAM Burst Mode:**

```
Read burst of N words:
  1. Send starting address once
  2. Activate row once (RAS)
  3. Access first column (CAS)
  4. Transfer data word 1
  5. Auto-increment column address
  6. Transfer data word 2
  7. Auto-increment column address
  8. Transfer data word 3
  ... (continue for burst length)
  N. Transfer data word N

Time: ~100 ns + (N-1) × 2 ns
Example (N=32): 100 + 31 × 2 = 162 ns for 256 bytes
Bandwidth: 1,580 MB/s (20× faster!)
```

---

### **Key Characteristics:**

**Burst Length:**
- Typical burst lengths: 4, 8, 16, 32, 64 words
- Modern GPUs: 32-byte or 128-byte transactions
- CPU: 64-byte cache lines

**Auto-Increment:**
- After first CAS, column address auto-increments
- No need to send address for each word
- Hardware handles sequential access

**Sequential Access:**
- Burst transfers consecutive memory locations
- Address[0], Address[1], Address[2], ..., Address[N-1]
- All within same DRAM row

---

### **Why DRAM Burst is Efficient:**

### **1. Amortize Overhead:**

```
Without Burst (read 32 words):
  32 × (RAS + CAS + precharge) = 32 × 100ns = 3,200 ns

With Burst (read 32 words):
  1 × (RAS + CAS) + 31 × (column cycle) = 100 + 62 = 162 ns

Speedup: 3,200 / 162 = 20× faster!
```

**Overhead paid once:**
- Row activation (RAS): Expensive (~50 ns)
- Column access (CAS): Moderate (~20 ns)
- Data transfer: Fast (~2 ns per word)

**Burst mode:**
- Pay RAS + CAS once
- Transfer many words quickly

---

### **2. Exploit Spatial Locality:**

**Programs access consecutive memory:**
```c
// Sequential array access
for (int i = 0; i < 1000; i++)
    sum += array[i];  // Access array[0], array[1], array[2], ...
```

**DRAM burst matches this pattern:**
```
Request array[0]:
  → DRAM bursts 32 words: array[0..31]
  → Cache line filled
  → Next 31 accesses: cache hits (no DRAM access!)
```

---

### **3. Higher Bandwidth:**

**Single Access:**
```
Transfer 8 bytes in 100 ns
Bandwidth: 8 bytes / 100 ns = 80 MB/s
```

**Burst Access (32 words):**
```
Transfer 256 bytes in 162 ns
Bandwidth: 256 bytes / 162 ns = 1,580 MB/s
→ 20× improvement!
```

---

### **DRAM Burst in Practice:**

### **CPU Cache Lines:**
```
Cache line = 64 bytes
Memory request triggers:
  DRAM burst of 64 bytes
  Fills entire cache line in one transaction
```

### **GPU Memory Coalescing:**
```
Warp requests 32 × 4-byte words = 128 bytes
If coalesced (consecutive addresses):
  → DRAM burst: transfers 128 bytes in one transaction
If not coalesced:
  → Multiple separate DRAM accesses (slow!)
```

---

### **Example:**

**Without Burst:**
```
Read 4 integers (16 bytes):
  Access 1: Read address 0x1000 → 100 ns
  Access 2: Read address 0x1004 → 100 ns
  Access 3: Read address 0x1008 → 100 ns
  Access 4: Read address 0x100C → 100 ns
Total: 400 ns
```

**With Burst:**
```
Read 4 integers (16 bytes):
  Burst access starting at 0x1000 (length=4)
  → Send address once
  → Transfer 0x1000, 0x1004, 0x1008, 0x100C
Total: ~106 ns (100 + 3 × 2)
Speedup: 4× faster!
```

---

### **DRAM Burst Parameters:**

**DDR SDRAM:**
```
Burst Length: 4, 8 (configurable)
Burst Type: Sequential or Interleaved
Data Rate: Double (both clock edges)
```

**GDDR5/HBM (GPU Memory):**
```
Burst Length: 32 bytes (typical)
Optimized for graphics access patterns
Very high bandwidth (>800 GB/s)
```

---

### **Relation to Memory Coalescing:**

**GPU Memory Coalescing uses DRAM burst:**
```
Warp of 32 threads access:
  Thread 0: Address 0x1000
  Thread 1: Address 0x1004
  Thread 2: Address 0x1008
  ...
  Thread 31: Address 0x107C

Consecutive addresses → Coalesced!
  → Hardware generates single DRAM burst
  → Transfers all 128 bytes in one transaction
  → Efficient!
```

**Uncoalesced Access:**
```
Warp of 32 threads access:
  Thread 0: Address 0x1000
  Thread 1: Address 0x2000  (not consecutive!)
  Thread 2: Address 0x3000
  ...

Non-consecutive addresses → Not coalesced!
  → 32 separate DRAM transactions
  → Cannot use burst mode
  → Slow!
```

---

### **Benefits:**

1. **Higher Throughput:** 10-20× bandwidth improvement
2. **Lower Latency:** Amortize overhead over multiple words
3. **Efficiency:** One address lookup, multiple data transfers
4. **Matches Locality:** Exploits spatial locality in programs

---

### **Summary:**

**DRAM Burst:**
- Transfer **multiple consecutive words** in **one transaction**
- **Address sent once**, column auto-increments
- **Amortizes overhead** (RAS/CAS) over many transfers
- **10-20× faster** than individual accesses
- **Exploits spatial locality** (sequential access patterns)
- **Used by cache lines and GPU coalescing**

**Key Insight:** DRAM burst mode is fundamental to achieving high memory bandwidth by exploiting sequential access patterns.

---

### B. Provide an example of CUDA un-coalesced accesses. (1 point)

**Answer:**

**Un-coalesced access** occurs when threads in a warp access **non-consecutive** or **misaligned** memory addresses, resulting in **multiple memory transactions** instead of one.

---

### **Example 1: Strided Access**

```cuda
__global__ void uncoalescedStride(float* data, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Access with stride of 32 (every 32nd element)
    int address = idx * 32;
    float value = data[address];  // UN-COALESCED! ✗
}
```

**Memory Access Pattern:**
```
Warp 0 (32 threads):
  Thread 0:  data[0]     (address 0x0000)
  Thread 1:  data[32]    (address 0x0080)
  Thread 2:  data[64]    (address 0x0100)
  Thread 3:  data[96]    (address 0x0180)
  ...
  Thread 31: data[992]   (address 0x0F80)

Non-consecutive addresses! (gap of 32 elements)
→ Multiple memory transactions (up to 32!)
→ Un-coalesced ✗
```

**Memory Transactions:**
```
32 threads access 32 different cache lines
Each cache line: 128 bytes (32 floats)
Result: 32 separate memory transactions
Efficiency: ~3% (1/32)
```

---

### **Example 2: Reverse Order Access**

```cuda
__global__ void uncoalescedReverse(float* data, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Access in reverse order
    int reverse_idx = n - 1 - idx;
    float value = data[reverse_idx];  // UN-COALESCED! ✗
}
```

**Memory Access Pattern:**
```
Warp 0 (32 threads), n=1000:
  Thread 0:  data[999]   (address 0x0F9C)
  Thread 1:  data[998]   (address 0x0F98)
  Thread 2:  data[997]   (address 0x0F94)
  ...
  Thread 31: data[968]   (address 0x0F00)

Backward consecutive (not forward)
May span multiple cache lines
→ Multiple transactions
→ Un-coalesced ✗
```

---

### **Example 3: Random/Scattered Access**

```cuda
__global__ void uncoalescedRandom(float* data, int* indices, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Access using random indices
    int random_idx = indices[idx];
    float value = data[random_idx];  // UN-COALESCED! ✗
}
```

**Memory Access Pattern:**
```
Warp 0 (32 threads):
  Thread 0:  data[742]   (random location)
  Thread 1:  data[23]    (random location)
  Thread 2:  data[891]   (random location)
  ...
  Thread 31: data[156]   (random location)

Completely scattered addresses!
→ Up to 32 separate transactions
→ Worst case un-coalesced ✗
```

---

### **Example 4: Structure of Arrays (Bad Layout)**

```cuda
struct Particle {
    float x, y, z;
    float vx, vy, vz;
};

__global__ void uncoalescedStruct(Particle* particles, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Access x coordinate
    float x = particles[idx].x;  // UN-COALESCED! ✗
}
```

**Memory Layout:**
```
Memory: [p0.x, p0.y, p0.z, p0.vx, p0.vy, p0.vz, p1.x, p1.y, ...]
         ↑                                      ↑
       Thread 0                              Thread 1

Thread 0 accesses: offset 0
Thread 1 accesses: offset 24 (6 floats = 24 bytes later)
Thread 2 accesses: offset 48

Stride of 24 bytes → non-consecutive
→ Un-coalesced ✗
```

---

### **Example 5: Matrix Transpose (Column Access)**

```cuda
__global__ void uncoalescedTranspose(float* matrix, int width, int height) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    // Access column (non-coalesced for column-major)
    float value = matrix[col * height + row];  // UN-COALESCED! ✗
}
```

**Memory Access Pattern (Row-major Storage):**
```
Matrix stored row-major: [row0, row1, row2, ...]

Threads in warp access same column:
  Thread 0: matrix[col, 0]     (address: col * height + 0)
  Thread 1: matrix[col, 1]     (address: col * height + 1)
  ...
  Thread 31: matrix[col, 31]   (address: col * height + 31)

All threads access different rows (far apart in memory)
Stride = height elements
→ Un-coalesced ✗
```

---

### **Example 6: Unaligned Access**

```cuda
__global__ void uncoalescedUnaligned(float* data, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Start at offset 1 (misaligned!)
    float value = data[idx + 1];  // UN-COALESCED! ✗
}
```

**Memory Access Pattern:**
```
Warp 0:
  Thread 0:  data[1]    (address 0x0004) ← Misaligned!
  Thread 1:  data[2]    (address 0x0008)
  Thread 2:  data[3]    (address 0x000C)
  ...
  Thread 31: data[32]   (address 0x0080)

Consecutive, but misaligned to cache line boundary
Cache line 0: contains data[0-31]
Cache line 1: contains data[32-63]

Warp spans two cache lines → 2 transactions instead of 1
→ Partially un-coalesced ✗
```

---

### **Performance Impact:**

**Coalesced (Good):**
```cuda
int idx = blockIdx.x * blockDim.x + threadIdx.x;
float value = data[idx];  // Consecutive access

Memory transactions: 1 (for entire warp)
Bandwidth efficiency: 100%
Time: ~400 cycles
```

**Un-Coalesced (Bad - Stride 32):**
```cuda
int idx = blockIdx.x * blockDim.x + threadIdx.x;
float value = data[idx * 32];  // Strided access

Memory transactions: 32 (one per thread)
Bandwidth efficiency: 3%
Time: ~12,800 cycles (32× slower!)
```

---

### **How to Fix:**

**Bad (Un-coalesced):**
```cuda
// Stride access
int idx = threadIdx.x * 32;
value = data[idx];
```

**Good (Coalesced):**
```cuda
// Consecutive access
int idx = threadIdx.x;
value = data[idx];
```

**Bad (Structure of Arrays):**
```cuda
struct Particle { float x, y, z; };
Particle particles[N];
// Access: particles[idx].x → strided
```

**Good (Array of Structures):**
```cuda
float x[N], y[N], z[N];
// Access: x[idx] → consecutive, coalesced!
```

---

### **Detection:**

Use **NVIDIA profilers** to detect un-coalesced access:

```bash
nvprof --metrics gld_efficiency ./program

Output:
  Global Load Efficiency: 25%  ← Low efficiency = un-coalesced!

High efficiency (>80%): Coalesced ✓
Low efficiency (<50%): Un-coalesced ✗
```

---

### **Summary:**

**Un-coalesced access examples:**
1. **Strided access:** `data[idx * stride]`
2. **Reverse access:** `data[n - idx]`
3. **Random access:** `data[random_indices[idx]]`
4. **Structure stride:** `particles[idx].x` (bad layout)
5. **Column-major access:** Matrix transpose
6. **Misaligned access:** `data[idx + offset]`

**Result:** Multiple memory transactions, low bandwidth efficiency, poor performance (up to 32× slower!)

**Fix:** Reorganize data layout and access patterns to ensure consecutive memory access.

---

### C. Please explain what a Total Store Ordering consistency model does. (1 point)

**Answer:**

**Total Store Ordering (TSO)** is a memory consistency model that enforces **program order for most memory operations** but **relaxes the Write→Read ordering** by allowing reads to bypass pending writes to different addresses through the use of **write buffers**.

---

### **TSO Memory Ordering Rules:**

TSO maintains **three out of four** orderings:

| Ordering | TSO | Behavior |
|----------|-----|----------|
| **W→R** (Write before Read) | ✗ **Relaxed** | Read can bypass write to different address |
| **R→R** (Read before Read) | ✓ **Enforced** | Reads execute in program order |
| **R→W** (Read before Write) | ✓ **Enforced** | Write after read executes in order |
| **W→W** (Write before Write) | ✓ **Enforced** | Writes execute in program order |

---

### **Key Feature: Write Buffer**

**Mechanism:**
```
Processor → Write Buffer → Memory

Writes go to write buffer (fast)
Reads can bypass write buffer (if different address)
Writes drain to memory in order (eventually)
```

**Example:**
```c
// Thread 1:
X = 1;      // W1: Goes to write buffer
r1 = Y;     // R1: Can bypass W1 if Y ≠ X

// TSO allows: R1 executed before W1 visible to other processors
// W→R relaxed!
```

---

### **What TSO Enforces:**

### **1. R→R (Read before Read):**
```c
r1 = X;     // R1
r2 = Y;     // R2

TSO ensures: R1 completes before R2 executes
→ R2 cannot see memory state older than R1
```

### **2. R→W (Read before Write):**
```c
r1 = X;     // R1
Y = 1;      // W1

TSO ensures: W1 only executes after R1 completes
→ Cannot write before read finishes
```

### **3. W→W (Write before Write):**
```c
X = 1;      // W1
Y = 2;      // W2

TSO ensures: W1 visible to all processors before W2
→ No other processor can see Y=2 while X=0
→ Write order preserved
```

---

### **What TSO Relaxes:**

### **W→R (Write before Read) - RELAXED:**

```c
X = 1;      // W1: Sent to write buffer
r1 = Y;     // R1: Can read Y before X=1 visible globally

Hardware:
  W1 → Write buffer (not yet in memory)
  R1 → Bypass write buffer, read Y from memory
  R1 may complete before W1 drains to memory!
```

**Why Allow This?**
- Write buffers hide write latency
- Processor doesn't stall on writes
- Reads can continue while writes pending
- Performance improvement!

---

### **Example: TSO Violation**

**Classic Problem:**

```c
// Initially: X=0, Y=0

// Thread 1:
X = 1;      // W1
r1 = Y;     // R1

// Thread 2:
Y = 1;      // W2
r2 = X;     // R2

// Can we get r1=0 and r2=0?
```

**Under Sequential Consistency:** **NO**
```
All operations in some global order
If r1=0, then R1 before W2
If r2=0, then R2 before W1
→ Cycle: R1 < W2 < R2 < W1 < R1 (impossible!)
```

**Under TSO:** **YES!**
```
Timeline:
  T1: W1 (X=1) → write buffer (not visible yet)
  T1: R1 reads Y=0 (bypasses write buffer) ← r1=0
  T2: W2 (Y=1) → write buffer (not visible yet)
  T2: R2 reads X=0 (bypasses write buffer) ← r2=0

Later:
  W1 and W2 drain to memory

Result: r1=0, r2=0 (allowed under TSO!)
→ W→R relaxation allows this!
```

---

### **TSO in Practice:**

### **Hardware: Intel x86/x64**

Intel processors implement TSO (with minor extensions):
```
x86 Memory Model ≈ TSO
- Write buffers for performance
- Reads can bypass writes (different addresses)
- Maintains W→W, R→R, R→W ordering
```

**MFENCE instruction:**
```c
X = 1;
__asm__ __volatile__("mfence" ::: "memory");  // Memory fence
r1 = Y;

MFENCE: Drain write buffer before continuing
→ Enforces W→R ordering when needed
```

---

### **Programming with TSO:**

**Safe Code (No Fence Needed):**
```c
// Producer-Consumer with flag
int data = 0;
int ready = 0;

// Producer:
data = 42;      // W1
ready = 1;      // W2

// TSO ensures W→W: data=42 visible before ready=1 ✓

// Consumer:
while (ready == 0);  // R1
int value = data;    // R2

// TSO ensures R→R: ready=1 seen before data read ✓
```

**Unsafe Code (Needs Fence):**
```c
// Initially: X=0, Y=0

// Thread 1:
X = 1;          // W1
if (Y == 0)     // R1 (can bypass W1 under TSO!)
    critical_section();

// Thread 2:
Y = 1;          // W2
if (X == 0)     // R2 (can bypass W2 under TSO!)
    critical_section();

// Problem: Both threads may enter critical section!
// TSO allows R1 and R2 to bypass W1 and W2
```

**Fix with Fence:**
```c
// Thread 1:
X = 1;
memory_fence();  // Drain write buffer
if (Y == 0)
    critical_section();

// Thread 2:
Y = 1;
memory_fence();  // Drain write buffer
if (X == 0)
    critical_section();

// Now safe: Fence enforces W→R ordering
```

---

### **TSO vs Other Models:**

| Model | W→R | R→R | R→W | W→W | Hardware |
|-------|-----|-----|-----|-----|----------|
| **SC (Sequential Consistency)** | ✓ | ✓ | ✓ | ✓ | None (theory) |
| **TSO (Total Store Order)** | ✗ | ✓ | ✓ | ✓ | x86, SPARC |
| **PSO (Partial Store Order)** | ✗ | ✓ | ✓ | ✗ | SPARC |
| **WO (Weak Ordering)** | ✗ | ✗ | ✗ | ✗ | ARM, POWER |

---

### **Benefits of TSO:**

**1. Performance:**
- Write buffers hide write latency
- Processor doesn't stall on writes
- Common case (W→R with different addresses) is fast

**2. Programmer-Friendly:**
- Closer to sequential consistency than PSO/WO
- Most intuitive code works correctly
- Only specific patterns need fences

**3. Hardware Simplicity:**
- Relatively simple to implement
- Just need write buffers
- Used by x86 (most common architecture)

---

### **Summary:**

**Total Store Ordering (TSO):**

**Enforces:**
- ✓ **R→R** (reads in order)
- ✓ **R→W** (read before write)
- ✓ **W→W** (writes in order)

**Relaxes:**
- ✗ **W→R** (read can bypass write via write buffer)

**Mechanism:**
- **Write buffer** allows reads to bypass pending writes
- Improves performance (hide write latency)
- Needs **memory fences** for W→R when required

**Used By:**
- Intel x86/x64
- SPARC TSO mode

**Key Insight:** TSO is a good balance between performance (write buffers) and programmability (enforces most orderings).

---

### D. What is meant by heterogeneous processing? (1 point)

**Answer:**

**Heterogeneous processing** (or **heterogeneous computing**) is the use of **multiple types of processors** or **specialized processing units** with **different architectures** working together in a single system to execute a workload, with each type optimized for specific tasks.

---

### **Key Characteristics:**

### **1. Multiple Processor Types:**

A heterogeneous system combines:
- **General-purpose CPUs** (sequential control, branching)
- **GPUs** (massive data parallelism)
- **Fixed-function units** (specialized tasks: video encode, crypto)
- **DSPs** (digital signal processing)
- **FPGAs** (reconfigurable logic)
- **ASICs** (application-specific: neural networks, encryption)

**Example System:**
```
Smartphone SoC (System-on-Chip):
├─ CPU: 4× ARM Cortex-A cores (general-purpose)
├─ GPU: Mali-G78 (graphics, compute)
├─ DSP: Hexagon (audio/image processing)
├─ NPU: Neural Processing Unit (AI inference)
├─ ISP: Image Signal Processor (camera)
├─ Video encode/decode units
└─ Modem (communication)
```

---

### **2. Task Specialization:**

**Principle:** Use the **most efficient processor** for each task type

| Task Type | Processor | Why |
|-----------|-----------|-----|
| **Control flow** | CPU | Complex branching, sequential logic |
| **Data parallelism** | GPU | Thousands of parallel operations |
| **Video encoding** | Fixed-function | Ultra-efficient, specific algorithm |
| **Neural networks** | TPU/NPU | Matrix operations, low precision |
| **Signal processing** | DSP | FFT, filtering, low power |

**Example Workflow:**
```
Video Processing Application:
1. CPU: Read file, parse headers, coordinate tasks
2. GPU: Decode video frames (parallel)
3. DSP: Apply audio filters
4. Fixed-function: Encode to H.265 (specialized)
5. CPU: Write output file

Each processor does what it's best at!
```

---

### **3. Energy Efficiency:**

**Core Motivation:** Different architectures have drastically different **performance/watt**

**Energy Comparison:**
```
Task: 1 billion multiply-add operations

CPU (general-purpose):
  - Performance: 100 GFLOPS
  - Power: 100 W
  - Energy: 1 second × 100 W = 100 J

GPU (data-parallel):
  - Performance: 1000 GFLOPS
  - Power: 250 W
  - Energy: 0.1 second × 250 W = 25 J
  - → 4× more energy efficient!

ASIC (specialized):
  - Performance: 100 GFLOPS
  - Power: 1 W
  - Energy: 1 second × 1 W = 1 J
  - → 100× more energy efficient!
```

**Energy per Operation:**
```
CPU:  1000 pJ/op
GPU:  250 pJ/op  (4× better)
ASIC: 10 pJ/op   (100× better)
```

---

### **4. Heterogeneous Memory:**

**Different memory types:**
- CPU: DDR4/DDR5 (high capacity, moderate speed)
- GPU: GDDR6/HBM2 (high bandwidth, lower capacity)
- Accelerators: On-chip SRAM (very fast, very small)

**Data Movement Challenge:**
```
Transfer data between:
  CPU memory ↔ GPU memory (PCIe bottleneck)
  CPU memory ↔ Accelerator memory

Data movement costs more energy than computation!
→ Minimize transfers, keep data local
```

---

### **Examples of Heterogeneous Systems:**

### **1. Desktop/Laptop:**
```
Intel Core i7 + NVIDIA RTX GPU:
- CPU: 8 cores (sequential tasks, OS, control)
- GPU: 10,000+ CUDA cores (parallel compute, graphics)
- Tasks distributed via CUDA/OpenCL
```

### **2. Smartphone:**
```
Apple A15 Bionic:
- CPU: 6 cores (2 performance + 4 efficiency)
- GPU: 5-core (graphics, compute)
- Neural Engine: 16-core (ML inference)
- ISP: Camera processing
- Video encode/decode units
```

### **3. Supercomputer:**
```
Summit (Oak Ridge National Lab):
- CPU: IBM POWER9 (control, complex logic)
- GPU: NVIDIA V100 (data-parallel science apps)
- 27,000+ GPUs for massive parallelism
```

### **4. Data Center:**
```
Google TPU Pods:
- CPU: Handles requests, coordination
- TPU: Matrix operations (neural networks)
- Specialized for AI inference/training
```

---

### **Programming Heterogeneous Systems:**

**Challenges:**
1. **Multiple programming models:**
   - CPU: C++, pthreads
   - GPU: CUDA, OpenCL
   - FPGA: HDL (Verilog/VHDL)

2. **Data movement:**
   - Explicit copies between memories
   - Synchronization between processors

3. **Load balancing:**
   - Which tasks go where?
   - Dynamic vs static assignment

**Example Code:**
```cpp
// Heterogeneous application
void process() {
    // CPU: Control and coordination
    std::vector<float> data = loadData();

    // GPU: Parallel processing
    float* d_data;
    cudaMalloc(&d_data, size);
    cudaMemcpy(d_data, data.data(), size, cudaMemcpyHostToDevice);

    kernel<<<blocks, threads>>>(d_data, size);  // GPU work

    cudaMemcpy(result.data(), d_data, size, cudaMemcpyDeviceToHost);

    // CPU: Final processing
    postProcess(result);
}
```

---

### **Benefits:**

**1. Performance:**
- 10-100× speedup for specialized tasks
- Massive parallelism (GPUs)
- Each processor type optimized

**2. Energy Efficiency:**
- 10-1000× better energy efficiency
- Critical for mobile devices, data centers
- Specialized units use less power

**3. Flexibility:**
- Different workloads use different processors
- Adapt to application needs
- Scale performance with specialized hardware

---

### **Trade-offs:**

**Advantages:**
- ✓ Higher performance (specialized units)
- ✓ Better energy efficiency (task-specific processors)
- ✓ Scalability (add specialized units)

**Disadvantages:**
- ✗ Complex programming (multiple models)
- ✗ Data movement overhead (CPU ↔ GPU transfers)
- ✗ Load balancing challenges
- ✗ Higher hardware cost and complexity

---

### **Real-World Impact:**

**Mobile Devices:**
```
Battery life improvement:
- Heterogeneous SoC vs. all-CPU:
  → 5-10× longer battery life
  → Same performance
```

**Machine Learning:**
```
Google TPU for neural networks:
- 15-30× faster than GPU
- 30-80× more power efficient
- Critical for large-scale AI
```

**Graphics:**
```
GPU vs. CPU for rendering:
- 100× faster
- Enables real-time ray tracing
```

---

### **Summary:**

**Heterogeneous Processing:**
- **Multiple processor types** (CPU, GPU, DSP, ASIC, FPGA)
- **Task specialization** (each processor handles what it's best at)
- **Energy efficiency** (10-1000× improvement)
- **Performance** (10-100× speedup)
- **Examples:** Smartphones, supercomputers, data centers

**Core Idea:** "Use the right tool for the job"
- CPU: Control, branching
- GPU: Data parallelism
- Specialized units: Ultra-efficient for specific tasks

**Key Challenge:** Programming complexity and data movement overhead

**Key Benefit:** Dramatic improvements in performance and energy efficiency

---

# End of Exam Answers

All answers are based on course material and designed for exam success! ✅
