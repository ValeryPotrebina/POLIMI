# Parallel Computing - Exam Questions & Answers (Without OpenMP)
## Based on parallel-computing.pdf course material

---

## 📘 EXERCISE 1: PRAM Models (4 points)

### A. Describe the main characteristics of a PRAM model. (2 points)

**Answer:**

The **PRAM (Parallel Random Access Machine)** is a shared-memory abstract machine with the following main characteristics:

1. **Unbounded Collection of RAM Processors**: P₀, P₁, P₂, ... that work in parallel
2. **Shared Memory**: Unbounded collection of shared memory cells accessible to all processors
3. **Unit Time Access**: All processors can access all memory cells in unit time
4. **Communication via Shared Memory**: All inter-processor communication happens through shared memory
5. **No Tape**: Processors don't have tape storage
6. **Unbounded Registers**: Each processor has unbounded local registers
7. **Five-Phase Computation**:
   - Read from input cells X⁽¹⁾, X⁽²⁾, ...
   - Read from shared memory A⁽¹⁾, A⁽²⁾, ...
   - Perform internal computation
   - May write to output cells Y⁽¹⁾, Y⁽²⁾, ...
   - May write to shared memory A⁽¹⁾, A⁽²⁾, ...

**Key Strengths:**
- **Natural**: Operations per cycle ≤ P processors
- **Strong**: Any processor can access any memory in unit time
- **Simple**: Abstracts away communication/synchronization overhead
- **Benchmark**: If no efficient PRAM solution exists, no efficient parallel solution exists

---

### B. List and classify the possible READ and WRITE types in a PRAM model. (1 point)

**Answer:**

PRAM models are classified by their **read/write capabilities**:

**READ Types:**
- **ER (Exclusive Read)**: All processors can read from DISTINCT memory locations simultaneously
- **CR (Concurrent Read)**: All processors can read from ANY memory location simultaneously (including the same location)

**WRITE Types:**
- **EW (Exclusive Write)**: All processors can write to DISTINCT memory locations simultaneously
- **CW (Concurrent Write)**: All processors can write to ANY memory location simultaneously

**Four Main PRAM Models:**
1. **EREW** (Exclusive Read, Exclusive Write) - Most restrictive
2. **CREW** (Concurrent Read, Exclusive Write)
3. **ERCW** (Exclusive Read, Concurrent Write)
4. **CRCW** (Concurrent Read, Concurrent Write) - Most powerful

**CW Conflict Resolution Strategies:**
- **Priority CW**: Highest priority processor wins
- **Common CW**: Write allowed only if all values are equal
- **Arbitrary/Random CW**: Random processor wins

**Realistic and Useful:**
- EREW and CREW are most realistic (hardware-implementable)
- CRCW is useful for algorithm design but requires conflict resolution
- Common CRCW is useful for Boolean operations

---

### C. Explain where concurrent reads occur in a prefix sum computation on a PRAM model. (1 point)

**Answer:**

In a **prefix sum (SPMD sum)** algorithm on PRAM:

**Concurrent Reads occur during:**

1. **Initial Phase**: When all processors read from the input array A simultaneously
   ```
   GLOBAL READ (A ← A(I))  // All processors read their input
   ```

2. **Accumulation Phase**: During each iteration H of the summation tree, when multiple processors need to read from the SAME shared memory locations in array B
   ```
   For H = 1 to K:
     If i ≤ n/2^h:
       GLOBAL READ (X ← B(2i-1))  // Multiple processors may read same B locations
       GLOBAL READ (Y ← B(2i))    // Concurrent reads happen here
   ```

**Example with 8 elements:**
- Iteration 1: Pairs (1,2), (3,4), (5,6), (7,8) are read concurrently
- Iteration 2: Elements B(1), B(2), B(3), B(4) may be read by multiple processors
- Each level reduces active processors but increases potential for concurrent reads of partial sums

**Why Concurrent Reads?**
- Multiple processors in the reduction tree may need the same partial result
- This is why **CREW or CRCW** models are needed for efficient prefix sum
- EREW would require serialization, reducing parallelism

---

### D. Give definitions of speedup and efficiency in a PRAM model. Explain the relationship between them. (1 point)

**Answer:**

**Speedup Definition:**
```
SUₚ(n) = T*(n) / Tₚ(n)
```
Where:
- T*(n) = Time on 1 processor using BEST sequential algorithm
- Tₚ(n) = Time on P processors using parallel algorithm
- Measures: "How much faster is the parallel version?"

**Efficiency Definition:**
```
Eₚ(n) = T₁(n) / (P × Tₚ(n))
```
Where:
- T₁(n) = Time on 1 processor using the parallel algorithm
- Measures: "How well are processors utilized?"
- Range: 0 ≤ Eₚ ≤ 1 (1 = perfect utilization)

**Relationship:**
When T*(n) ≈ T₁(n) (parallel algorithm is also good sequentially):
```
Eₚ ≈ SUₚ / P
```

**Interpretation:**
- **Eₚ = 1**: Perfect efficiency (linear speedup)
- **Eₚ < 1**: Processors are underutilized (due to overhead, synchronization, or idle time)
- **SUₚ ≤ P**: Speedup bounded by number of processors
- **High efficiency** → **Good speedup relative to processor count**

---

## 📘 EXERCISE 2: Processor Architecture & Execution Models (4 points)

### A. Compare and contrast Superscalar, SIMD, and Multi-core processor architectures. (2 points)

**Answer:**

| Aspect | **Superscalar** | **SIMD** | **Multi-core** |
|--------|----------------|----------|----------------|
| **Parallelism Type** | Instruction-Level (ILP) | Data-Level | Thread-Level (TLP) |
| **Execution Units** | Multiple ALUs | Many identical ALUs | Independent cores |
| **Instruction Stream** | Single (finds independent instructions) | Single (broadcast to all ALUs) | Multiple (different programs) |
| **Data** | Single | Multiple (vectors) | Multiple (independent) |
| **Control** | Complex (dynamic scheduling) | Simple (one decoder) | Independent per core |
| **Discovery** | Hardware (automatic) | Compiler/Hardware | Programmer (explicit) |
| **Best For** | General-purpose code | Data-parallel operations | Independent tasks |

**Superscalar:**
- Decodes and executes up to N instructions per clock
- Hardware finds independent instructions automatically
- Example: Modern Intel/AMD CPUs (4-6 wide)

**SIMD:**
- One instruction operates on multiple data elements
- Amortizes control cost over many ALUs
- Example: AVX-512 (16 floats at once)
- Types:
  - **Explicit SIMD**: Compiler generates vector instructions
  - **Implicit SIMD**: Hardware executes scalar instructions in SIMD fashion

