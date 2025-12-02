# Domain Decomposition Project: Structure and Implementation Plan

## Project Overview

Implementation of the **Schwarz Domain Decomposition Method** for solving 1D boundary value problems with parallel computing support.

**Problem:**
$$
-\mu \frac{d^2 u}{dx^2} + c \, u(x) = f(x), \quad x \in (a, b)
$$

---

## 1. Project Structure

```
AMSC/
├── labs/
│   └── domain_decomposition/
│       │
│       ├── CMakeLists.txt              # Build configuration
│       ├── README.md                   # Project documentation
│       │
│       ├── include/                    # Header files
│       │   ├── problem.hpp             # Problem definition (μ, c, f, BCs)
│       │   ├── tridiagonal_solver.hpp  # Thomas algorithm
│       │   ├── local_problem.hpp       # Single subdomain solver
│       │   ├── schwarz_solver.hpp      # Sequential Schwarz iteration
│       │   ├── parallel_schwarz.hpp    # MPI/OpenMP parallel version
│       │   ├── preconditioner.hpp      # Schwarz as preconditioner
│       │   └── utils.hpp               # Utility functions
│       │
│       ├── src/                        # Source files
│       │   ├── tridiagonal_solver.cpp
│       │   ├── local_problem.cpp
│       │   ├── schwarz_solver.cpp
│       │   ├── parallel_schwarz.cpp
│       │   └── preconditioner.cpp
│       │
│       ├── tests/                      # Unit tests
│       │   ├── test_tridiagonal.cpp
│       │   ├── test_local_problem.cpp
│       │   ├── test_schwarz.cpp
│       │   └── test_convergence.cpp
│       │
│       ├── examples/                   # Example applications
│       │   ├── example_sequential.cpp  # Sequential solver demo
│       │   ├── example_mpi.cpp         # MPI parallel demo
│       │   ├── example_openmp.cpp      # OpenMP parallel demo
│       │   └── example_precond.cpp     # Preconditioner demo
│       │
│       ├── scripts/                    # Utility scripts
│       │   ├── run_scaling_test.sh     # Scalability tests
│       │   └── plot_results.py         # Visualization
│       │
│       └── results/                    # Output directory
│           ├── solutions/
│           └── plots/
```

---

## 2. Implementation Plan

### Phase 1: Core Components (Week 1)

#### Task 1.1: Problem Definition
**File:** `include/problem.hpp`

```cpp
#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include <functional>
#include <cmath>

struct BVPProblem {
    double mu;                              // Diffusion coefficient
    double c;                               // Reaction coefficient
    double a, b;                            // Domain [a, b]
    double ua, ub;                          // Boundary values
    std::function<double(double)> f;        // Source function

    // Optional: exact solution for testing
    std::function<double(double)> exact_solution = nullptr;
};

// Example problems
namespace TestProblems {

    // Problem 1: Simple diffusion
    // -u'' = 1, u(0) = 0, u(1) = 0
    // Exact: u(x) = x(1-x)/2
    inline BVPProblem simple_diffusion() {
        return {
            .mu = 1.0,
            .c = 0.0,
            .a = 0.0, .b = 1.0,
            .ua = 0.0, .ub = 0.0,
            .f = [](double x) { return 1.0; },
            .exact_solution = [](double x) { return x * (1.0 - x) / 2.0; }
        };
    }

    // Problem 2: Diffusion-reaction
    // -u'' + u = sin(πx), u(0) = 0, u(1) = 0
    inline BVPProblem diffusion_reaction() {
        const double pi = M_PI;
        return {
            .mu = 1.0,
            .c = 1.0,
            .a = 0.0, .b = 1.0,
            .ua = 0.0, .ub = 0.0,
            .f = [pi](double x) { return std::sin(pi * x); },
            .exact_solution = [pi](double x) {
                return std::sin(pi * x) / (pi * pi + 1.0);
            }
        };
    }
}

#endif // PROBLEM_HPP
```

#### Task 1.2: Tridiagonal Solver (Thomas Algorithm)
**File:** `include/tridiagonal_solver.hpp`

