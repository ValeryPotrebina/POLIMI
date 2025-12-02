# Domain Decomposition for PDEs: Tutorial

Based on the lecture notes by **Luca Formaggia** (November 10, 2024)

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [The Schwarz Method (Continuous Setting)](#2-the-schwarz-method-continuous-setting)
3. [The Algebraic Setting](#3-the-algebraic-setting)
4. [Schwarz Solver (Discrete Version)](#4-schwarz-solver-discrete-version)
5. [Schwarz as Preconditioner](#5-schwarz-as-preconditioner)
6. [Practical Assignment](#6-practical-assignment)

---

## 1. Introduction

Domain decomposition techniques are a common tool for implementing **parallelization** to solve problems governed by partial differential equations.

### Model Problem (1D)

The tutorial focuses on the following boundary value problem:

$$
\begin{cases}
-\mu \frac{d^2 u}{dx^2} + c \, u(x) = f(x), & x \in (a, b) \\
u(a) = u_a \\
u(b) = u_b
\end{cases}
$$

Where:
- $\mu > 0$ — diffusion coefficient
- $c \geq 0$ — reaction coefficient
- $f$ — given source function

### Main Approaches

The Schwarz method can be implemented as:
1. **Solver** — iterative method to find the solution
2. **Preconditioner** — accelerator for other iterative methods

---

## 2. The Schwarz Method (Continuous Setting)

### 2.1 Domain Partitioning

#### Step 1: Non-overlapping partition

Divide the interval $(a, b)$ into $N$ non-overlapping subdomains:

$$
(\tilde{a}_i, \tilde{b}_i), \quad i = 1, \ldots, N
$$

Where:
- $\tilde{a}_1 = a$
- $\tilde{b}_N = b$
- $\tilde{b}_i = \tilde{a}_{i+1}$ for $i = 1, \ldots, N-1$

```
Non-overlapping partition:
├────────┼────────┼────────┼────────┤
a       ã₂      ã₃      ã₄       b
    Ω̃₁      Ω̃₂      Ω̃₃      Ω̃₄
```

#### Step 2: Extension with overlap δ > 0

Extend to $N$ **overlapping** subdomains:

$$
(a_i, b_i), \quad i = 1, \ldots, N
$$

Where:
- $a_1 = a$, $b_N = b$
- $a_i = \tilde{a}_i - \delta/2$ for $i = 2, \ldots, N$
- $b_i = \tilde{b}_i + \delta/2$ for $i = 1, \ldots, N-1$
- Overlap: $b_i - a_{i+1} = \delta$

```
Overlapping partition (δ > 0):
├──────────┤
     ├──────────┤
          ├──────────┤
               ├──────────┤
a    a₂   a₃   a₄        b
   b₁   b₂   b₃

   ←─δ─→  overlap regions
```

### 2.2 Operators

#### Restriction Operator $R_i$

Given any function $g$ defined on $[a, b]$, returns its restriction to $[a_i, b_i]$:

$$
R_i g(x) =
\begin{cases}
g(x), & x \in [a_i, b_i] \\
0, & x \notin [a_i, b_i]
\end{cases}
$$

#### Prolongation Operator $P_i$

Maps local function $g_i$ from $(\tilde{a}_i, \tilde{b}_i)$ to $(a, b)$:

$$
P_i g_i(x) =
\begin{cases}
g_i(x), & x \in (\tilde{a}_i, \tilde{b}_i) \\
0, & \text{otherwise}
\end{cases}
$$

**Note:** The prolongation acts on the **non-expanded** domain $(\tilde{a}_i, \tilde{b}_i)$, not on the extended domain $(a_i, b_i)$.

### 2.3 Schwarz Iteration Algorithm

```
SCHWARZ ITERATION (Continuous Version)

Input: Initial functions u_i^(0) on each extended subdomain (a_i, b_i)

For k = 1, 2, ... do:

    For i = 1, ..., N (in parallel):

        Solve the local problem:
        ┌─────────────────────────────────────────────────────┐
        │  -μ d²u_i^(k)/dx² + c u_i^(k) = R_i f               │
        │                                                      │
        │  Boundary conditions:                                │
        │  • u_i^(k)(a_i) = u_{i-1}^(k-1)(a_i)  if i ≠ 1      │
        │                 = u_a                  if i = 1      │
        │                                                      │
        │  • u_i^(k)(b_i) = u_{i+1}^(k-1)(b_i)  if i ≠ N      │
        │                 = u_b                  if i = N      │
        └─────────────────────────────────────────────────────┘

    End For

    Check convergence:
        If max_{i=1}^N ‖u_i^(k) - u_i^(k-1)‖ ≤ ε  then EXIT

End For

Output: Reconstruct full solution
        u = Σ_{i=1}^N P_i u_i^(k)
```

### 2.4 Key Observations

> "In practice, we solve a Dirichlet problem in each subdomain with the boundary condition taken from the adjacent subdomain at the previous iteration, apart from the actual domain boundary where we need to enforce the boundary condition of the global problem." (p. 2)

**Convergence properties:**
- The method converges
- Convergence rate depends on:
  - Number of subdomains $N$
  - Length of the overlap $\delta$

**Interface treatment:**
- At interfaces between subdomains, values may differ
- Typically take an **average** of the two values
- Alternative: average over the whole overlapped region

**Parallelization:**
- Highly parallelizable
- Only communication needed: boundary values exchange
- Only need to store last two iterations: $u_i^{(k)}$ and $u_i^{(k-1)}$

---

## 3. The Algebraic Setting

### 3.1 Finite Difference Discretization

Subdivide $[a, b]$ into $n$ elements of constant length $h$:

$$
[x_k, x_{k+1}], \quad k = 0, \ldots, n-1
$$

Where:
- $h = x_{k+1} - x_k$
- $x_0 = a$, $x_n = b$

### 3.2 Discrete Problem

Find $u_k \approx u(x_k)$ for $k = 0, \ldots, n$ satisfying:

$$
\begin{cases}
\mu \frac{-u_{k-1} + 2u_k - u_{k+1}}{h^2} + c \, u_k = f(x_k), & k = 1, \ldots, n-1 \\
u_0 = u_a \\
u_n = u_b
\end{cases}
$$

### 3.3 Matrix Form

The system can be written as:

$$
A \mathbf{u} = \mathbf{b}
$$

Where:
- $\mathbf{u} = [u_0, u_1, \ldots, u_n]^T$
- $\mathbf{b} = [u_a, f(x_1), \ldots, f(x_{n-1}), u_b]^T$

**Matrix A** (tridiagonal):

$$
A = \begin{pmatrix}
1 & 0 & & & \\
-\frac{1}{h^2} & \frac{2}{h^2} + c & -\frac{1}{h^2} & & \\
& -\frac{1}{h^2} & \frac{2}{h^2} + c & -\frac{1}{h^2} & \\
& & \ddots & \ddots & \ddots \\
& & & & 1
\end{pmatrix}
$$

**Structure:**
- First and last rows: enforce boundary conditions
- Interior rows: finite difference stencil

---

## 4. Schwarz Solver (Discrete Version)

### 4.1 Partition of Node Indices

The domain subdivision corresponds to **partitions of node indices**:

- $j$ — local node index in partition $i$, where $j = 0, \ldots, n_i - 1$
- $k$ — Schwarz iteration index
- $l > 0$ — overlap as number of mesh elements

### 4.2 Local-to-Global Map

Define $\sigma_i(j)$ that returns the **global index** of local node $j$ in partition $i$:

$$
f_j = f(x_{\sigma_i(j)})
$$

### 4.3 Local Problems

For each subdomain $i$, solve:

$$
\begin{cases}
\mu \frac{-u_{i,j-1}^{(k)} + 2u_{i,j}^{(k)} - u_{i,j+1}^{(k)}}{h^2} + c \, u_{i,j}^{(k)} = f_j, & j = 1, \ldots, n_i - 2 \\[10pt]
u_{i,0}^{(k)} = u_{i-1, n_{i-1}-1-l}^{(k-1)} & \text{if } i \neq 1, \text{ else } u_a \\[5pt]
u_{i,n_i}^{(k)} = u_{i+1, l}^{(k-1)} & \text{if } i \neq N, \text{ else } u_b
\end{cases}
$$

### 4.4 Building Partitions

**Procedure:**

1. Partition indices $1, \ldots, n$ into $N$ **non-overlapping** partitions of adjacent nodes
2. Add $l$ nodes on each end (except at domain boundaries)

```
Example: n = 12 nodes, N = 3 partitions, l = 1 overlap

Global indices:    0   1   2   3   4   5   6   7   8   9  10  11  12

Non-overlapping:  [0   1   2   3   4] [5   6   7   8] [9  10  11  12]

With overlap l=1: [0   1   2   3   4   5]
                          [4   5   6   7   8   9]
                                  [8   9  10  11  12]
```

### 4.5 Projection Operators (Discrete)

**Projection matrix $P_i$:**
- Size: $n_i$ rows × $n$ columns
- Row $j$ has all zeros except element $\sigma_i(j)$ which equals 1

**Operations:**
- $\mathbf{b}_i = P_i \mathbf{b}$ — local projection of global vector
- $A_i = P_i A P_i^T$ — local matrix

### 4.6 Convergence Test

Compute 2-norm between successive iterates:

$$
\| \mathbf{u}^{(k)} - \mathbf{u}^{(k-1)} \|_2 \leq \epsilon
$$

### 4.7 Solution Reconstruction

Use only the **non-overlapping** indices:

$$
u_{\sigma_i(j)} = u_{i,j}^{(k)}, \quad j = l, \ldots, n_j - 1 - l, \quad i = 1, \ldots, N
$$

(With appropriate modifications for boundary partitions)

---

## 5. Schwarz as Preconditioner

### 5.1 Motivation

Instead of using Schwarz as a standalone solver, use it to **accelerate** another iterative method.

### 5.2 Richardson Iteration (Non-preconditioned)

Starting from initial iterate $\mathbf{u}^{(0)}$, for $k = 0, 1, \ldots$:

$$
\mathbf{u}^{(k+1)} = \mathbf{u}^{(k)} + \alpha \, \mathbf{r}^{(k)}
$$

Where:
- $\mathbf{r}^{(k)} = \mathbf{b} - A\mathbf{u}^{(k)}$ is the **residual**
- $\alpha > 0$ is a suitably chosen parameter

Iterate until residual is sufficiently small.

### 5.3 Preconditioned Version

**Algorithm:**

```
PRECONDITIONED RICHARDSON WITH SCHWARZ

For k = 0, 1, 2, ... do:

    1. Compute residual:
       r^(k) = b - A u^(k)

    2. Compute preconditioned residual (in parallel):
       Solve: S z^(k) = r^(k)

       (This is ONE Schwarz iteration with homogeneous BC
        and r^(k) as right-hand side)

    3. Update solution:
       u^(k+1) = u^(k) + α z^(k)

    4. Check convergence

End For
```

### 5.4 Why This Works

> "The idea is that at the global level you do only matrix-vector multiplications, which are easily parallelizable, and to speed up the convergence you use a parallel preconditioner exploiting the fact that if the subdomains are sufficiently small, the solution of the problem in each subdomain is cheap." (p. 3)

**Computational structure:**

| Operation | Parallelism | Cost |
|-----------|-------------|------|
| $A\mathbf{u}$ (matrix-vector) | Easy to parallelize | $O(n)$ |
| Local solves in $S$ | Fully parallel | $O(n_i)$ each |
| Communication | Boundary exchange only | $O(N)$ |

### 5.5 Limitation and Improvement

**Problem:** The basic Schwarz preconditioner is too "local" — distant subdomains don't communicate well.

**Solution:** Add a **coarse correction**

> "The simplest way to build the correction is to take the central node of each subdomain and build the discrete problem with just those nodes." (p. 3)

```
Fine grid (many nodes):     Coarse grid (N nodes):

○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○     ●       ●       ●       ●
    ↓       ↓       ↓
  center  center  center    One node per subdomain
```

**Two-level preconditioner:**

$$
P^{-1} = P_{Schwarz}^{-1} + P_{coarse}^{-1}
$$

---

## 6. Practical Assignment

### 6.1 Required Tasks

1. **Implement a class** to solve problem (1)
   - Use Thomas algorithm for tridiagonal system
   - Alternative: use Eigen library

2. **Implement Schwarz iterator** as a solver
   - Use composition pattern for the class

3. **Implement parallel version**
   - Choose: MPI or OpenMP

4. **Visualization**
   - Choose any graphics library/tool

### 6.2 Code Structure Suggestion

```cpp
// Suggested class hierarchy

class TridiagonalSolver {
    // Thomas algorithm or Eigen-based solver
    void solve(Vector& x, const Vector& b);
};

class LocalProblem {
    // Single subdomain problem
    TridiagonalSolver solver;
    int n_local;        // local number of nodes
    int overlap;        // overlap size l
    double h;           // mesh size

    void solve(Vector& u_local, const Vector& f_local,
               double bc_left, double bc_right);
};

class SchwarzSolver {
    // Composed of LocalProblem instances
    std::vector<LocalProblem> subdomains;
    int N;              // number of subdomains
    double tolerance;

    void solve(Vector& u_global, const Vector& f_global);
};

// Parallel version (MPI)
class ParallelSchwarzSolver {
    LocalProblem my_subdomain;  // each process owns one
    MPI_Comm comm;

    void exchangeBoundaries();
    void solve(...);
};
```

### 6.3 Further Developments (Exam Projects)

1. **Extend to 2D problems**
   - Rectangular domain with 2D partitioning
   - Use sparse matrix formats

2. **Preconditioner version**
   - Interface with iterative solvers from `LinearAlgebra/IML_Eigen`
   - Requires Eigen library

3. **Algebraic coarse grid operator**
   - Automatic construction without geometric information
   - Agglomeration-based approach

4. **GPU implementation**
   - CUDA or OpenCL
   - Exploit massive parallelism

---

## Summary

### Key Formulas

| Concept | Formula |
|---------|---------|
| **Overlap** | $b_i - a_{i+1} = \delta$ |
| **Local BC (left)** | $u_i^{(k)}(a_i) = u_{i-1}^{(k-1)}(a_i)$ |
| **Local BC (right)** | $u_i^{(k)}(b_i) = u_{i+1}^{(k-1)}(b_i)$ |
| **Convergence test** | $\max_i \|u_i^{(k)} - u_i^{(k-1)}\| \leq \epsilon$ |
| **Solution reconstruction** | $u = \sum_i P_i u_i^{(k)}$ |
| **Local matrix** | $A_i = P_i A P_i^T$ |
| **Preconditioned residual** | $S z^{(k)} = r^{(k)}$ |

### Convergence Factors

| Factor | Effect on Convergence |
|--------|----------------------|
| Overlap $\delta$ ↑ | Faster convergence |
| Number of subdomains $N$ ↑ | Slower convergence |
| Coarse correction | Restores scalability |

### Parallelization Summary

```
Schwarz Iteration — Parallel Structure:

    ┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐
    │ Proc 0  │   │ Proc 1  │   │ Proc 2  │   │ Proc 3  │
    │ Solve   │   │ Solve   │   │ Solve   │   │ Solve   │
    │ local   │   │ local   │   │ local   │   │ local   │
    └────┬────┘   └────┬────┘   └────┬────┘   └────┬────┘
         │             │             │             │
         └──────┬──────┴──────┬──────┴──────┬──────┘
                │             │             │
           Exchange boundary values (MPI_Send/Recv)
                │             │             │
         ┌──────┴──────┬──────┴──────┬──────┴──────┐
         │             │             │             │
    ┌────┴────┐   ┌────┴────┐   ┌────┴────┐   ┌────┴────┐
    │ Proc 0  │   │ Proc 1  │   │ Proc 2  │   │ Proc 3  │
    │ Next    │   │ Next    │   │ Next    │   │ Next    │
    │ iter    │   │ iter    │   │ iter    │   │ iter    │
    └─────────┘   └─────────┘   └─────────┘   └─────────┘
```

---

*Document generated from: Formaggia, L. (2024). Domain decomposition for PDEs. Lecture notes, Politecnico di Milano.*