**Multi-core:**
- Completely independent CPUs on one chip
- Each core can run different programs
- Example: Intel Core i7 (8+ cores)
- Requires explicit threading (pthreads, etc.)

**Key Difference:** Superscalar finds parallelism in sequential code, SIMD exploits data parallelism, Multi-core enables task parallelism.

---

### B. How does multi-threading reduce memory access stalls? Provide an example. (1 point)

**Answer:**

**Mechanism:**
Multi-threading **hides memory latency** by interleaving execution of multiple threads on the same core. When one thread stalls on memory access, the processor switches to another thread to keep ALUs busy.

**Example:**

Assume:
- 3 arithmetic instructions + 1 memory load (12-cycle latency)
- Each arithmetic instruction = 1 clock cycle

**Without Multi-threading:**
```
Cycles: [ALU ALU ALU | STALL STALL ... STALL]
         3 work      + 12 stall = 15 cycles
CPU Utilization: 3/15 = 20%
```

**With 2 Threads:**
```
Thread 0: [ALU ALU ALU | STALL STALL ...]
Thread 1:     [ALU ALU ALU | STALL STALL ...]
Cycles: 6 work + 12 stall = 18 cycles total
CPU Utilization: 6/18 = 33%
```

**With 5 Threads:**
```
Threads interleaved to fill all 15 cycles with work
CPU Utilization: 15/15 = 100%
```

**Formula for threads needed:**
```
Threads = (Work_cycles + Stall_cycles) / Work_cycles
        = (3 + 12) / 3 = 5 threads
```

**Key Point:** Multi-threading doesn't reduce latency itself, but keeps the processor busy by working on other threads during memory stalls.

---

### C. Explain when an application is classified as "bandwidth-bound" versus "compute-bound". (1 point)

**Answer:**

**Bandwidth-Bound Application:**

An application is **bandwidth-bound** when its performance is limited by the rate at which data can be transferred between memory and processor (memory bandwidth), not by computation speed.

**Characteristics:**
- Many memory accesses per operation
- Low arithmetic intensity (FLOPS/byte ratio)
- Example: 1 FLOP requires 4 bytes → 4B/FLOP
- Processor stalls waiting for data
- **More processors don't help** if memory bandwidth is saturated

**Example Scenario:**
```
GPU with:
- Peak: 1,600 GFLOPS
- Memory bandwidth: 600 GB/s
- Application: 4 bytes/FLOP arithmetic intensity

Required bandwidth: 1,600 GFLOPS × 4 B/FLOP = 6,400 GB/s
Available bandwidth: 600 GB/s

Actual performance: 600 GB/s / 4 B/FLOP = 150 GFLOPS
Utilization: 150/1,600 = 9.3% of peak!
```

**Compute-Bound Application:**

Performance is limited by **computational throughput** (FLOPS), not memory bandwidth.

**Characteristics:**
- Few memory accesses per operation
- High arithmetic intensity
- Example: Dense matrix multiply (reuses data from cache)
- Processor ALUs are the bottleneck
- **More/faster cores help**

**Solutions for Bandwidth-Bound:**
1. **Increase data reuse** (temporal locality)
2. **Use shared memory/cache** effectively
3. **Reduce memory accesses** (fusion optimizations)
4. **Compress data** before transfer

**Key Metric:**
```
Arithmetic Intensity = Operations / Bytes transferred
- Low (<1): Bandwidth-bound
- High (>10): Compute-bound
```

---

## 📘 EXERCISE 3: Programming Models & CUDA (4 points)

### A. Compare "Shared address space" and "Message passing" programming models. (1 point)

**Answer:**

| Aspect | **Shared Address Space** | **Message Passing** |
|--------|-------------------------|-------------------|
| **Memory Model** | Global shared memory accessible to all threads | Private memory per process |
| **Communication** | Read/write to shared variables | Explicit send/receive messages |
| **Synchronization** | Locks, barriers, atomics | Message ordering, blocking calls |
| **Data Location** | Transparent (anywhere in shared space) | Explicit (must specify source/dest) |
| **Hardware Support** | Requires cache coherence | Only needs network |
| **Scalability** | Limited (cache coherence overhead) | Excellent (no shared state) |
| **Programming** | Easier (implicit communication) | More explicit control |
| **Examples** | Pthreads, OpenMP | MPI |

**Shared Address Space:**

**Advantages:**
- Natural programming model
- Implicit communication through shared variables
- Fine-grained sharing possible

**Disadvantages:**
- Requires expensive cache coherence hardware
- Risk of race conditions
- Limited scalability

**Example:**
```c
int shared_counter = 0;  // All threads see this

#pragma omp parallel
{
    #pragma omp atomic
    shared_counter++;  // Implicit communication
}
```

**Message Passing:**

**Advantages:**
- No shared state = easier to reason about
- Excellent scalability (clusters, supercomputers)
- Data isolation prevents races
- Works on commodity hardware

**Disadvantages:**
- More explicit programming required
- Communication overhead (copying data)
- Synchronization through message order

**Example:**
```c
if (rank == 0) {
    MPI_Send(&data, size, MPI_INT, 1, tag, comm);  // Explicit
} else {
    MPI_Recv(&data, size, MPI_INT, 0, tag, comm, &status);
}
```

**When to Use:**
- **Shared Memory**: Multi-core CPUs, GPUs, tight coupling
- **Message Passing**: Distributed systems, clusters, loose coupling

---

### B. What is a "Warp" in CUDA? Is it part of the CUDA language? What is its relation to the CUDA thread hierarchy? (1 point)

**Answer:**

**Definition:**
A **Warp** is a group of **32 threads** that execute the **same instruction** simultaneously on a GPU Streaming Multiprocessor (SM). It's the fundamental unit of execution in NVIDIA GPUs.

**Not Part of CUDA Language:**
- **No**: Warp is **not** part of the CUDA programming model/language
- It's an **implementation detail** of NVIDIA hardware
- CUDA code doesn't explicitly create or manage warps
- Understanding warps is crucial for optimization, but not required for correctness

**Relation to CUDA Thread Hierarchy:**

```
Grid (all threads executing kernel)
  └─ Blocks (logical grouping by programmer)
      └─ Threads (individual execution contexts)
          └─ [Warps] (hardware implementation: groups of 32)
```

**Example:**
```cuda
// Programmer specifies:
kernel<<<numBlocks, threadsPerBlock>>>();
// threadsPerBlock = 256

// Hardware implementation:
// 256 threads = 8 warps (256/32)
// All threads in a warp execute together
```

**Key Properties:**
1. **Size**: Always 32 threads (on current NVIDIA GPUs)
2. **SIMD Execution**: All threads in warp execute same instruction
3. **Divergence**: If threads take different paths (if/else), warp serializes execution
4. **Scheduling**: SM schedules warps, not individual threads
5. **Context**: Each warp has register context (32 sets of registers)