```cpp
#ifndef TRIDIAGONAL_SOLVER_HPP
#define TRIDIAGONAL_SOLVER_HPP

#include <vector>
#include <stdexcept>

class TridiagonalSolver {
public:
    // Solve Ax = d where A is tridiagonal
    // a: lower diagonal (size n-1)
    // b: main diagonal (size n)
    // c: upper diagonal (size n-1)
    // d: right-hand side (size n)
    // Returns: solution vector x (size n)

    static std::vector<double> solve(
        const std::vector<double>& a,   // lower diagonal
        const std::vector<double>& b,   // main diagonal
        const std::vector<double>& c,   // upper diagonal
        const std::vector<double>& d    // RHS
    );

    // In-place version (modifies d)
    static void solve_inplace(
        const std::vector<double>& a,
        std::vector<double>& b,         // modified
        const std::vector<double>& c,
        std::vector<double>& d          // becomes solution
    );
};

#endif // TRIDIAGONAL_SOLVER_HPP
```

**File:** `src/tridiagonal_solver.cpp`

```cpp
#include "tridiagonal_solver.hpp"

std::vector<double> TridiagonalSolver::solve(
    const std::vector<double>& a,
    const std::vector<double>& b,
    const std::vector<double>& c,
    const std::vector<double>& d
) {
    size_t n = b.size();

    if (n == 0) throw std::invalid_argument("Empty system");
    if (a.size() != n - 1 || c.size() != n - 1 || d.size() != n) {
        throw std::invalid_argument("Inconsistent sizes");
    }

    // Working copies
    std::vector<double> c_star(n - 1);
    std::vector<double> d_star(n);

    // Forward sweep
    c_star[0] = c[0] / b[0];
    d_star[0] = d[0] / b[0];

    for (size_t i = 1; i < n - 1; ++i) {
        double denom = b[i] - a[i - 1] * c_star[i - 1];
        c_star[i] = c[i] / denom;
        d_star[i] = (d[i] - a[i - 1] * d_star[i - 1]) / denom;
    }

    // Last element
    d_star[n - 1] = (d[n - 1] - a[n - 2] * d_star[n - 2]) /
                   (b[n - 1] - a[n - 2] * c_star[n - 2]);

    // Back substitution
    std::vector<double> x(n);
    x[n - 1] = d_star[n - 1];

    for (int i = n - 2; i >= 0; --i) {
        x[i] = d_star[i] - c_star[i] * x[i + 1];
    }

    return x;
}
```

#### Task 1.3: Local Problem Solver
**File:** `include/local_problem.hpp`

```cpp
#ifndef LOCAL_PROBLEM_HPP
#define LOCAL_PROBLEM_HPP

#include "problem.hpp"
#include "tridiagonal_solver.hpp"
#include <vector>

class LocalProblem {
private:
    int n_local;            // Number of local nodes
    double h;               // Mesh spacing
    double mu, c;           // Problem coefficients

    // Local node positions
    std::vector<double> x_local;

    // Tridiagonal matrix components
    std::vector<double> diag_lower;
    std::vector<double> diag_main;
    std::vector<double> diag_upper;

public:
    LocalProblem(int n_nodes, double x_start, double x_end,
                 double mu, double c);

    // Solve local problem with given boundary conditions
    // f_local: source function values at local nodes
    // bc_left, bc_right: Dirichlet boundary values
    std::vector<double> solve(
        const std::vector<double>& f_local,
        double bc_left,
        double bc_right
    );

    // Getters
    int get_n_local() const { return n_local; }
    double get_x(int j) const { return x_local[j]; }
    const std::vector<double>& get_nodes() const { return x_local; }
};

#endif // LOCAL_PROBLEM_HPP
```

---

### Phase 2: Schwarz Solver (Week 2)

#### Task 2.1: Domain Decomposition Structure
**File:** `include/decomposition.hpp`

```cpp
#ifndef DECOMPOSITION_HPP
#define DECOMPOSITION_HPP

#include <vector>

struct Subdomain {
    int id;                     // Subdomain index
    int global_start;           // First global node index
    int global_end;             // Last global node index
    int n_nodes;                // Number of nodes (including overlap)
    int overlap_left;           // Overlap nodes on left
    int overlap_right;          // Overlap nodes on right

    // Neighbor information
    int left_neighbor;          // -1 if none
    int right_neighbor;         // -1 if none
};

class DomainDecomposition {
private:
    int n_global;               // Total number of nodes
    int n_subdomains;           // Number of subdomains
    int overlap;                // Overlap size (number of elements)
    std::vector<Subdomain> subdomains;

public:
    DomainDecomposition(int n_global, int n_subdomains, int overlap);

    const Subdomain& get_subdomain(int i) const { return subdomains[i]; }
    int get_n_subdomains() const { return n_subdomains; }

    // Map local index to global index
    int local_to_global(int subdomain_id, int local_idx) const;

    // Get non-overlapping range for solution assembly
    std::pair<int, int> get_owned_range(int subdomain_id) const;
};

#endif // DECOMPOSITION_HPP
```

