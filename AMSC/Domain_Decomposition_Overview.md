# Domain Decomposition Techniques: A Comprehensive Overview

Based on the paper by **Formaggia, Sala, and Saleri (2006)** from Politecnico di Milano and Sandia National Laboratories.

---

## Table of Contents

1. [Introduction and Motivation](#1-introduction-and-motivation)
2. [Classification of Domain Decomposition Methods](#2-classification-of-domain-decomposition-methods)
3. [Domain Partitioning Strategies](#3-domain-partitioning-strategies)
4. [The Schur Complement Method](#4-the-schur-complement-method)
5. [Approximate Schur Complement (ASC) Preconditioner](#5-approximate-schur-complement-asc-preconditioner)
6. [The Schwarz Preconditioner](#6-the-schwarz-preconditioner)
7. [Numerical Results](#7-numerical-results)
8. [Conclusions](#8-conclusions)
9. [Summary of Key Formulas](#9-summary-of-key-formulas)

---

## 1. Introduction and Motivation

Modern supercomputers are organized as **distributed environments**, and domain decomposition (DD) techniques provide a natural framework for implementing parallel algorithms for solving partial differential equations (PDEs).

As stated in the paper:

> "The basic idea, as the name goes, is to decompose the original computational domain Ω into subdomains Ωᵢ, i = 1,...,M, which may or may not overlap, and then rewrite the global problem as a 'sum' of contributions coming from each subdomain, which may be computed in parallel." (p. 135)

The ultimate goal is to solve large, sparse linear systems of the form:

$$
Au = f
$$

where $A \in \mathbb{R}^{n \times n}$ is a sparse matrix arising from finite element or finite volume discretizations.

### Scalability

The paper defines scalability as follows:

> "An algorithm is scalable if its performance is proportional to the number of processors employed." (p. 135)

The key measure for DD methods is the **condition number** of the preconditioned system. The algorithm is scalable if the condition number remains approximately constant when the ratio between problem size and number of subdomains is kept constant.

The scalability measure is defined as:

$$
S_{M,N} = \frac{E(M\gamma, M)}{E(N\gamma, N)}
$$

where $E(s, N)$ is the execution time for problem size $s$ using $N$ processors, and $\gamma$ is the size of the problem on a single processor.

---

## 2. Classification of Domain Decomposition Methods

The paper identifies **two main groups** of DD methods (p. 136-137):

### 2.1 Differential Domain Decomposition Methods

- Operate on the **differential problem**
- Interface conditions are recast as boundary conditions for local problems
- Allow different discretization methods on different subdomains
- Better exploit characteristics of the differential problem

### 2.2 Algebraic Domain Decomposition Methods

- Operate at the **algebraic level** (on the matrix system)
- Problem-independent
- Can be interpreted as preconditioners
- **Focus of this paper**

---

## 3. Domain Partitioning Strategies

The paper describes two notable decomposition types (p. 139, Figure 4.1):

### 3.1 Element-Oriented (EO) Decomposition

> "Γ⁽¹'²⁾ reduces to a finite number of disjoint measurable d-1 manifolds... each element of Tₕ belongs exclusively to one of the subdomains Ω⁽ⁱ⁾, while the vertices laying on Γ⁽¹'²⁾ are shared between the subdomain triangulations." (p. 139)

**Characteristics:**
- Interface is a (d-1)-dimensional manifold
- Elements belong to exactly one subdomain
- Vertices on the interface are shared

```
┌─────────┬─────────┐
│ △ △ △ △ │ △ △ △ △ │
│ △ △ △ △ │ △ △ △ △ │
│    Ω₁   │   Ω₂    │
└─────────┴─────────┘
          ↑
    Interface Γ (shared vertices)
```

### 3.2 Vertex-Oriented (VO) Decomposition

> "Γ⁽¹'²⁾ ⊂ ℝᵈ is formed by one layer of elements of the original mesh laying between Ω⁽¹⁾ and Ω⁽²⁾... each vertex of the original mesh belongs to just one of the two subdomains." (p. 139)

**Characteristics:**
- Interface is a layer of elements
- Each vertex belongs to exactly one subdomain
- Creates extended, overlapping subdomains: $\tilde{\Omega}^{(i)} = \Omega^{(i)} \cup \Gamma^{(1,2)}$

```
┌─────────┬───┬─────────┐
│ △ △ △ △ │ △ │ △ △ △ △ │
│ △ △ △ △ │ △ │ △ △ △ △ │
│    Ω₁   │ Γ │   Ω₂    │
└─────────┴───┴─────────┘
              ↑
    Interface layer (element overlap)
```

The paper notes that **VO is often preferred** because:

> "The transition region Γ⁽¹'²⁾ may be replicated on the processors... and provides a natural means of data communication among processors which also allows to implement a parallel matrix-vector product." (p. 140)

---

## 4. The Schur Complement Method

### 4.1 Block Structure

For an EO decomposition with two subdomains, the system takes the block form (p. 140, equation 4.3):

$$
\begin{pmatrix}
A_{II}^{(1)} & 0 & A_{IB}^{(1)} \\
0 & A_{II}^{(2)} & A_{IB}^{(2)} \\
A_{BI}^{(1)} & A_{BI}^{(2)} & A_{BB}^{(1)} + A_{BB}^{(2)}
\end{pmatrix}
\begin{pmatrix}
u_I^{(1)} \\
u_I^{(2)} \\
u_B
\end{pmatrix}
=
\begin{pmatrix}
f_I^{(1)} \\
f_I^{(2)} \\
f_B
\end{pmatrix}
$$

Where:
- **I** denotes internal nodes
- **B** denotes boundary (interface) nodes
- Superscripts indicate the subdomain

### 4.2 Schur Complement Matrix

The Schur complement is defined as (p. 141, equation 4.5):

$$
S_h = S_h^{(1)} + S_h^{(2)}
$$

where

$$
S_h^{(i)} = A_{BB}^{(i)} - A_{BI}^{(i)} \cdot (A_{II}^{(i)})^{-1} \cdot A_{IB}^{(i)}
$$

### 4.3 Solution Procedure

The paper describes a two-step procedure (p. 141):

**Step 1:** Solve for boundary values:

$$
S_h \cdot u_B = g
$$

where $g = g^{(1)} + g^{(2)}$, with $g^{(i)} = f_B^{(i)} - A_{BI}^{(i)} \cdot (A_{II}^{(i)})^{-1} \cdot f_I^{(i)}$

**Step 2:** Solve for internal values (in parallel):

$$
A_{II}^{(i)} \cdot u_I^{(i)} = f_I^{(i)} - A_{IB}^{(i)} \cdot u_B, \quad i = 1, 2
$$

### 4.4 Algorithm for Matrix-Vector Product

The paper provides **Algorithm 1** for computing $w_B = S_h v_B$ (p. 144):

```
ALGORITHM 1: COMPUTATION OF w_B = S_h v_B

1. Restrict v_B to each subdomain boundary:
   v_B^(i) = R_i v_B,  i = 1, ..., M

2. For every Ω^(i), i = 1, ..., M solve:
   A_II^(i) u_I^(i) = -A_IB^(i) v_B^(i)

   Then compute:
   w_B^(i) = Σ_j E^(ij) v_B^j + A_BB^(i) v_B^(i) - A_BI^(i) u_I^(i)

3. Apply the prolongation operators:
   w_B = Σ_{i=1}^M R_i^T w_B^(i)
```

### 4.5 Preconditioners for the Schur Complement

The paper summarizes convergence rates in **Table 4.1** (p. 145):

| Preconditioner | Condition Number Estimate |
|----------------|---------------------------|
| Jacobi ($P_h^J$) | $\kappa \leq C \cdot H^{-2} \cdot (1 + \log(H/h))^2$ |
| Bramble-Pasciak-Schatz ($P_h^{BPS}$) | $\kappa \leq C \cdot (1 + \log(H/h))^2$ |
| Vertex-Space ($P_h^{VS}$) | $\kappa \leq C \cdot (1 + \log \delta^{-1})^2$ |
| Wire-Basket ($P_h^{WB}$) | $\kappa \leq C \cdot (1 + \log(H/h))^2$ |
| Balancing Neumann-Neumann ($P_h^{NN,b}$) | $\kappa \leq C \cdot (1 + \log(H/h))^2$ |

Where:
- $H$ = subdomain size
- $h$ = mesh size
- $\delta$ = overlap fraction
- $C$ = constant independent of $h$ and $H$

### 4.6 General Preconditioner Construction

The paper presents a general approach using identity (4.10):

$$
S_h^{-1} = \begin{pmatrix} 0 & I \end{pmatrix} A^{-1} \begin{pmatrix} 0 \\ I \end{pmatrix}
$$

This allows building a Schur preconditioner from **any preconditioner** $P_A^{-1}$ of the original matrix $A$:

$$
P_{Schur}^{-1} v_B = R_B P_A^{-1} R_B^T v_B
$$

---

## 5. Approximate Schur Complement (ASC) Preconditioner

### 5.1 Motivation

> "Although the Schur complement matrix is better conditioned than A, its multiplication with a vector is in general expensive." (p. 146)

### 5.2 Construction

The ASC preconditioner replaces exact solves with approximations (p. 146):

$$
P_{ASC} = \begin{pmatrix} \tilde{A}_{II} & 0 \\ A_{BI} & I \end{pmatrix} \begin{pmatrix} I & \tilde{A}_{II}^{-1} A_{IB} \\ 0 & \tilde{S} \end{pmatrix}
$$

Where:
- $\tilde{A}_{II} \approx A_{II}$ (e.g., ILU factorization)
- $\tilde{S} = A_{BB} - A_{BI} \tilde{A}_{II}^{-1} A_{IB}$

### 5.3 Algorithm 2: Application of ASC Preconditioner

From page 147:

```
ALGORITHM 2: APPLICATION OF THE ASC PRECONDITIONER

1. Apply the lower triangular part of P_ASC:
   - Solve: Ã_II y_I = r_I
   - Compute: y_B = r_B - A_BI y_I

2. Apply the upper triangular part of P_ASC:
   - Solve: S̃ z_B = y_B
   - Compute: z_I = y_I - Ã_II^{-1} A_IB z_B
```

### 5.4 Notation

The paper uses notation **ASC-L-iluf** to denote:
- **L** = number of iterations of inner Krylov solver
- **ILU(f)** = ILU factorization with fill-in level f

Examples:
- `ASC-2-ilu0` = 2 inner iterations, ILU(0)
- `ASC-4-ilu1` = 4 inner iterations, ILU(1)

---

## 6. The Schwarz Preconditioner

### 6.1 Basic Construction

The additive Schwarz preconditioner is defined as (p. 148, equation 4.13):

$$
P_{AS}^{-1} = \sum_{i=1}^{M} P^{(i)} B^{(i)} R^{(i)}
$$

Where:
- $B^{(i)}$ is the local matrix for extended subdomain $\tilde{\Omega}^{(i)}$
- $R^{(i)}$ is the restriction operator from global to local
- $P^{(i)}$ is the prolongation operator (usually $P^{(i)} = (R^{(i)})^T$)

### 6.2 Scalability Issue

> "Although simple to implement, the scalability of the Schwarz preconditioner is hindered by the weak coupling between far away subdomains." (p. 148)

**Problem illustration:**
```
Information propagation without coarse correction:

Iteration 1:  [Ω₁] → [Ω₂]
Iteration 2:  [Ω₁] → [Ω₂] → [Ω₃]
Iteration 3:  [Ω₁] → [Ω₂] → [Ω₃] → [Ω₄]
...

Many iterations needed for global communication!
```

### 6.3 Solution: Coarse Grid Correction

The paper proposes an **agglomeration-based coarse operator** (Section 4.4.1).

#### Construction of Coarse Space

**Key idea:** Build a coarse space using weight vectors $\beta_s^{(i)}$ that define coarse basis functions as linear combinations of fine basis functions:

$$
\Phi_s^{(i)} = \sum_{k=1}^{n^{(i)}} \beta_{s,k}^{(i)} \cdot \phi_k^{(i)}
$$

Where:
- $\phi_k^{(i)}$ are fine-level basis functions
- $l^{(i)}$ is the local dimension of the coarse space
- Total coarse dimension: $l = \sum_{i=1}^{M} l^{(i)}$

#### Coarse Operators

The coarse matrix and right-hand side are (p. 151):

$$
A_H = R_H A R_H^T
$$

$$
f_H = R_H f
$$

The prolongation matrix $R_H^T \in \mathbb{R}^{n \times l}$ has block structure:

$$
R_H^T = \begin{pmatrix}
\beta_1^{(1)T} & \beta_2^{(1)T} & \cdots & 0 & 0 & \cdots \\
0 & 0 & \cdots & \beta_1^{(2)T} & \beta_2^{(2)T} & \cdots \\
\vdots & & & & & \ddots
\end{pmatrix}
$$

### 6.4 Two Variants of ACM Preconditioner

**One-step (additive) version** (equation 4.15):

$$
P_{ACM,1}^{-1} = P_S^{-1} + R_H^T A_H^{-1} R_H
$$

**Two-step (multiplicative) version** (equation 4.16):

$$
u^{n+1/2} = u^n + P_S^{-1} r^n
$$

$$
u^{n+1} = u^{n+1/2} + R_H^T A_H^{-1} R_H r^{n+1/2}
$$

The corresponding preconditioner (equation 4.17):

$$
P_{ACM,2}^{-1} = P_S^{-1} + R_H^T A_{ACM}^{-1} R_H - P_S^{-1} A R_H^T A_{ACM}^{-1} R_H
$$

### 6.5 Parallel Implementation

For the coarse problem solution:

1. **Limited processors:** Gather $A_H$ on one processor, solve sequentially, broadcast result
2. **Many processors:** Use parallel direct solver

```
Parallel workflow:

1. Each processor computes contribution to r_H
2. Gather r_H on processor 0 (or use parallel solver)
3. Solve: A_H u_H = r_H
4. Scatter/broadcast u_H to all processors
5. Each processor applies prolongation R_H^T locally
```

---

## 7. Numerical Results

### 7.1 2D Poisson Problem

**Test configuration:**
- Domain: $\Omega = (0,1) \times (0,1)$
- Mesh: 180×180 squares (each divided into 2 triangles)
- Discretization: P1 finite elements
- Solver: GMRES(60) or GMRESR
- Tolerance: $10^{-6}$

**Table 4.2** (p. 153) - Iterations to converge:

| Solver | M=4 | M=9 | M=16 | M=25 |
|--------|-----|-----|------|------|
| $P_S$ (no coarse) | - | 57 | 70 | 76 |
| $P_C$ (classical coarse) | - | 42 | 40 | 39 |
| $P_{ACM,1}$ | - | 56 | 69 | 70 |
| $P_{ACM,2}$ | - | 51 | 49 | 46 |
| ASC-2-ilu0 | 99 | 97 | 97 | 99 |
| ASC-4-ilu0 | 82 | 78 | 75 | 71 |
| ASC-2-ilu1 | 68 | 68 | 70 | 69 |
| ASC-2-ilu2 | 52 | 53 | 56 | 52 |

**Key observations:**
- $P_S$ without coarse correction: iterations **increase** with M (poor scalability)
- $P_C$ and $P_{ACM,2}$: iterations **nearly constant** (excellent scalability)
- ASC methods: good scalability regardless of M

**Table 4.3** (p. 154) - CPU time (seconds):

| Method | M=4 | M=9 | M=16 | M=25 |
|--------|-----|-----|------|------|
| $P_S$ | - | 3.90 | 1.59 | 0.77 |
| $P_{ACM,2}$ | - | 4.16 | 2.03 | 0.89 |
| ASC-2-ilu0 | 12.64 | 3.94 | 3.34 | 2.12 |
| ASC-2-ilu2 | 6.44 | 4.32 | 1.77 | 1.54 |

### 7.2 Effect of Coarse Space Dimension

**Tables 4.4-4.5** (p. 154) show the effect of local coarse space dimension $N_p$:

| $N_p$ | M=9 (iter) | M=16 (iter) | M=25 (iter) |
|-------|------------|-------------|-------------|
| 1 | 51 | 49 | 46 |
| 2 | 57 | 54 | 50 |
| 4 | 55 | 49 | 46 |
| 8 | 50 | 45 | 41 |
| 32 | 50 | 34 | 32 |

**Conclusion:** Increasing coarse space dimension improves convergence but with diminishing returns on CPU time.

### 7.3 Compressible Euler Equations

The paper applies DD methods to **aeronautical CFD problems**.

**Test cases** (Table 4.6, p. 157):

| Name | $M_\infty$ | $\alpha$ | Nodes | Cells |
|------|------------|----------|-------|-------|
| FALCON 45k | 0.45 | 1.0° | 45,387 | 255,944 |
| M6 23k | 0.84 | 3.06° | 23,008 | 125,690 |
| M6 42k | 0.84 | 3.06° | 42,305 | 232,706 |
| M6 94k | 0.84 | 3.06° | 94,493 | 666,569 |
| M6 316k | 0.84 | 3.06° | 316,275 | 1,940,182 |

**Results for FALCON 45k** (Table 4.7, p. 157):

| Processors | ASC-1-ilu0 | ASC-2-ilu0 | ASC-4-ilu0 | ASC-8-ilu0 |
|------------|------------|------------|------------|------------|
| 4 | 2542.4 s | 2401.7 s | 2393.2 s | 3319.7 s |
| 8 | 925.5 s | 897.6 s | 1406.6 s | 1423.2 s |
| 16 | 863.7 s | 753.7 s | 561.6 s | 707.2 s |
| 32 | 443.8 s | 332.1 s | **248.6 s** | 398.6 s |

**Key finding:**

> "A value of about 4 seems a good compromise" for the number of inner iterations L (p. 156)

### 7.4 M6 316k Results

**Table 4.11** (p. 160) - 32 processors:

| Method | CPU Time |
|--------|----------|
| $P_S$ (no coarse) | 1524.2 s |
| $P_{ACM,2}$ | 1370.6 s |
| ASC-4-ilu0 | 2691.3 s |

---

## 8. Conclusions

The paper summarizes the key trade-offs (p. 162):

### ASC Method

> "The approximate Schur complement system has generally a better preconditioning property at the price of a higher cost 'per iteration'. It performs better when the ratio unknowns/number of subdomains is 'low'."

**Advantages:**
- Better conditioning
- Attractive when communication is expensive

**Disadvantages:**
- Higher cost per iteration
- Cost scales with square of local DOFs

### Schwarz Method

> "The Schwarz preconditioner is often the matter of choice of many parallel linear algebra packages, because of its rather simple implementation. The minimal overlap variant is also rather attractive in terms of memory usage. Yet it needs a coarse operator to obtain scalability."

**Advantages:**
- Simple implementation
- Low memory (minimal overlap)
- Lower cost per iteration

**Disadvantages:**
- Needs coarse correction for scalability
- Slower convergence

### General Principle

> "All efficient DD preconditioners consist of a local and a global component. The local part acts at the subdomain level... the global part provides instead an overall communication among far away subdomains." (p. 162)

```
Efficient DD Preconditioner = Local Part + Global Part
                                  ↓            ↓
                           Subdomain      Coarse grid
                             solves       correction
```

---

## 9. Summary of Key Formulas

### Core Definitions

| Component | Formula |
|-----------|---------|
| **Schur Complement** | $S_h = A_{BB} - A_{BI} A_{II}^{-1} A_{IB}$ |
| **Additive Schwarz** | $P_{AS}^{-1} = \sum_i P^{(i)} B^{(i)} R^{(i)}$ |
| **Coarse Matrix** | $A_H = R_H A R_H^T$ |
| **ACM (additive)** | $P_{ACM,1}^{-1} = P_S^{-1} + R_H^T A_H^{-1} R_H$ |
| **Schur Preconditioner** | $P_{Schur}^{-1} = R_B P_A^{-1} R_B^T$ |

### Condition Number Estimates

| Method | Estimate |
|--------|----------|
| No preconditioner | $\kappa(S_h) \sim O(h^{-2})$ |
| Jacobi | $\kappa \leq C H^{-2} (1 + \log(H/h))^2$ |
| Optimal (BPS, NN, WB) | $\kappa \leq C (1 + \log(H/h))^2$ |

### Notation Reference

| Symbol | Meaning |
|--------|---------|
| $\Omega$ | Computational domain |
| $\Omega^{(i)}$ | Subdomain $i$ |
| $\tilde{\Omega}^{(i)}$ | Extended subdomain (with overlap) |
| $\Gamma$ | Interface between subdomains |
| $h$ | Mesh element size |
| $H$ | Subdomain size |
| $\delta$ | Overlap size |
| $M$ | Number of subdomains |
| $R^{(i)}$ | Restriction operator to subdomain $i$ |
| $P^{(i)}$ | Prolongation operator from subdomain $i$ |
| $R_H$ | Restriction to coarse space |

---

## References

Key works cited in the paper:

1. **Quarteroni, A. & Valli, A.** (1999). *Domain Decomposition Methods for Partial Differential Equations*. Oxford University Press.

2. **Smith, B., Bjorstad, P. & Gropp, W.** (1996). *Domain Decomposition, Parallel Multilevel Methods for Elliptic Partial Differential Equations*. Cambridge University Press.

3. **Toselli, A. & Widlund, O.** (2005). *Domain Decomposition Methods - Algorithms and Theory*. Springer.

4. **Saad, Y.** (1996). *Iterative Methods for Sparse Linear Systems*. Thompson.

5. **Karypis, G. & Kumar, V.** (1998). *METIS: Unstructured Graph Partitioning and Sparse Matrix Ordering System*. University of Minnesota.

---

## Appendix: Comparison Table

| Aspect | Schwarz | Schur Complement | ASC |
|--------|---------|------------------|-----|
| **Complexity** | Simple | Moderate | Complex |
| **Memory** | Low | Moderate | Moderate |
| **Cost per iteration** | Low | High | Medium |
| **Convergence rate** | Slow | Fast | Medium |
| **Scalability (basic)** | Poor | Moderate | Good |
| **With coarse correction** | Excellent | - | - |
| **Parallelism** | Excellent | Good | Good |
| **Implementation** | Easy | Moderate | Moderate |

---

*Document generated from: Formaggia, L., Sala, M., & Saleri, F. (2006). Domain Decomposition Techniques. In Bentley Systems.*