**Performance Implications:**
- **Warp divergence** = performance loss
- **Memory coalescing** happens per warp
- **Occupancy** measured in warps per SM

**Example of Warp Impact:**
```cuda
// BAD: Warp divergence
if (threadIdx.x % 2 == 0) {
    // 16 threads execute this
} else {
    // 16 threads execute this (serialized!)
}

// GOOD: Entire warp takes same path
if (threadIdx.x < 16) {
    // One warp executes this
} else {
    // Another warp executes this
}
```

---

### C. Describe how tiling/buffering is implemented in CUDA. Provide a simple example. (2 points)

**Answer:**

**Tiling (Blocking) Concept:**

Tiling divides large computations into smaller "tiles" that fit into **shared memory**, reducing slow global memory accesses.

**Benefits:**
1. **Reduced Global Memory Access**: Load data once, reuse many times
2. **Improved Cache Efficiency**: Data stays in fast shared memory
3. **Memory Bandwidth Optimization**: Fewer DRAM transactions

**Implementation Steps:**

1. **Divide work** into tiles (e.g., TILE_WIDTH × TILE_WIDTH)
2. **Load tile** from global memory → shared memory
3. **Synchronize** threads (`__syncthreads()`)
4. **Compute** using shared memory data
5. **Synchronize** again before next tile
6. **Repeat** for all tiles

**Example: Tiled Matrix Multiplication**

**Without Tiling (Naive):**
```cuda
__global__ void matMulNaive(float* M, float* N, float* P, int Width) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;
    for (int k = 0; k < Width; k++) {
        sum += M[row * Width + k] * N[k * Width + col];
        // Global memory access every iteration! Slow!
    }
    P[row * Width + col] = sum;
}
```

**With Tiling:**
```cuda
#define TILE_WIDTH 16

__global__ void matMulTiled(float* M, float* N, float* P, int Width) {
    // Shared memory for tiles (fast!)
    __shared__ float Mds[TILE_WIDTH][TILE_WIDTH];
    __shared__ float Nds[TILE_WIDTH][TILE_WIDTH];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int Row = by * TILE_WIDTH + ty;
    int Col = bx * TILE_WIDTH + tx;

    float Pvalue = 0.0f;

    // Loop over tiles
    int numTiles = Width / TILE_WIDTH;
    for (int t = 0; t < numTiles; t++) {
        // Phase 1: Load tile into shared memory
        Mds[ty][tx] = M[Row * Width + (t * TILE_WIDTH + tx)];
        Nds[ty][tx] = N[(t * TILE_WIDTH + ty) * Width + Col];

        // Phase 2: Synchronize (ensure tile is loaded)
        __syncthreads();

        // Phase 3: Compute using shared memory
        for (int k = 0; k < TILE_WIDTH; k++) {
            Pvalue += Mds[ty][k] * Nds[k][tx];  // Fast shared memory!
        }

        // Phase 4: Synchronize before loading next tile
        __syncthreads();
    }

    // Write result
    P[Row * Width + Col] = Pvalue;
}
```

**Performance Comparison:**

For TILE_WIDTH = 16:
- **Naive**: Width global memory loads per element
- **Tiled**: 2 × (Width / TILE_WIDTH) global memory loads per element
- **Reduction**: Width / (2 × Width/16) = 8x fewer global memory accesses!

**Memory Access Pattern:**
```
Naive:     [Global] → [ALU] → [Global] (every operation)
Tiled:     [Global] → [Shared] → [ALU]×16 → [Shared] → [Global]
           \_____  Reuse data 16 times!  _____/
```

**Key Points:**
- `__shared__` keyword declares shared memory (per-block)
- `__syncthreads()` ensures all threads complete before proceeding
- Tile size limited by shared memory available (typically 48KB per SM)
- Must handle boundary conditions for non-multiple sizes

---

## 📘 EXERCISE 4: Memory Systems & Heterogeneous Computing (4 points)

### A. Explain what "memory coalesced accesses" means in CUDA. (1 point)

**Answer:**

**Definition:**
**Memory coalescing** is when multiple threads in a warp access **consecutive memory locations** in a single memory transaction, combining their requests into one efficient operation.

**How It Works:**

**DRAM Burst:**
- DRAM transfers data in "bursts" (e.g., 32, 64, or 128 bytes at a time)
- Each memory transaction fetches an entire burst sector

**Coalesced Access:**
When threads in a warp access memory addresses that:
1. Are **contiguous** (consecutive)
2. Are **aligned** to burst boundaries
3. Fall within a **single burst sector**

→ Hardware combines them into **one transaction**

**Example:**

**COALESCED (Good):**
```cuda
// Thread i accesses element i
int idx = blockIdx.x * blockDim.x + threadIdx.x;
float value = array[idx];  // ✓ Consecutive access

// Thread 0: array[0]
// Thread 1: array[1]
// Thread 2: array[2]
// ...
// Thread 31: array[31]
// → 1 memory transaction for 32 threads!
```

**UN-COALESCED (Bad):**
```cuda
// Thread i accesses element i*4 (strided)
int idx = (blockIdx.x * blockDim.x + threadIdx.x) * 4;
float value = array[idx];  // ✗ Non-consecutive

// Thread 0: array[0]
// Thread 1: array[4]
// Thread 2: array[8]
// ...
// → Multiple memory transactions
// → Wasted bandwidth (bytes fetched but not used)
```

**Performance Impact:**

| Pattern | Transactions | Efficiency |
|---------|-------------|-----------|
| Coalesced | 1 per warp | 100% |
| Strided (stride=2) | 2 per warp | 50% |
| Random | Up to 32 per warp | <10% |

**Rules for Coalescing:**

1. **Access Pattern:**
   ```
   address[thread] = base + (constant × threadIdx.x)
   ```

2. **Alignment:** Starting address should be aligned to 32/64/128 bytes

3. **Within Burst:** All addresses in same burst sector

**Visual:**
```
Memory:  [  Burst Sector 1  ][  Burst Sector 2  ]
         [0 1 2 ... 30 31 32][33 34 35 ... 63 64]

Coalesced:     Thread 0→31 access [0...31]    ✓ 1 transaction
Un-coalesced:  Threads access [0,2,4,6,...]   ✗ Multiple transactions
```

**Detection:** Use NVIDIA profilers (nvprof, Nsight) to measure:
- Global memory load efficiency
- Global memory store efficiency
- Values < 100% indicate poor coalescing

---

### B. Define "memory consistency" and explain why it matters in parallel systems. (1 point)

**Answer:**

**Definition:**

**Memory Consistency** is a contract that defines **when and in what order** memory operations (reads and writes) from different threads/processors become visible to other threads/processors.