#### Task 2.2: Sequential Schwarz Solver
**File:** `include/schwarz_solver.hpp`

```cpp
#ifndef SCHWARZ_SOLVER_HPP
#define SCHWARZ_SOLVER_HPP

#include "problem.hpp"
#include "local_problem.hpp"
#include "decomposition.hpp"
#include <vector>
#include <functional>

struct SchwarzConfig {
    int n_nodes = 100;          // Global number of nodes
    int n_subdomains = 4;       // Number of subdomains
    int overlap = 2;            // Overlap in elements
    double tolerance = 1e-8;    // Convergence tolerance
    int max_iterations = 1000;  // Maximum iterations
    bool verbose = false;       // Print convergence info
};

struct SchwarzResult {
    std::vector<double> solution;
    std::vector<double> x_nodes;
    int iterations;
    double final_residual;
    bool converged;
    std::vector<double> residual_history;
};

class SchwarzSolver {
private:
    BVPProblem problem;
    SchwarzConfig config;
    DomainDecomposition decomposition;
    std::vector<LocalProblem> local_problems;

    // Local solutions (current and previous iteration)
    std::vector<std::vector<double>> u_local;
    std::vector<std::vector<double>> u_local_prev;

    // Initialize local problems
    void setup();

    // Perform one Schwarz iteration
    void iterate();

    // Check convergence
    double compute_residual() const;

    // Assemble global solution from local solutions
    std::vector<double> assemble_solution() const;

public:
    SchwarzSolver(const BVPProblem& prob, const SchwarzConfig& cfg);

    SchwarzResult solve();

    // Access to configuration
    const SchwarzConfig& get_config() const { return config; }
};

#endif // SCHWARZ_SOLVER_HPP
```

**File:** `src/schwarz_solver.cpp` (partial implementation)

```cpp
#include "schwarz_solver.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

SchwarzSolver::SchwarzSolver(const BVPProblem& prob, const SchwarzConfig& cfg)
    : problem(prob), config(cfg),
      decomposition(cfg.n_nodes, cfg.n_subdomains, cfg.overlap)
{
    setup();
}

void SchwarzSolver::setup() {
    double h = (problem.b - problem.a) / (config.n_nodes - 1);

    // Create local problems for each subdomain
    for (int i = 0; i < config.n_subdomains; ++i) {
        const auto& sub = decomposition.get_subdomain(i);

        double x_start = problem.a + sub.global_start * h;
        double x_end = problem.a + sub.global_end * h;

        local_problems.emplace_back(
            sub.n_nodes, x_start, x_end, problem.mu, problem.c
        );

        // Initialize local solutions to zero
        u_local.push_back(std::vector<double>(sub.n_nodes, 0.0));
        u_local_prev.push_back(std::vector<double>(sub.n_nodes, 0.0));
    }
}

void SchwarzSolver::iterate() {
    // Save previous iteration
    u_local_prev = u_local;

    // Solve each subdomain
    for (int i = 0; i < config.n_subdomains; ++i) {
        const auto& sub = decomposition.get_subdomain(i);
        auto& lp = local_problems[i];

        // Evaluate source function at local nodes
        std::vector<double> f_local(sub.n_nodes);
        for (int j = 0; j < sub.n_nodes; ++j) {
            f_local[j] = problem.f(lp.get_x(j));
        }

        // Get boundary conditions from neighbors (previous iteration)
        double bc_left, bc_right;

        if (sub.left_neighbor < 0) {
            bc_left = problem.ua;  // Physical boundary
        } else {
            // Get from left neighbor's solution
            int neighbor = sub.left_neighbor;
            const auto& neighbor_sub = decomposition.get_subdomain(neighbor);
            int idx = neighbor_sub.n_nodes - 1 - config.overlap;
            bc_left = u_local_prev[neighbor][idx];
        }

        if (sub.right_neighbor < 0) {
            bc_right = problem.ub;  // Physical boundary
        } else {
            // Get from right neighbor's solution
            int neighbor = sub.right_neighbor;
            bc_right = u_local_prev[neighbor][config.overlap];
        }

        // Solve local problem
        u_local[i] = lp.solve(f_local, bc_left, bc_right);
    }
}

double SchwarzSolver::compute_residual() const {
    double max_diff = 0.0;

    for (int i = 0; i < config.n_subdomains; ++i) {
        for (size_t j = 0; j < u_local[i].size(); ++j) {
            double diff = std::abs(u_local[i][j] - u_local_prev[i][j]);
            max_diff = std::max(max_diff, diff);
        }
    }

    return max_diff;
}

SchwarzResult SchwarzSolver::solve() {
    SchwarzResult result;
    result.converged = false;

    for (int iter = 0; iter < config.max_iterations; ++iter) {
        iterate();

        double residual = compute_residual();
        result.residual_history.push_back(residual);

        if (config.verbose) {
            std::cout << "Iteration " << iter + 1
                      << ": residual = " << residual << std::endl;
        }

        if (residual < config.tolerance) {
            result.converged = true;
            result.iterations = iter + 1;
            result.final_residual = residual;
            break;
        }
    }

    if (!result.converged) {
        result.iterations = config.max_iterations;
        result.final_residual = result.residual_history.back();
    }

    result.solution = assemble_solution();

    // Build x_nodes
    double h = (problem.b - problem.a) / (config.n_nodes - 1);
    result.x_nodes.resize(config.n_nodes);
    for (int i = 0; i < config.n_nodes; ++i) {
        result.x_nodes[i] = problem.a + i * h;
    }

    return result;
}
```

