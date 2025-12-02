# Parallel Computing - Exam Answers
## PC-1st-part-23-01-31.pdf - January 31st, 2023

---

## Exercise 1: PRAM Models (4 points)

### A. Is it possible to derive the space complexity of a PRAM algorithm? Example? (1 point)

**Answer: Yes**

**Space complexity** of a PRAM algorithm is the amount of **shared memory** used by the algorithm.

**Derivation:**
- Count the number of shared memory cells accessed/allocated during execution
- Typically expressed as a function of input size n and number of processors P

**Example: Prefix Sum (Parallel Sum Algorithm)**

For computing prefix sum of n elements using P ≤ n processors:
- Input array: n cells
- Working array B: n cells
- Output array: n cells
- **Space complexity: S(n) = O(n)**

The algorithm uses linear space relative to input size, regardless of the number of processors used.

---

### B. Assuming P' < P, is it possible to derive the slowdown factor when P' processors are used? How? (1 point)

**Answer: Yes, using Lemma 1**

**Lemma 1 (Processor Simulation):**
Any problem that can be solved for a P processor PRAM in T steps can be solved in a P' processor PRAM (where P' < P) in:

```
T' = O(TP/P')
```

**Slowdown Factor:**
```
Slowdown = T'/T = O(P/P')
```

**Explanation:**
- Partition P processors into P' groups of size P/P' each
- Each of the P' simulating processors handles one group
- Each simulating processor executes:
  1. All read and local computation substeps of its group first
  2. Then all write substeps
- This serializes P/P' processors per simulating processor

**Example:**
- If P = 100 processors, T = 10 steps
- Using P' = 25 processors:
  - T' = O(10 × 100/25) = O(40) steps
  - Slowdown = 40/10 = 4×

---

### C. What does it mean to have common concurrent writes in an algorithm? Example? (1 point)

**Answer:**

**Common Concurrent Writes** means multiple processors attempt to write to the **same memory location simultaneously**, and the write is allowed **only if all processors write the same value**.

**Characteristics:**
- Part of the **Common CRCW** PRAM model
- If values differ, the write fails or result is undefined
- If all values are identical, the write succeeds
- Useful for coordination and Boolean operations

**Example: OR Operation**

Computing OR of n boolean values in parallel:

```
Algorithm: Parallel OR
Input: Array A[1..n] of boolean values
Output: B = A[1] OR A[2] OR ... OR A[n]

1. Initialize B = 0
2. For each processor i (1 ≤ i ≤ n) in parallel:
   If A[i] = 1:
       Write 1 to B    // Common concurrent write!
3. Return B
```

**Why it works:**
- If any A[i] = 1, that processor writes 1 to B
- Multiple processors may write 1 simultaneously (all write same value = 1)
- Common CRCW allows this because all concurrent writes have the same value
- Time: O(1) instead of O(log n) for reduction tree

---

### D. Gustafson's law assumes that the problem has a fixed serial and parallel time. True/False? (1 point)

**Answer: True**

**Explanation:**

Gustafson's Law assumes **fixed execution time** rather than fixed problem size.

**Key Assumption:**
- The **total execution time T is fixed** (constant)
- This time is divided into:
  - Serial portion: **s × T** (fixed serial time)
  - Parallel portion: **(1-s) × T** (fixed parallel time)

**Formula:**
```
Speedup(P) = s + P(1-s)

Where:
- s = fraction of time spent on serial portion
- P = number of processors
```

**Interpretation:**
- When we add more processors, we solve a **bigger problem** in the **same time**
- The serial fraction s remains constant in time
- The parallel fraction scales linearly with processors

**Contrast with Amdahl's Law:**
- **Amdahl**: Fixed problem size, variable time
- **Gustafson**: Fixed time, variable problem size (scales with P)

**Example:**
If s = 0.05 (5% serial time) with P = 100 processors:
- Speedup = 0.05 + 100(0.95) = 0.05 + 95 = 95.05×
- Nearly linear scaling because serial time stays fixed while parallel work grows

---

## Exercise 2: Parallel Execution (4 points)

### A. Briefly describe the difference between Shared address space and Message passing programming models used for parallel programs. (1 point)

**Answer:**