It answers: *"If thread A writes X=1 and thread B writes Y=1, in what order will other threads see these changes?"*

**Two Key Aspects:**

1. **Coherence**: Behavior when a **single address** is accessed by multiple threads
   - What happens when multiple threads read/write the same location?

2. **Consistency**: Order of reads, writes, and synchronizations (RWS) with **different addresses** across threads
   - What is the global ordering of operations on different locations?

**Why It Matters:**

**Problem: Reordering by Hardware/Compiler**
```c
// Thread 1          // Thread 2
X = 1;               while (flag == 0) { }
flag = 1;            print(X);
```

**Without Consistency Guarantees:**
- Compiler/hardware might reorder: `flag = 1` before `X = 1`
- Thread 2 might print garbage (X not set yet)!
- Result: **Undefined behavior, data races**

**Memory Consistency Models:**

| Model | Restrictions | Performance | Complexity |
|-------|-------------|-------------|-----------|
| **Sequential Consistency** | All 4 orderings enforced | Slowest | Simplest |
| **TSO/PC** | W→R relaxed | Medium | Medium |
| **PSO** | W→R, W→W relaxed | Faster | Complex |
| **Weak/Release** | All relaxed | Fastest | Most complex |

**Sequential Consistency (Strongest):**
- Operations appear in **some global sequential order**
- Each processor's operations appear in **program order**
- Easy to reason about, but expensive (no reordering allowed)

**Relaxed Consistency (Weaker):**
- Allows hardware/compiler optimizations (reordering)
- Requires explicit **synchronization** (fences, barriers, atomics)
- Faster but harder to program correctly

**Example: Need for Fences**
```c
// With relaxed consistency
X = 1;
memory_fence();      // ← Ensures X=1 completes before flag=1
flag = 1;
```

**In CUDA:**
```cuda
__shared__ int data;
__shared__ int flag;

// Thread 0
data = 42;
__threadfence_block();  // Memory fence
flag = 1;

// Thread 1
while (!flag) { }
__threadfence_block();  // Memory fence
int value = data;       // Guaranteed to see 42
```

**Key Takeaway:**
Without memory consistency guarantees, parallel programs can have **non-deterministic, incorrect behavior** even without data races in the program logic. Consistency models define the "rules of the game" for parallel memory access.

---

### C. In a sequentially consistent memory system, what are the four operation orderings? Which orderings do relaxed models relax? (1 point)

**Answer:**

**Four Operation Orderings in Sequential Consistency:**

A sequentially consistent memory system maintains ALL four orderings:

1. **W→R (Write before Read)**
   ```
   Write to X must commit before a subsequent Read from Y
   ```

2. **R→R (Read before Read)**
   ```
   Read from X must commit before a subsequent Read from Y
   ```

3. **R→W (Read before Write)**
   ```
   Read from X must commit before a subsequent Write to Y
   ```

4. **W→W (Write before Write)**
   ```
   Write to X must commit before a subsequent Write to Y
   ```

**What Relaxed Models Relax:**

| Model | Relaxed Orderings | Example |
|-------|------------------|---------|
| **Sequential Consistency** | ❌ None (all 4 enforced) | Strictest |
| **TSO (Total Store Order)** | ✓ W→R | Intel x86 |
| **PC (Processor Consistency)** | ✓ W→R | Similar to TSO |
| **PSO (Partial Store Order)** | ✓ W→R, ✓ W→W | SPARC |
| **Weak Ordering (WO)** | ✓ All 4 | ARM, POWER |
| **Release Consistency (RC)** | ✓ All 4 | ARM, POWER |

**Details:**

**TSO/PC (Relax W→R):**
```c
// Allowed reordering:
A = 1;      // Write
r1 = B;     // Read can execute before write completes
```
- Uses **write buffers** to hide write latency
- Reads can bypass pending writes to different addresses

**PSO (Relax W→R, W→W):**
```c
// Allowed reordering:
A = 1;      // Write 1
flag = 1;   // Write 2 can occur before Write 1 completes!
```
- Multiple writes can be reordered
- Useful for write-heavy workloads

**Weak/Release Consistency (Relax All):**
```c
// All reorderings allowed between synchronization points:
A = 1;      // Can be reordered with anything below
B = 2;      // Can be reordered with anything
r1 = C;     // Can be reordered with anything
r2 = D;     // Can be reordered with anything
```
- Requires **explicit synchronization** (acquire/release, fences)
- Maximum performance but complex programming

**Why Relax Orderings?**

1. **Hide Memory Latency**: Allow later operations while waiting for earlier ones
2. **Enable Optimizations**: Compiler and hardware can reorder for efficiency
3. **Improve Throughput**: More operations in flight simultaneously

**Cost:**
- Harder to program correctly
- Need memory fences/barriers for correctness
- Non-deterministic behavior without proper synchronization

**Example Problem with Relaxed Ordering:**
```c
// Thread 1              // Thread 2
A = 1;  ───────┐         while (flag == 0) { }
flag = 1; ─────┼─────>   print(A);
               │
               └─ If W→W relaxed, flag=1 can happen first!
                  Thread 2 might print A=0 (garbage)!
```

**Solution: Memory Fence**
```c
A = 1;
memory_fence();    // Ensures A=1 visible before flag=1
flag = 1;
```

---

### D. Describe the two main ideas behind heterogeneous systems. (1 point)

**Answer:**

**Two Main Ideas:**

### **1. Specialized Processing (Compute Specialization)**

**Idea:** Use the **most energy-efficient processor** for each specific task type, rather than using general-purpose processors for everything.

**Key Principle:**
```
Power = (Operations/second) × (Joules/Operation)
```

**Efficiency Gains:**
- **Throughput-Optimized Architectures** (GPUs):
  - ~10x better performance/watt vs CPU for data-parallel workloads
  - Conditions: Compute-bound + maps to wide data-parallel execution

- **Fixed-Function ASICs** (Application-Specific):
  - ~100-1000x better performance/watt vs CPU
  - Conditions: Compute-bound + specific algorithm

**Examples:**
| Task | Processor Type | Reason |
|------|---------------|--------|
| Control flow | CPU | Complex branching |
| Matrix multiplication | GPU | Data parallelism |
| Video encoding | Fixed-function unit | Repetitive, specific |
| Neural network inference | TPU/ASIC | Specialized operations |
| Signal processing | DSP | Optimized for DSP operations |

**Components in Heterogeneous Systems:**
- **CPU cores**: General-purpose, sequential tasks
- **GPU cores**: Parallel, throughput-oriented tasks
- **FPGAs**: Reconfigurable, flexible acceleration
- **Fixed-function units**: Ultra-efficient for specific tasks (texture mapping, H.264 encode, etc.)
- **DSPs**: Signal processing optimized