---

### Phase 3: Parallel Implementation (Week 3)

#### Task 3.1: MPI Version
**File:** `include/parallel_schwarz.hpp`

```cpp
#ifndef PARALLEL_SCHWARZ_HPP
#define PARALLEL_SCHWARZ_HPP

#include "problem.hpp"
#include "local_problem.hpp"
#include <mpi.h>
#include <vector>

class ParallelSchwarzSolver {
private:
    BVPProblem problem;
    int n_global;
    int overlap;
    double tolerance;
    int max_iterations;

    // MPI info
    MPI_Comm comm;
    int rank;
    int size;

    // Local data (each process owns one subdomain)
    LocalProblem* local_problem;
    std::vector<double> u_local;
    std::vector<double> u_local_prev;

    // Neighbor ranks
    int left_neighbor;
    int right_neighbor;

    // Communication buffers
    double send_left, send_right;
    double recv_left, recv_right;

    void setup();
    void exchange_boundaries();
    void iterate();
    double compute_global_residual();

public:
    ParallelSchwarzSolver(const BVPProblem& prob, int n_nodes,
                          int overlap, double tol, int max_iter,
                          MPI_Comm comm = MPI_COMM_WORLD);
    ~ParallelSchwarzSolver();

    std::vector<double> solve(int& iterations, bool& converged);

    // Gather solution on rank 0
    std::vector<double> gather_solution();
};

#endif // PARALLEL_SCHWARZ_HPP
```

**File:** `src/parallel_schwarz.cpp` (key methods)

```cpp
#include "parallel_schwarz.hpp"
#include <algorithm>
#include <cmath>

void ParallelSchwarzSolver::exchange_boundaries() {
    MPI_Request requests[4];
    MPI_Status statuses[4];
    int n_requests = 0;

    // Prepare data to send
    if (left_neighbor >= 0) {
        send_left = u_local[overlap];  // Value to send left
        MPI_Isend(&send_left, 1, MPI_DOUBLE, left_neighbor, 0,
                  comm, &requests[n_requests++]);
        MPI_Irecv(&recv_left, 1, MPI_DOUBLE, left_neighbor, 1,
                  comm, &requests[n_requests++]);
    }

    if (right_neighbor >= 0) {
        int n = u_local.size();
        send_right = u_local[n - 1 - overlap];  // Value to send right
        MPI_Isend(&send_right, 1, MPI_DOUBLE, right_neighbor, 1,
                  comm, &requests[n_requests++]);
        MPI_Irecv(&recv_right, 1, MPI_DOUBLE, right_neighbor, 0,
                  comm, &requests[n_requests++]);
    }

    // Wait for all communications
    MPI_Waitall(n_requests, requests, statuses);
}

void ParallelSchwarzSolver::iterate() {
    // Save previous solution
    u_local_prev = u_local;

    // Exchange boundary values with neighbors
    exchange_boundaries();

    // Determine boundary conditions
    double bc_left = (left_neighbor < 0) ? problem.ua : recv_left;
    double bc_right = (right_neighbor < 0) ? problem.ub : recv_right;

    // Evaluate source function
    std::vector<double> f_local(u_local.size());
    for (size_t j = 0; j < u_local.size(); ++j) {
        f_local[j] = problem.f(local_problem->get_x(j));
    }

    // Solve local problem
    u_local = local_problem->solve(f_local, bc_left, bc_right);
}

double ParallelSchwarzSolver::compute_global_residual() {
    // Compute local max difference
    double local_max = 0.0;
    for (size_t j = 0; j < u_local.size(); ++j) {
        double diff = std::abs(u_local[j] - u_local_prev[j]);
        local_max = std::max(local_max, diff);
    }

    // Global reduction
    double global_max;
    MPI_Allreduce(&local_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, comm);

    return global_max;
}
```