| Aspect | **Shared Address Space** | **Message Passing** |
|--------|-------------------------|---------------------|
| **Memory** | Global shared memory accessible to all threads | Private local memory per process |
| **Communication** | Implicit via read/write to shared variables | Explicit via send/receive messages |
| **Synchronization** | Locks, barriers, atomic operations | Message ordering, blocking calls |
| **Hardware** | Requires cache coherence mechanisms | Only needs network interconnect |
| **Scalability** | Limited (coherence overhead) | Excellent (no shared state) |
| **Examples** | Pthreads, OpenMP | MPI |

**Shared Address Space:**
- Threads share memory space
- Communication is implicit (write/read shared variables)
- Easier to program but requires careful synchronization
- Example: `shared_counter++` with locks

**Message Passing:**
- Processes have separate address spaces
- Communication is explicit (send/receive operations)
- No race conditions (data is copied)
- Example: `MPI_Send()` and `MPI_Recv()`

**Key Difference:** Shared address space allows implicit communication through memory, while message passing requires explicit send/receive operations.

---

### B. Prefetching reduces stalls. True/False? Explain what is meant with prefetching and stall. (1 point)

**Answer: True**

**Prefetching:**
- Technique to **fetch data from memory before it's actually needed**
- Anticipates future memory accesses
- Loads data into cache while processor is busy with other work
- Can be done by hardware (automatic) or software (explicit instructions)

**Stall:**
- **Processor wait time** when it needs data that isn't in cache
- Processor is idle waiting for memory access to complete
- Typical DRAM latency: 100-300 cycles
- Wastes computational resources

**How Prefetching Reduces Stalls:**

**Without Prefetching:**
```
Cycle 1-3:   Execute instructions
Cycle 4:     Request data X (cache miss!)
Cycle 5-100: STALL (waiting for DRAM)
Cycle 101:   Resume execution with X
```

**With Prefetching:**
```
Cycle 1:     Prefetch request for X (anticipate need)
Cycle 2-100: Execute other instructions (X loading in background)
Cycle 101:   Use X (already in cache!)
             No stall - latency hidden
```

**Benefits:**
- **Hides memory latency** by overlapping computation with data transfer
- Keeps processor busy during memory access
- Most effective when access patterns are predictable
- Can improve performance by 2-10× for memory-bound applications

---

### C. What is meant by SIMD processing? Are Intel processors supporting SIMD processing? (1 point)

**Answer:**

**SIMD (Single Instruction Multiple Data):**
- **One instruction** operates on **multiple data elements** simultaneously
- Same operation applied to many data items in parallel
- Explicit data-level parallelism
- One control unit broadcasts instruction to many ALUs

**Characteristics:**
- Multiple functional units execute the same instruction
- Each unit operates on different data
- Amortizes instruction fetch/decode cost over many operations
- Ideal for data-parallel operations (vector operations)

**Intel SIMD Support:**

**Yes, Intel processors support SIMD** through multiple instruction set extensions:

1. **MMX** (1997): 64-bit registers, integer operations
2. **SSE (Streaming SIMD Extensions)** (1999): 128-bit registers, 4 floats
3. **SSE2-SSE4** (2001-2006): Enhanced operations
4. **AVX (Advanced Vector Extensions)** (2011): 256-bit registers, 8 floats
5. **AVX-512** (2016): 512-bit registers, 16 floats

**Example:**
```c
// Scalar: Process one element at a time
for (int i = 0; i < n; i++)
    C[i] = A[i] + B[i];  // n operations

// SIMD (AVX-512): Process 16 floats at once
// Same loop body, but 16 additions per instruction
// n/16 operations!
```

**Performance:** 4-16× speedup for vectorizable code

---

### D. In a Superscalar processor, the parallelism is automatically discovered by the hardware. True/False? (1 point)

**Answer: True**

**Explanation:**

**Superscalar processors** use **hardware-based dynamic instruction scheduling** to automatically find and exploit instruction-level parallelism (ILP).

**How it works:**