### **2. Minimize Data Movement**

**Idea:** **Reduce memory bandwidth** requirements by minimizing the distance and frequency of data transfers, since data movement costs more energy than computation.

**Key Facts:**
```
Energy to read 64 bits:
- From registers:        ~0 pJ (essentially free)
- From on-chip SRAM (1mm): ~26 pJ
- From L1 cache:         ~50 pJ
- From DRAM (LPDDR):     ~1200 pJ (46x more expensive!)
```

**Strategies:**

1. **Data Reuse**:
   - Keep data in local/shared memory
   - Access once, compute many times
   - Example: Tiling in matrix multiplication

2. **Algorithm Restructuring**:
   - Maximize on-chip data reuse
   - Minimize global memory access
   - Example: Cache blocking, fusion

3. **Compression**:
   - Compress before transfer
   - Use fixed-function compression hardware
   - Trade computation for bandwidth

4. **Memory Hierarchy Optimization**:
   - Use fastest memory available
   - Stage data through cache hierarchy
   - Avoid DRAM when possible

**Example Impact:**
```
Matrix Multiply (naive):  Width global memory loads per element
Matrix Multiply (tiled):  2 × (Width/TILE_WIDTH) loads per element
Reduction:                8x fewer memory accesses!
```

**Trade-offs:**
- **More computation** to reduce data movement = **net energy savings**
- Example: Compute derivative instead of storing/loading pre-computed values

**Heterogeneous System Benefits:**

1. **Energy Efficiency**: 10-1000x improvement by matching task to processor
2. **Performance**: Specialized units execute faster
3. **Flexibility**: Multiple processor types handle diverse workloads
4. **Scalability**: Add specialized units as needed

**Example Systems:**
- **Mobile SoCs** (Snapdragon, Apple M-series): CPU + GPU + DSP + ISP + NPU
- **Modern GPUs**: CUDA cores + Tensor cores + RT cores + texture units
- **FPGAs**: Programmable logic + embedded processors + DSP blocks
- **Supercomputers**: CPUs + GPUs + network processors

**Challenges:**
- Programming complexity (multiple programming models)
- Load balancing between different processor types
- Data transfer overhead between components
- Determining optimal resource mix

---

## 📘 EXERCISE 5: Parallel Patterns & Algorithms (4 points)

### A. Describe the Map pattern and explain why it is easily parallelizable. (1 point)

**Answer:**

**Map Pattern Definition:**

The **Map pattern** applies a **unary function** (elemental function) to **each element** of a collection **independently**, producing a new collection of the same size.

```
Map: [a₁, a₂, a₃, ..., aₙ] → [f(a₁), f(a₂), f(a₃), ..., f(aₙ)]
```

**Key Characteristics:**

1. **Independence**: Each output depends ONLY on the corresponding input
   - `output[i] = f(input[i])`
   - No dependencies between iterations

2. **Same Size**: Output collection has same length as input

3. **Side-Effect Free**: Function `f` doesn't modify shared state

4. **Read-Only Input**: Input collection isn't modified

**Why Easily Parallelizable:**

**1. No Data Dependencies:**
```
output[0] = f(input[0])  ← Can execute in parallel
output[1] = f(input[1])  ← Completely independent
output[2] = f(input[2])  ← No communication needed
...
```

**2. No Synchronization Required:**
- No shared state to protect
- No race conditions possible
- No locks/barriers needed during computation

**3. Perfect Load Balance:**
- Each operation does same amount of work
- Easy to distribute evenly across processors

**4. Scalability:**
- With ∞ processors: O(1) time
- With P processors: O(n/P) time (linear speedup)

**5. Flexible Mapping:**
```
Processor 0: elements [0, P, 2P, 3P, ...]        (cyclic)
Processor 1: elements [1, P+1, 2P+1, ...]
OR
Processor 0: elements [0 ... n/P-1]              (block)
Processor 1: elements [n/P ... 2n/P-1]
```

**Examples:**

**Simple Map:**
```c
// Sequential
for (int i = 0; i < n; i++) {
    output[i] = input[i] * 2;  // Double each element
}

// Parallel (CUDA-style)
__global__ void mapKernel(float* input, float* output, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        output[i] = input[i] * 2;  // Each thread independent!
    }
}
```

**N-ary Map (multiple inputs):**
```c
// Vector addition: A + B = C
for (int i = 0; i < n; i++) {
    C[i] = A[i] + B[i];  // Still independent per element
}
```

**Composition:**
```c
// Multiple maps can be fused
output[i] = h(g(f(input[i])))  // Still no dependencies!
```

**Performance:**
- **Work**: W = n (n operations total)
- **Span**: S = 1 (with ∞ processors)
- **Parallelism**: P = n/1 = n (excellent!)

**Optimizations:**
1. **Code Fusion**: Combine multiple maps into one
2. **Vectorization**: SIMD instructions (AVX, SSE)
3. **Cache Fusion**: Process in blocks that fit in cache

**Contrast with Other Patterns:**
- **Reduce**: Has dependencies (needs associative combining)
- **Scan**: Sequential dependencies (prefix sum)
- **Stencil**: Needs neighbor data (not fully independent)

**Key Takeaway:** Map is the "embarrassingly parallel" pattern - requires no communication, no synchronization, perfectly load balanced.

---

### B. Explain the Reduce pattern. What property must the combiner function have? Provide examples of valid and invalid reduce operations. (2 points)

**Answer:**

**Reduce Pattern Definition:**

The **Reduce pattern** combines all elements of a collection into a **single summary value** using a **binary combiner function**.

```
Reduce: [a₁, a₂, a₃, ..., aₙ] → a₁ ⊕ a₂ ⊕ a₃ ⊕ ... ⊕ aₙ
```

**Required Property: ASSOCIATIVITY**

**Critical:** The combiner function **MUST be associative** to be parallelizable.

**Associative Property:**
```
(a ⊕ b) ⊕ c = a ⊕ (b ⊕ c)
```

**Why Required:**
- Parallel reduction combines elements in **arbitrary order**
- Different execution schedules must produce **same result**
- Associativity guarantees correctness regardless of grouping

**Examples of Valid Reduce Operations:**