#### Task 3.2: OpenMP Version
**File:** `include/openmp_schwarz.hpp`

```cpp
#ifndef OPENMP_SCHWARZ_HPP
#define OPENMP_SCHWARZ_HPP

#include "problem.hpp"
#include "local_problem.hpp"
#include <vector>
#include <omp.h>

class OpenMPSchwarzSolver {
private:
    BVPProblem problem;
    SchwarzConfig config;

    std::vector<LocalProblem> local_problems;
    std::vector<std::vector<double>> u_local;
    std::vector<std::vector<double>> u_local_prev;

    // Boundary values for communication
    std::vector<double> left_bc;
    std::vector<double> right_bc;

    void setup();

public:
    OpenMPSchwarzSolver(const BVPProblem& prob, const SchwarzConfig& cfg);

    SchwarzResult solve();
};

#endif // OPENMP_SCHWARZ_HPP
```

**Key parallel loop:**

```cpp
void OpenMPSchwarzSolver::iterate() {
    // Copy boundary conditions (sequential - small operation)
    #pragma omp single
    {
        for (int i = 0; i < config.n_subdomains; ++i) {
            // ... prepare boundary conditions from u_local_prev
        }
    }

    // Parallel solve
    #pragma omp parallel for
    for (int i = 0; i < config.n_subdomains; ++i) {
        // Evaluate local source
        std::vector<double> f_local(local_problems[i].get_n_local());
        for (int j = 0; j < local_problems[i].get_n_local(); ++j) {
            f_local[j] = problem.f(local_problems[i].get_x(j));
        }

        // Solve local problem
        u_local[i] = local_problems[i].solve(f_local, left_bc[i], right_bc[i]);
    }
}
```

---

### Phase 4: Preconditioner (Week 4)

#### Task 4.1: Schwarz Preconditioner Interface
**File:** `include/preconditioner.hpp`

```cpp
#ifndef PRECONDITIONER_HPP
#define PRECONDITIONER_HPP

#include "schwarz_solver.hpp"
#include <vector>

// Abstract preconditioner interface
class Preconditioner {
public:
    virtual ~Preconditioner() = default;

    // Apply preconditioner: z = P^{-1} r
    virtual std::vector<double> apply(const std::vector<double>& r) = 0;
};

// Schwarz preconditioner (one iteration)
class SchwarzPreconditioner : public Preconditioner {
private:
    BVPProblem problem;
    SchwarzConfig config;
    DomainDecomposition decomposition;
    std::vector<LocalProblem> local_problems;

public:
    SchwarzPreconditioner(const BVPProblem& prob, const SchwarzConfig& cfg);

    std::vector<double> apply(const std::vector<double>& r) override;
};

// Two-level preconditioner with coarse correction
class TwoLevelPreconditioner : public Preconditioner {
private:
    SchwarzPreconditioner schwarz;

    // Coarse grid data
    int n_coarse;
    std::vector<double> coarse_nodes;
    // ... coarse matrix

public:
    TwoLevelPreconditioner(const BVPProblem& prob, const SchwarzConfig& cfg);

    std::vector<double> apply(const std::vector<double>& r) override;
};

#endif // PRECONDITIONER_HPP
```

#### Task 4.2: Preconditioned Iterative Solver
**File:** `include/iterative_solver.hpp`