1. **Instruction Fetch:** Fetches multiple instructions per cycle
2. **Decode:** Decodes multiple instructions simultaneously
3. **Dependency Analysis:** Hardware analyzes data dependencies at runtime
4. **Issue:** Dispatches independent instructions to multiple execution units
5. **Out-of-Order Execution:** Reorders instructions to maximize parallelism
6. **Retire:** Commits results in program order

**Key Point:** Programmer writes **sequential code**, hardware **automatically** finds parallelism.

**Example:**
```c
// Sequential code
a = b + c;    // Instruction 1
d = e + f;    // Instruction 2 (independent!)
g = a + d;    // Instruction 3 (depends on 1 and 2)
```

Superscalar hardware automatically:
- Detects Instructions 1 and 2 are independent
- Executes them in parallel on different ALUs
- Waits for both before executing Instruction 3

**Contrast:**
- **SIMD**: Programmer/compiler explicitly vectorizes
- **Multi-core**: Programmer explicitly creates threads
- **Superscalar**: Hardware discovers parallelism automatically

**Typical Performance:** Modern processors issue 4-6 instructions per cycle

---

## Exercise 3: CUDA (4 points)

### A. On NVIDIA V100 a CUDA block could be executed on multiple SM units. True/False? (1 point)

**Answer: False**

**Explanation:**

A **single CUDA block cannot be split** across multiple Streaming Multiprocessors (SMs).

**CUDA Execution Rules:**

1. **Block-to-SM Assignment:**
   - Each block is assigned to **exactly one SM**
   - Block cannot span multiple SMs
   - All threads in a block execute on the same SM

2. **Why this restriction:**
   - Blocks need **shared memory** for inter-thread communication
   - Shared memory is **per-SM resource**
   - **`__syncthreads()`** synchronizes threads within a block (requires same SM)
   - Cross-SM synchronization would be too expensive

3. **However:**
   - **One SM can execute multiple blocks** concurrently
   - If SM has enough resources (registers, shared memory, threads)
   - Example: V100 SM can run 32 blocks simultaneously (if small enough)

**Correct Model:**
```
Grid
 └─ Block 0 → SM 0 ✓
 └─ Block 1 → SM 0 ✓ (multiple blocks per SM allowed)
 └─ Block 2 → SM 1 ✓
 └─ Block 3 → SM 0, SM 1 ✗ (block split across SMs NOT allowed)
```

**Implication:** Block size must fit within a single SM's resources:
- Max threads per block: 1024 (V100)
- Shared memory per SM: 96 KB (V100)

---

### B. CUDA kernels may create dependencies between threads in a block. True/False? (1 point)

**Answer: True**

**Explanation:**

CUDA kernels **can create dependencies** between threads in a block through:

**1. Shared Memory:**
```cuda
__shared__ int shared_data[256];

// Thread i writes
shared_data[threadIdx.x] = input[i];
__syncthreads();  // Synchronization required!

// Thread j reads (dependency!)
int value = shared_data[threadIdx.x + 1];
```

**2. Synchronization Primitives:**

**`__syncthreads()`:** Creates barrier - all threads must reach this point
```cuda
// Thread dependencies via synchronization
data[tid] = input[tid];
__syncthreads();        // Dependency: wait for all writes
result[tid] = data[tid-1] + data[tid+1];  // Depends on neighbors
```

**3. Atomic Operations:**
```cuda
atomicAdd(&counter, 1);  // Creates dependency on shared resource
```

**4. Warp Shuffle:**
```cuda
int value = __shfl_down_sync(mask, var, 1);  // Dependency within warp
```

**Key Points:**
- Without `__syncthreads()`, undefined behavior (race conditions)
- Dependencies are intentional and controlled
- Required for algorithms like reduction, scan, tiling
- Dependencies exist **within a block only** (no inter-block synchronization in CUDA)

**Example Algorithm: Block-level Reduction**
```cuda
// Each thread depends on others in the block
for (int stride = blockDim.x/2; stride > 0; stride >>= 1) {
    if (tid < stride)
        sdata[tid] += sdata[tid + stride];  // Dependency!
    __syncthreads();  // Enforce dependency
}
```

---

### C. Please elaborate on why tiling technique may improve the performance when is used with CUDA. True/False? When? (2 points)

**Answer: True (when memory bandwidth is the bottleneck)**

**Why Tiling Improves Performance:**