| Operation | Combiner | Associative? | Identity | Example |
|-----------|----------|--------------|----------|---------|
| **Sum** | `a + b` | ✓ Yes | 0 | `1+2+3+4 = 10` |
| **Product** | `a × b` | ✓ Yes | 1 | `2×3×4 = 24` |
| **Maximum** | `max(a,b)` | ✓ Yes | -∞ | `max(5,2,9,1) = 9` |
| **Minimum** | `min(a,b)` | ✓ Yes | +∞ | `min(5,2,9,1) = 1` |
| **Logical AND** | `a && b` | ✓ Yes | true | All true? |
| **Logical OR** | `a \|\| b` | ✓ Yes | false | Any true? |
| **Bitwise AND** | `a & b` | ✓ Yes | ~0 | Bit operations |
| **Bitwise OR** | `a \| b` | ✓ Yes | 0 | Bit operations |
| **Bitwise XOR** | `a ^ b` | ✓ Yes | 0 | Parity check |
| **String concat** | `a + b` | ✓ Yes | "" | "ab"+"cd" = "abcd" |
| **Set union** | `a ∪ b` | ✓ Yes | ∅ | Combine sets |

**Examples of Invalid Reduce Operations:**

| Operation | Combiner | Associative? | Problem |
|-----------|----------|--------------|---------|
| **Subtraction** | `a - b` | ✗ **NO** | `(5-3)-1 ≠ 5-(3-1)` |
| **Division** | `a / b` | ✗ **NO** | `(8/4)/2 ≠ 8/(4/2)` |
| **Exponentiation** | `a ^ b` | ✗ **NO** | `(2³)² ≠ 2^(3²)` |
| **Matrix multiply** (some) | `A × B` | ✗ **NO** | `(AB)C ≠ A(BC)` for non-square |

**Parallel Reduction Tree:**

**Sequential (left-to-right):**
```
[1, 2, 3, 4, 5, 6, 7, 8]
└──> ((((((1+2)+3)+4)+5)+6)+7)+8 = 36
     7 steps, fully sequential
```

**Parallel (tree reduction):**
```
Level 0: [1, 2, 3, 4, 5, 6, 7, 8]
         ↓  ↓  ↓  ↓  ↓  ↓  ↓  ↓
Level 1: [3    7    11   15   ]  (4 parallel operations)
         ↓    ↓    ↓    ↓
Level 2: [10       26        ]   (2 parallel operations)
         ↓         ↓
Level 3: [36                 ]   (1 operation)

3 steps = log₂(8), parallel speedup!
```

**Performance:**
- **Sequential**: Time = O(n), Work = O(n)
- **Parallel**: Time = O(log n), Work = O(n), Parallelism = O(n/log n)

**Code Example:**

**Sequential:**
```c
int reduce_sum(int* array, int n) {
    int sum = 0;  // Identity element
    for (int i = 0; i < n; i++) {
        sum = sum + array[i];  // Associative combiner
    }
    return sum;
}
```

**Parallel (CUDA-style with shared memory):**
```cuda
__global__ void reduceSum(int* input, int* output, int n) {
    __shared__ int sdata[256];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    // Load into shared memory
    sdata[tid] = (i < n) ? input[i] : 0;
    __syncthreads();

    // Tree reduction in shared memory
    for (int stride = blockDim.x/2; stride > 0; stride >>= 1) {
        if (tid < stride) {
            sdata[tid] += sdata[tid + stride];  // Parallel combine
        }
        __syncthreads();
    }

    // Write block result
    if (tid == 0) output[blockIdx.x] = sdata[0];
}
```

**Limitations:**

**1. Floating-Point Non-Associativity:**
```c
// Mathematical: (a + b) + c = a + (b + c)
// Floating-point: May differ due to rounding!

float a = 1e20, b = -1e20, c = 1.0;
(a + b) + c = 0.0 + 1.0 = 1.0
a + (b + c) = 1e20 + (-1e20 + 1.0) ≈ 1e20 + (-1e20) = 0.0  // Lost c!

// Different parallel schedules → different results!
```

**2. Order Dependence:**
- Parallel reduce may give **slightly different results** each run
- Due to non-deterministic thread scheduling
- Solution: Use compensated summation (Kahan) for critical precision

**Key Takeaway:**
- Reduce is powerful but **requires associativity**
- Non-associative operations must be computed sequentially
- Floating-point arithmetic introduces subtle issues

---

### C. What is the Scan pattern? Explain the difference between inclusive and exclusive scan. (1 point)

**Answer:**

**Scan Pattern Definition:**

The **Scan pattern** (also called **prefix sum** or **prefix computation**) computes **all partial reductions** of an input sequence using a **binary associative operator**.

```
Scan produces: [a₁, a₁⊕a₂, a₁⊕a₂⊕a₃, ..., a₁⊕a₂⊕...⊕aₙ]
```

**Two Types:**

### **1. Inclusive Scan**

**Definition:** Each output includes the **current** input element.

```
output[i] = input[0] ⊕ input[1] ⊕ ... ⊕ input[i]
```

**Example with Addition (+):**
```
Input:     [1,  2,  3,  4,  5]
Output:    [1,  3,  6, 10, 15]
            ↑   ↑   ↑   ↑   ↑
           1  1+2 1+2+3 ... 1+2+3+4+5
```

**Formula:**
```
inclusive_scan[i] = Σ(input[j]) for j=0 to i
```

### **2. Exclusive Scan**

**Definition:** Each output **excludes** the current input element, includes all **previous** elements.

```
output[i] = input[0] ⊕ input[1] ⊕ ... ⊕ input[i-1]
```

**Example with Addition (+):**
```
Input:     [1,  2,  3,  4,  5]
Output:    [0,  1,  3,  6, 10]
            ↑   ↑   ↑   ↑   ↑
       identity 1  1+2 1+2+3 1+2+3+4
```

**Formula:**
```
exclusive_scan[i] = Σ(input[j]) for j=0 to i-1
exclusive_scan[0] = identity (0 for +, 1 for ×)
```

**Comparison Table:**

| Aspect | **Inclusive Scan** | **Exclusive Scan** |
|--------|-------------------|-------------------|
| **Includes Current?** | ✓ Yes | ✗ No |
| **First Element** | input[0] | Identity element |
| **Last Element** | Total sum | Sum without last |
| **Use Case** | Running totals | Array indexing |

**Conversion:**
```c
// Inclusive → Exclusive
exclusive[i] = (i == 0) ? identity : inclusive[i-1]

// Exclusive → Inclusive
inclusive[i] = exclusive[i] ⊕ input[i]
```

**Why Scan is Powerful:**

**1. Array Indexing (Exclusive Scan):**
```c
// Compute starting positions for each block
int counts[4] = {3, 5, 2, 4};     // Block sizes
int positions[4];

exclusive_scan(counts, positions); // [0, 3, 8, 10]

// Now can write to output without conflicts:
// Block 0: writes to output[0..2]
// Block 1: writes to output[3..7]
// Block 2: writes to output[8..9]
// Block 3: writes to output[10..13]
```

**2. Parallel Work Distribution:**
```c
// Find work offsets for each thread
int work_per_thread[P];           // Variable work amounts
int thread_offsets[P];

exclusive_scan(work_per_thread, thread_offsets);
// Each thread knows where its work starts!
```