```cpp
#ifndef ITERATIVE_SOLVER_HPP
#define ITERATIVE_SOLVER_HPP

#include "preconditioner.hpp"
#include <vector>
#include <functional>

// Matrix-free interface
using MatVecProduct = std::function<std::vector<double>(const std::vector<double>&)>;

struct IterativeSolverResult {
    std::vector<double> solution;
    int iterations;
    double final_residual;
    bool converged;
};

// Preconditioned Richardson iteration
IterativeSolverResult richardson_solve(
    MatVecProduct A,
    const std::vector<double>& b,
    Preconditioner& P,
    double alpha,
    double tolerance,
    int max_iterations
);

// Preconditioned Conjugate Gradient (for SPD systems)
IterativeSolverResult pcg_solve(
    MatVecProduct A,
    const std::vector<double>& b,
    Preconditioner& P,
    double tolerance,
    int max_iterations
);

#endif // ITERATIVE_SOLVER_HPP
```

---

### Phase 5: Testing and Visualization (Week 5)

#### Task 5.1: Unit Tests
**File:** `tests/test_schwarz.cpp`

```cpp
#include "schwarz_solver.hpp"
#include "problem.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

void test_simple_diffusion() {
    auto problem = TestProblems::simple_diffusion();

    SchwarzConfig config;
    config.n_nodes = 101;
    config.n_subdomains = 4;
    config.overlap = 2;
    config.tolerance = 1e-10;
    config.verbose = true;

    SchwarzSolver solver(problem, config);
    auto result = solver.solve();

    assert(result.converged);

    // Check against exact solution
    double max_error = 0.0;
    for (size_t i = 0; i < result.solution.size(); ++i) {
        double exact = problem.exact_solution(result.x_nodes[i]);
        double error = std::abs(result.solution[i] - exact);
        max_error = std::max(max_error, error);
    }

    std::cout << "Max error: " << max_error << std::endl;
    std::cout << "Iterations: " << result.iterations << std::endl;

    assert(max_error < 1e-4);  // Should be small
}

void test_convergence_vs_overlap() {
    auto problem = TestProblems::simple_diffusion();

    std::cout << "\nOverlap vs Iterations:\n";
    std::cout << "Overlap\tIterations\n";

    for (int overlap = 1; overlap <= 10; ++overlap) {
        SchwarzConfig config;
        config.n_nodes = 101;
        config.n_subdomains = 4;
        config.overlap = overlap;
        config.tolerance = 1e-8;

        SchwarzSolver solver(problem, config);
        auto result = solver.solve();

        std::cout << overlap << "\t" << result.iterations << std::endl;
    }
}

int main() {
    test_simple_diffusion();
    test_convergence_vs_overlap();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
```

#### Task 5.2: Visualization Script
**File:** `scripts/plot_results.py`

```python
#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import sys

def plot_solution(filename):
    """Plot numerical solution vs exact (if available)"""
    data = np.loadtxt(filename)
    x = data[:, 0]
    u_numerical = data[:, 1]

    plt.figure(figsize=(10, 6))
    plt.plot(x, u_numerical, 'b-', linewidth=2, label='Numerical')

    if data.shape[1] > 2:
        u_exact = data[:, 2]
        plt.plot(x, u_exact, 'r--', linewidth=2, label='Exact')

    plt.xlabel('x')
    plt.ylabel('u(x)')
    plt.title('Domain Decomposition Solution')
    plt.legend()
    plt.grid(True)
    plt.savefig('solution.png', dpi=150)
    plt.show()

def plot_convergence(filename):
    """Plot convergence history"""
    residuals = np.loadtxt(filename)

    plt.figure(figsize=(10, 6))
    plt.semilogy(residuals, 'b-o', markersize=4)
    plt.xlabel('Iteration')
    plt.ylabel('Residual (log scale)')
    plt.title('Schwarz Iteration Convergence')
    plt.grid(True)
    plt.savefig('convergence.png', dpi=150)
    plt.show()

def plot_scaling(filename):
    """Plot parallel scaling results"""
    data = np.loadtxt(filename)
    procs = data[:, 0]
    times = data[:, 1]

    plt.figure(figsize=(10, 6))

    # Actual speedup
    speedup = times[0] / times
    plt.subplot(1, 2, 1)
    plt.plot(procs, speedup, 'b-o', label='Actual')
    plt.plot(procs, procs, 'r--', label='Ideal')
    plt.xlabel('Number of Processors')
    plt.ylabel('Speedup')
    plt.title('Strong Scaling')
    plt.legend()
    plt.grid(True)

    # Efficiency
    efficiency = speedup / procs * 100
    plt.subplot(1, 2, 2)
    plt.plot(procs, efficiency, 'g-o')
    plt.xlabel('Number of Processors')
    plt.ylabel('Efficiency (%)')
    plt.title('Parallel Efficiency')
    plt.grid(True)

    plt.tight_layout()
    plt.savefig('scaling.png', dpi=150)
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python plot_results.py <type> <filename>")
        print("  type: solution, convergence, scaling")
        sys.exit(1)

    plot_type = sys.argv[1]
    filename = sys.argv[2]

    if plot_type == 'solution':
        plot_solution(filename)
    elif plot_type == 'convergence':
        plot_convergence(filename)
    elif plot_type == 'scaling':
        plot_scaling(filename)
```