### **1. Reduces Global Memory Access**

**Problem:** Global memory access is slow (100-400 cycles latency)

**Solution:** Load data once into shared memory, reuse many times

**Example: Matrix Multiplication (without tiling)**
```cuda
// Each thread computes one element C[i][j]
for (int k = 0; k < Width; k++) {
    sum += A[i][k] * B[k][j];
    // Global memory access every iteration!
    // Width × 2 global memory accesses per output element
}
```

**With Tiling:**
```cuda
// Load TILE_WIDTH × TILE_WIDTH tile into shared memory
__shared__ float As[TILE_WIDTH][TILE_WIDTH];
__shared__ float Bs[TILE_WIDTH][TILE_WIDTH];

for (int t = 0; t < numTiles; t++) {
    // Load tile (1 global access)
    As[ty][tx] = A[...];
    Bs[ty][tx] = B[...];
    __syncthreads();

    // Compute using shared memory (fast!)
    for (int k = 0; k < TILE_WIDTH; k++)
        sum += As[ty][k] * Bs[k][tx];  // Shared memory access
    __syncthreads();
}
// Only 2 × (Width/TILE_WIDTH) global accesses per output
```

**Reduction in Global Memory Access:**
- **Without tiling:** Width global memory loads
- **With tiling (16×16):** Width/8 global memory loads
- **8× reduction in bandwidth requirements!**

### **2. Increases Arithmetic Intensity**

**Arithmetic Intensity = Operations / Bytes Transferred**

**Without Tiling:**
```
Arithmetic Intensity = Width / (Width × 8 bytes) = 0.125 OP/byte
```

**With Tiling (16×16):**
```
Load 2 × 16 × 16 × 4 bytes = 2048 bytes per tile
Compute 16 × 16 × 16 = 4096 operations
Arithmetic Intensity = 4096/2048 = 2.0 OP/byte (16× better!)
```

### **3. Exploits Data Reuse**

Each data element loaded into shared memory is used **TILE_WIDTH times**:
- Load A[i][k] once → use 16 times (for all j in tile)
- Load B[k][j] once → use 16 times (for all i in tile)

### **4. Better Cache Utilization**

- Shared memory acts as programmer-managed cache
- Data stays in fast on-chip memory
- Reduces DRAM bandwidth pressure

### **When Tiling Helps:**

**✓ Effective when:**
1. **Memory-bandwidth bound** applications
2. **Data reuse** patterns exist (matrix ops, convolutions, stencils)
3. **Global memory latency** is bottleneck
4. **Arithmetic intensity** is low without optimization