**3. Pack/Compaction:**
```c
// Remove elements where mask[i] == 0
int mask[8] = {1, 0, 1, 1, 0, 0, 1, 1};

exclusive_scan(mask, positions);  // [0, 1, 1, 2, 3, 3, 3, 4]

// Elements to keep go to positions[i] if mask[i]==1
```

**Parallelization:**

**Challenge:** Scan has **sequential dependencies**
```
output[i] depends on output[i-1] depends on output[i-2] ...
```

**Solution: Tree-based algorithms**

**Up-Sweep (Reduce Phase):**
```
[1, 2, 3, 4, 5, 6, 7, 8]
 └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘
   3     7    11    15        Level 1
   └──┬──┘    └──┬──┘
      10          26           Level 2
      └─────┬─────┘
            36                 Level 3 (total sum)
```

**Down-Sweep (Distribute Phase):**
```
Propagate partial sums back down to compute all prefixes
Time: O(log n) with n processors
```

**Performance:**
- **Sequential**: O(n) time, O(n) work
- **Parallel**: O(log n) time, O(n) work, O(n/log n) parallelism

**Applications:**

1. **Stream Compaction**: Remove unwanted elements
2. **Radix Sort**: Compute bucket positions
3. **Dynamic Work Allocation**: Assign variable work to threads
4. **Sparse Matrix Operations**: Index compressed formats
5. **Graph Algorithms**: Parallel breadth-first search
6. **Tree Operations**: Compute tree traversal orders

**Key Properties:**
- Requires **associative operator** (like reduce)
- More complex to parallelize than map/reduce
- Critical building block for many parallel algorithms

---

## 📘 EXERCISE 6: Advanced Topics (4 points)

### A. Explain Amdahl's Law. Derive the speedup formula and discuss its implications. (2 points)

**Answer:**

**Amdahl's Law Statement:**

> "The overall performance improvement of a system is limited by the fraction of the workload that cannot be improved."

In parallel computing: **The speedup is limited by the sequential portion of the program.**

**Assumptions:**

1. **Fixed workload**: Problem size N is constant
2. **Two parts**:
   - Serial fraction: (1-f) - cannot be parallelized
   - Parallel fraction: f - can be perfectly parallelized

**Derivation:**

Let:
- T₁ = execution time on 1 processor
- P = number of processors
- f = fraction of work that can be parallelized (0 ≤ f ≤ 1)
- (1-f) = serial fraction

**Time on P processors:**
```
Tₚ = T₁·(1-f) + T₁·f/P
     \_______/   \____/
     serial part  parallel part

Tₚ = T₁·[(1-f) + f/P]
```

**Speedup:**
```
Speedup(P,f) = T₁/Tₚ

             = T₁ / [T₁·((1-f) + f/P)]

             = 1 / [(1-f) + f/P]
```

**Final Formula:**
```
╔═══════════════════════════════╗
║  Speedup(P,f) = 1/(1-f + f/P) ║
╚═══════════════════════════════╝
```

**Maximum Speedup (P → ∞):**
```
lim(P→∞) Speedup(P,f) = 1/(1-f)
```

**Implications:**

**1. Serial Bottleneck:**
```
If f = 90% (90% parallelizable):
  Max speedup = 1/(1-0.9) = 10x

Even with infinite processors, can't exceed 10x!
```

**2. Diminishing Returns:**

| Processors | f=50% | f=90% | f=95% | f=99% |
|-----------|-------|-------|-------|-------|
| 1 | 1.0x | 1.0x | 1.0x | 1.0x |
| 2 | 1.3x | 1.8x | 1.9x | 2.0x |
| 4 | 1.6x | 3.1x | 3.5x | 3.9x |
| 8 | 1.8x | 4.7x | 5.9x | 7.5x |
| 16 | 1.9x | 6.4x | 8.8x | 13.9x |
| 100 | 2.0x | 9.2x | 16.8x | 50.5x |
| ∞ | **2.0x** | **10x** | **20x** | **100x** |

**Graph:**
```
Speedup
  20│                                    f=95%
    │                               ....─────
  15│                          ....─
    │                     ....─
  10│          f=90% ─────
    │           ....─────
   5│      ....─
    │ ....─  f=50%
   0└─────────────────────────────
     0   20   40   60   80   100  Processors
```

**3. Pessimistic View:**
- **Small serial portions have huge impact**
- Adding processors beyond a point gives minimal benefit
- Example: 1% serial → max 100x speedup (useless with 10,000 cores!)

**4. Efficiency Drops:**
```
Efficiency = Speedup/P

As P increases, efficiency decreases
Example (f=90%, P=100): E = 9.2/100 = 9.2% (terrible!)
```

**Real-World Example:**

Consider image processing:
```
10% - Load image (serial)
80% - Process pixels (parallel)
10% - Save image (serial)

f = 0.80, (1-f) = 0.20

Max speedup = 1/0.20 = 5x

With 16 cores: Speedup = 1/(0.20 + 0.80/16) = 4.7x
With 64 cores: Speedup = 1/(0.20 + 0.80/64) = 4.9x
Diminishing returns beyond 16 cores!
```

**Criticisms of Amdahl's Law:**

1. **Assumes fixed problem size** (unrealistic for many applications)
2. **Ignores communication overhead** (too optimistic)
3. **Doesn't account for cache effects**
4. **Parallel overhead not included** (locks, synchronization, load imbalance)

**When Amdahl's Law Applies:**
- Analyzing existing programs
- Fixed dataset size
- Understanding theoretical limits
- Deciding if parallelization is worth the effort

**Key Takeaway:**
> "Make the common case fast. If only 10% of your program is parallel, don't expect miracles from 1000 cores."

---

### B. Contrast Amdahl's Law with Gustafson's Law. In what scenarios is each more applicable? (2 points)

**Answer:**

**Fundamental Difference:**

| Aspect | **Amdahl's Law** | **Gustafson's Law** |
|--------|-----------------|-------------------|
| **Fixed Parameter** | Problem size (N) | Execution time (T) |
| **Variable** | Time decreases | Problem size increases |
| **Philosophy** | "How much faster?" | "How much more?" |
| **Assumption** | Same work on more processors | More work in same time |
| **Outlook** | Pessimistic | Optimistic |

---

### **Amdahl's Law (Fixed-Size Model)**

**Formula:**
```
Speedup = 1 / [(1-f) + f/P]
Max = 1/(1-f)
```

**Scenario:**
```
┌─────────────────┐
│  Fixed Work N   │  We have N tasks
│  More processors│  → Faster completion
│  Same result    │  → Same output
└─────────────────┘
```

**Example:**
```
Process 1 megapixel image:
- 1 core:  100 seconds
- 10 cores: 15 seconds (if 90% parallelizable)
- 100 cores: 10.9 seconds (diminishing returns!)
```