---

### Phase 6: Documentation and Final Testing (Week 6)

#### Task 6.1: CMakeLists.txt
**File:** `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)
project(DomainDecomposition CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(USE_MPI "Enable MPI support" ON)
option(USE_OPENMP "Enable OpenMP support" ON)

# Find packages
if(USE_MPI)
    find_package(MPI REQUIRED)
endif()

if(USE_OPENMP)
    find_package(OpenMP REQUIRED)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/tridiagonal_solver.cpp
    src/local_problem.cpp
    src/schwarz_solver.cpp
    src/decomposition.cpp
)

# Main library
add_library(dd_lib ${SOURCES})

if(USE_OPENMP)
    target_link_libraries(dd_lib OpenMP::OpenMP_CXX)
endif()

# Sequential example
add_executable(example_sequential examples/example_sequential.cpp)
target_link_libraries(example_sequential dd_lib)

# MPI example
if(USE_MPI)
    add_executable(example_mpi examples/example_mpi.cpp src/parallel_schwarz.cpp)
    target_link_libraries(example_mpi dd_lib MPI::MPI_CXX)
endif()

# OpenMP example
if(USE_OPENMP)
    add_executable(example_openmp examples/example_openmp.cpp)
    target_link_libraries(example_openmp dd_lib OpenMP::OpenMP_CXX)
endif()

# Tests
enable_testing()
add_executable(test_all tests/test_tridiagonal.cpp tests/test_schwarz.cpp)
target_link_libraries(test_all dd_lib)
add_test(NAME AllTests COMMAND test_all)
```

---

## 3. Timeline Summary

| Week | Phase | Tasks |
|------|-------|-------|
| **1** | Core Components | Problem definition, Thomas algorithm, Local solver |
| **2** | Schwarz Solver | Domain decomposition, Sequential Schwarz |
| **3** | Parallelization | MPI version, OpenMP version |
| **4** | Preconditioner | Schwarz preconditioner, Two-level method |
| **5** | Testing | Unit tests, Convergence tests, Visualization |
| **6** | Finalization | Documentation, Performance analysis, Report |

---

## 4. Milestones and Deliverables

### Milestone 1 (End of Week 2)
- [ ] Working sequential Schwarz solver
- [ ] Verified against exact solution
- [ ] Basic convergence tests

### Milestone 2 (End of Week 4)
- [ ] MPI parallel version
- [ ] OpenMP parallel version
- [ ] Scaling tests (strong/weak)

### Milestone 3 (End of Week 6)
- [ ] Preconditioner implementation
- [ ] Complete test suite
- [ ] Documentation and report
- [ ] Performance analysis

---

## 5. Extensions (Exam Project Options)

### Option A: 2D Extension
- Extend to 2D rectangular domains
- 2D domain partitioning (stripes or checkerboard)
- Use sparse matrix library (Eigen)

### Option B: Advanced Preconditioner
- Implement coarse grid correction
- Algebraic multigrid-style agglomeration
- Compare with basic Schwarz

### Option C: GPU Implementation
- CUDA implementation
- Batched tridiagonal solves
- Performance comparison CPU vs GPU

---

## 6. References

1. Formaggia, L. (2024). *Domain decomposition for PDEs*. Lecture notes.
2. Formaggia, L., Sala, M., & Saleri, F. (2006). *Domain Decomposition Techniques*.
3. Smith, B., Bjorstad, P., & Gropp, W. (1996). *Domain Decomposition*.
4. Quarteroni, A., & Valli, A. (1999). *Domain Decomposition Methods for PDEs*.

---

*Last updated: November 2024*