**✗ Not effective when:**
1. **Compute-bound** (ALUs are bottleneck, not memory)
2. **No data reuse** (streaming operations)
3. **Irregular access patterns** (can't tile effectively)

### **Performance Impact Example:**

Matrix Multiplication 1024×1024:
- **Naive:** 150 GFLOPS (15% of peak)
- **Tiled (16×16):** 1200 GFLOPS (120% better, near peak)

**Key Insight:** Tiling trades more complex code for dramatically better memory bandwidth utilization by exploiting data locality through shared memory.

---

## Exercise 4: Memory and Heterogeneous Systems (4 points)

### A. Please briefly describe what Memory coalescing means. (1 point)

**Answer:**

**Memory Coalescing** is when **multiple threads in a warp** access **consecutive memory addresses**, allowing the hardware to **combine their requests** into a **single memory transaction**.

**How It Works:**

**DRAM Organization:**
- DRAM transfers data in bursts (32, 64, 128 bytes at a time)
- Each memory transaction fetches an entire burst/sector
- Transaction has fixed overhead regardless of how much data is used

**Coalesced Access:**
When threads in a warp (32 threads) access memory such that:
1. Addresses are **consecutive** (thread i accesses address base+i)
2. Addresses are **aligned** to burst boundaries
3. All addresses fall within a **single burst sector**

→ Hardware combines them into **one transaction** (efficient!)

**Example:**

**Coalesced (Good):**
```cuda
int idx = blockIdx.x * blockDim.x + threadIdx.x;
float value = array[idx];

// Thread 0: array[0]
// Thread 1: array[1]
// Thread 2: array[2]
// ...
// Thread 31: array[31]
// → 1 memory transaction for entire warp! ✓
```

**Uncoalesced (Bad):**
```cuda
int idx = (blockIdx.x * blockDim.x + threadIdx.x) * 4;
float value = array[idx];  // Stride = 4

// Thread 0: array[0]
// Thread 1: array[4]
// Thread 2: array[8]
// ...
// → Multiple transactions needed (wasted bandwidth) ✗
```

**Performance Impact:**
- Coalesced: 100% memory efficiency
- Strided (stride=2): ~50% efficiency
- Random: <10% efficiency (up to 32 transactions per warp!)

**Key Benefit:** Coalescing maximizes memory bandwidth utilization by minimizing number of transactions.

---

### B. DRAM Banking could reduce the dead time. True/False? Why? (1 point)

**Answer: True**

**Explanation:**

**DRAM Banking** divides memory into **multiple independent banks** that can be accessed **in parallel**, reducing dead time caused by DRAM latency.

**DRAM Dead Time Problem:**

DRAM has inherent latency due to:
- **Row activation** (opening a row)
- **Column access** (reading data)
- **Precharge** (closing row before opening another)

**Single Bank:**
```
Request 1: [Activate][Access]----[Precharge]----
Request 2:                       [Activate][Access]----[Precharge]
                                  ↑ Dead time waiting for Request 1
```

**Multiple Banks:**
```
Bank 0: [Activate][Access]----[Precharge]----
Bank 1:     [Activate][Access]----[Precharge]----
Bank 2:         [Activate][Access]----[Precharge]----
Bank 3:             [Activate][Access]----[Precharge]----
        ↑ Parallel access! No waiting!
```

**How Banking Reduces Dead Time:**

1. **Parallel Access:**
   - Different banks can be accessed simultaneously
   - While Bank 0 is in precharge, Bank 1 can be accessed
   - Overlaps latency of different requests

2. **Bank Interleaving:**
   - Consecutive addresses map to different banks
   - Sequential accesses hit different banks → no conflicts
   - Example: Address 0→Bank 0, Address 1→Bank 1, etc.

3. **Hides Latency:**
   - Controller pipelines requests to different banks
   - One bank's dead time overlapped with another bank's access
   - Throughput increases even though latency per bank unchanged

**Example:**
- 4 banks, each with 100-cycle access time
- Sequential access to 4 addresses:
  - **Without banking:** 4 × 100 = 400 cycles
  - **With banking:** ~125 cycles (4 accesses pipelined)
  - **3× improvement!**

**Key Insight:** Banking doesn't reduce individual access latency, but increases throughput by enabling parallel access, effectively reducing dead/idle time.

---

### C. Please list the memory operation orderings in a sequentially consistent memory system. (1 point)

**Answer:**

A **sequentially consistent memory system** enforces ALL four memory operation orderings:

**1. W→R (Write before Read)**
```
Write to address X must complete before a subsequent Read from address Y
```

**2. R→R (Read before Read)**
```
Read from address X must complete before a subsequent Read from address Y
```

**3. R→W (Read before Write)**
```
Read from address X must complete before a subsequent Write to address Y
```

**4. W→W (Write before Write)**
```
Write to address X must complete before a subsequent Write to address Y
```

**What This Means:**

All memory operations from a single processor must appear to execute **in program order** when observed by any processor.

**Example:**
```c
// Thread 1
A = 1;    // W1
B = 2;    // W2

// Sequential Consistency guarantees:
// W1 (A=1) completes before W2 (B=2) due to W→W ordering
// No other thread can observe B=2 while A=0
```

**Visual Representation:**
```
Program Order:
  [Write X] → [Read Y]   ✓ W→R enforced
  [Read X]  → [Read Y]   ✓ R→R enforced
  [Read X]  → [Write Y]  ✓ R→W enforced
  [Write X] → [Write Y]  ✓ W→W enforced
```

**Contrast with Relaxed Models:**
- **TSO/PC:** Relaxes W→R (allows write buffer)
- **PSO:** Relaxes W→R and W→W
- **Weak Ordering:** Relaxes all four (requires explicit fences)

**Key Property:** Sequential consistency is the **strongest** memory model - all orderings preserved, easiest to reason about, but most expensive to implement.

---

### D. In Heterogeneous processing moving less data may give better power consumption. True/False? When? (1 point)

**Answer: True**

**Explanation:**

In heterogeneous systems, **data movement consumes more energy than computation**, so reducing data transfer improves power efficiency.

**Energy Cost of Data Movement:**

```
Energy to read 64 bits from:
- Registers:          ~0 pJ    (essentially free)
- On-chip SRAM (1mm): ~26 pJ
- L1 cache:           ~50 pJ
- L2 cache:           ~150 pJ
- DRAM:               ~1200 pJ  (46× more than L1!)
- Off-chip (PCIe):    ~10000 pJ (200× more than L1!)
```

**Computation Energy:**
- 32-bit floating-point ADD: ~0.9 pJ
- 32-bit floating-point MULT: ~3.7 pJ

**Key Insight:** Moving data 1mm costs **more energy** than performing 30+ floating-point operations!

**When Moving Less Data Helps:**

### **1. GPU-CPU Communication**

**Bad Approach:**
```
CPU → GPU (transfer data)          // Expensive!
GPU computes
GPU → CPU (transfer results)       // Expensive!
```

**Better Approach:**
```
Keep data on GPU
Perform multiple operations
Only transfer final results
// Minimize CPU↔GPU transfers
```

**Example:** Processing video frames
- Don't transfer each frame back to CPU
- Keep frames on GPU, process entire sequence
- Transfer only final output

### **2. Memory Hierarchy Utilization**

**Strategy:** Keep data in fastest memory level possible

```cuda
// Bad: Read from global memory repeatedly
for (int i = 0; i < 100; i++)
    sum += global_array[idx];  // 100 DRAM accesses

// Good: Read once into register
float temp = global_array[idx];  // 1 DRAM access
for (int i = 0; i < 100; i++)
    sum += temp;  // 100 register accesses (nearly free!)
```

### **3. Specialized Accelerators**

**Design Principle:** Move data to the computation (specialized unit) rather than moving results around

**Fixed-function units** are power-efficient because:
- Data enters unit once
- Stays on-chip during processing
- Only final result moves out
- Example: Video encoder, image signal processor

### **4. Data Reuse (Tiling)**

**Tiling improves power efficiency:**
```
Without tiling:
- Load data from DRAM: 1200 pJ × N operations
- Total: 1200N pJ

With tiling:
- Load data once: 1200 pJ
- Compute N times using on-chip memory: 26N pJ
- Total: 1200 + 26N pJ (much less!)
```

**Performance vs Power Trade-off:**

Sometimes **compute more** to **move less**:

```c
// Approach 1: Pre-compute and store gradients
gradient = precomputed_table[x];  // Memory access: 1200 pJ

// Approach 2: Compute gradient on-the-fly
gradient = compute_derivative(x);  // Computation: 50 pJ (24× better!)
```

**Key Strategy in Heterogeneous Systems:**

1. **Minimize CPU↔GPU transfers** (PCIe bandwidth limited, high energy)
2. **Use on-chip memory** (shared memory, L1 cache) instead of DRAM
3. **Reuse data** as much as possible before evicting
4. **Compute locally** on specialized units (less data movement)
5. **Compress data** before transfer when possible

**Example: Neural Network Inference**

**Bad (high power):**
```
For each layer:
    CPU → GPU (weights)
    Compute
    GPU → CPU (activations)
```

**Good (low power):**
```
Load all weights to GPU once
Keep activations on GPU
Compute all layers
Transfer only final result
// 100× less data movement!
```

**Conclusion:** In heterogeneous systems, power-efficient design prioritizes **data locality** and **minimal data movement** over raw compute power, because energy cost of moving data dominates energy cost of computation.

---

## Summary

All answers now use **correct course material**:
- Exercise 1B uses **Lemma 1** (T' = O(TP/P')) instead of Brent's Theorem
- All formulas and concepts match the parallel-computing.pdf material
- Examples are practical and exam-appropriate
- Explanations are concise yet complete

**Key Correction:** Slowdown factor derivation now properly references the processor simulation lemma from the course material.