**Characteristics:**
- Speedup **saturates** (hits ceiling)
- Serial portion dominates with many processors
- Efficiency **drops** as P increases

---

### **Gustafson's Law (Fixed-Time Model)**

**Formula:**
```
Speedup(P) = s + P·(1-s)

Where s = serial time fraction (in total execution time)
```

**Derivation:**
```
If we run for time T:
- Serial part takes: s·T
- Parallel part takes: (1-s)·T on P processors

Sequential equivalent would take:
T_seq = s·T + P·(1-s)·T

Speedup = T_seq/T = s + P·(1-s) ≈ P (if s small)
```

**Scenario:**
```
┌─────────────────┐
│  Fixed Time T   │  We have T seconds
│  More processors│  → Bigger problem
│  Better result  │  → Higher quality/resolution
└─────────────────┘
```

**Example:**
```
Process images for 100 seconds:
- 1 core:  1 megapixel image
- 10 cores: 9.5 megapixel image (if s=5%)
- 100 cores: 95 megapixel image (nearly linear!)
```

**Characteristics:**
- Speedup **scales linearly** with P
- Efficient use of additional processors
- Reflects real HPC usage patterns

---

### **Direct Comparison**

**Same Example, Different Perspectives:**

Setup: 5% serial, 95% parallel

**Amdahl (fixed N):**
```
P=1:   Speedup = 1.0x
P=10:  Speedup = 5.7x   (expected 10x, got 57%)
P=100: Speedup = 16.8x  (expected 100x, got 17%!)
```

**Gustafson (fixed T):**
```
P=1:   Speedup = 1.0x
P=10:  Speedup = 9.55x  (almost linear!)
P=100: Speedup = 95.05x (excellent scaling!)
```

**Why the Difference?**

**Amdahl:**
```
Time ▼
  │ Serial (fixed)
  │╔════╗
  ├╣ 5% ╠═══════════════════════════
  │╚════╝ Parallel (shrinks)
  └────────────────────> P
  More processors → less to parallelize
```

**Gustafson:**
```
Work ▲
  │ Serial (fixed)
  │╔════╗
  │║ 5% ║
  │╠════╣ Parallel (grows with P)
  │║    ║
  │║    ║
  │║    ║
  └────────────────────> P
  More processors → more parallel work
```

---

### **When to Apply Each:**

**Use Amdahl's Law when:**

1. **Analyzing existing applications**
   - Legacy code with fixed functionality
   - Benchmark suites (SPEC, etc.)

2. **Fixed problem size**
   - Compressing a specific file
   - Sorting a given dataset
   - Processing one video

3. **Understanding limits**
   - "Is parallelization worth it?"
   - Identifying bottlenecks
   - Cost-benefit analysis

4. **Small to medium systems**
   - Desktop/workstation (2-16 cores)
   - Where problem size is constrained

**Use Gustafson's Law when:**

1. **Designing new HPC systems**
   - Supercomputers
   - Data centers
   - Clusters

2. **Scalable problem sizes**
   - Weather simulation (more detail)
   - Molecular dynamics (more atoms)
   - Machine learning (larger models)

3. **Big Data applications**
   - Processing grows with data availability
   - Higher resolution simulations
   - Finer meshes in FEA

4. **Future planning**
   - Predicting value of more cores
   - Justifying hardware purchases
   - Research proposals

---

### **Real-World Examples:**

**Amdahl Applies:**
```
1. Compress single 1GB file
   → Fixed workload
   → 16 cores might be enough
   → 64 cores wasteful

2. Parse single XML document
   → Inherently sequential parts
   → Limited parallelism

3. Compile single C file
   → Dependencies limit parallelism
   → Few cores saturate benefit
```

**Gustafson Applies:**
```
1. Weather forecasting
   → More cores = higher resolution
   → 1km grid → 100m grid → 10m grid
   → Linear scaling common

2. Video rendering farm
   → More cores = more frames/time
   → Or higher quality per frame
   → Work expands to fill resources

3. Database queries
   → More cores = larger working set
   → Scan bigger tables
   → More complex queries
```

---

### **Synthesis:**

**Both laws are correct!** They model different scenarios:

```
┌────────────────┬────────────┬──────────────┐
│   Context      │  Amdahl    │  Gustafson   │
├────────────────┼────────────┼──────────────┤
│ Fixed Work     │  ✓ Applies │  ✗ Doesn't   │
│ Fixed Time     │  ✗ Doesn't │  ✓ Applies   │
│ Weak Scaling   │  ✗ No      │  ✓ Yes       │
│ Strong Scaling │  ✓ Yes     │  ✗ No        │
└────────────────┴────────────┴──────────────┘
```

**Scaling Types:**
- **Strong Scaling (Amdahl)**: Fixed problem, measure speedup
- **Weak Scaling (Gustafson)**: Scale problem with processors, measure throughput

**Modern View:**
- Most HPC follows **Gustafson's model**
- Desktop apps follow **Amdahl's model**
- Both laws provide important insights
- Real applications lie somewhere in between

**Key Insight:**
> Amdahl: "Don't bother with 1000 cores for small problems."
> Gustafson: "Do bother with 1000 cores for problems that scale."

Both are right in their respective contexts!

---

## 🎓 Additional Practice Questions

### Question 7: Describe the memory hierarchy in CUDA and explain the characteristics of each memory type. (2 points)

### Question 8: What is warp divergence in CUDA? Provide an example and explain how to minimize it. (2 points)

### Question 9: Explain the concept of "stream coherence" and its relation to SIMD processing. (1 point)

### Question 10: Compare coarse-grain, fine-grain, and simultaneous multi-threading. (2 points)

---

## 📝 Exam Tips

1. **Always explain your answers** - "Without explanation, answer will be considered invalid"
2. **Use diagrams** when helpful (memory layouts, execution timelines)
3. **Give concrete examples** for abstract concepts
4. **Show formulas** for performance metrics
5. **Compare and contrast** when asked
6. **State assumptions** clearly
7. **Manage time**: 1h 30min for 16 points = ~5.5 min per point

---

## ✅ Key Topics Summary (Without OpenMP)

- ✓ PRAM models & algorithms
- ✓ Amdahl's & Gustafson's Laws
- ✓ Architecture (Superscalar, SIMD, Multi-core)
- ✓ Memory hierarchy & bandwidth
- ✓ CUDA (threads, memory, tiling)
- ✓ Memory consistency models
- ✓ Heterogeneous systems
- ✓ Parallel patterns (Map, Reduce, Scan)
- ✓ Pthreads basics (mutexes, barriers)
- ✗ OpenMP (excluded as requested)

---

*Good luck on your exam! 🚀*
