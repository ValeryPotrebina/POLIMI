# Direct solvers (SparseLU and SparseLDLT) in Eigen

The goal of this lab is to test the sparse direct solvers available in the Eigen library and compare them with respect to the iterative methods we implemented in the previous labs. 

The native Eigen direct solvers are based on the Choleski and LU factorization in the case in which the input matrix is symmetric or not, respectively. The syntax for calling the `SparseLU` and `SimplicialLDLT` Eigen functions is similar to the one used for the iterative solvers `ConjugateGradient` and `BICGSTAB`:

```
  Eigen::SparseLU<Eigen::SparseMatrix<double> > solver;    // define solver
  solvelu.compute(A);
  if(solvelu.info()!=Eigen::Success) {                     // sanity check
      std::cout << "cannot factorize the matrix" << std::endl; 
      return 0;
  }
  x = solvelu.solve(b);                                    // solve
```


## Exercise 1

Let us consider the Hilbert matrix of size $n=100$ defined as follows:
$$
H_{i,j} = \frac1{i+j+1} \quad \forall 1\le i,j\le 100.
$$

We want to compare the `SparseLU` and `SimplicialLDLT` direct solvers with respect to the hand-made Conjugate Gradient method for solving the linear system $Ax = b$, where $b$ is obtained by taking $x = (1,1,\ldots, 1)$ as exact solution.

Since the conditioning number of the Hilbert matrix is very large, we observe that the direct solvers does not work. The resulting error is of order $10^3$.
Instead, the CG method is able to compute a good approximation in only 13 iterations.

```
#include <cstdlib>                      // System includes
#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include "cg.hpp"

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  int n = 100;
  SpMat A(n,n);                       // define Hilbert matrix
  for (int i=0; i<n; i++) {
      for (int j=0; j<n; j++){
          A.coeffRef(i, j) = 1.0/(i+j+1);
      }
  }

  double tol = 1.e-8;                  // Convergence tolerance
  int result, maxit = 1000;            // Maximum iterations for CG
  SpMat B = SpMat(A.transpose()) - A;  // Check symmetry
  std::cout<<"Norm of A-A.t: "<<B.norm()<<std::endl;
  // Create Rhs b
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());
  Eigen::DiagonalPreconditioner<double> D(A);

  // First with Eigen Choleski direct solver
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver;  // LDLT factorization
  solver.compute(A);
  if(solver.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl; // decomposition failed
      return 0;
  }

  x = solver.solve(b);                                         // solving
  std::cout << "Solution with Eigen Choleski:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // Then with Eigen SparseLU solver
  Eigen::SparseLU<Eigen::SparseMatrix<double> > solvelu;        // LU factorization
  solvelu.compute(A);
  if(solvelu.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl;  // decomposition failed
      return 0;
  }

  x = solvelu.solve(b);                                         // solving
  std::cout << "Solution with Eigen LU:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // Finally with hand-made CG
  x=0*x;
  result = CG(A, x, b, D, maxit, tol);                     // Call CG function
  std::cout << "Solution with Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl;
  std::cout << "Error norm: "<<(x-e).norm()<< std::endl;

  return result;
}
```

## Exercise 2

In this exercise we consider a symmetric square matrix of size $n=1000$ obtained as the sum of the tridiagonal matrix (arising when discretizing a 1D laplacian using finite differences) and an Hilbert matrix of size $20\times 20$, namely $A = B + H$ with
$$
B_{i,i} = 2,\quad B_{i,i+1} = B_{i,i-1} = -1, \quad B_{i,j} = 0 \quad \forall j\neq i-1, i, i+1, \forall 1\le i\le n,
$$
$$
\text{and} \quad H_{i,j} = \frac1{i+j+1} \quad \forall 1\le i,j\le 20.
$$

We want to compare the `SparseLU` and `SimplicialLDLT` direct solvers with respect to the hand-made Conjugate Gradient method for solving the linear system $Ax = b$, where $b$ is obtained by taking $x = (1,1,\ldots, 1)$ as exact solution.

We observe that in this case the direct solvers works better than the CG method, that requires almost 1000 of iterations to converge and returns a larger error.

```
#include <cstdlib>                      // System includes
#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include "cg.hpp"

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  int n = 1000;
  SpMat A(n,n);                       // define Hilbert matrix
  for (int i=0; i<20; i++) {
      for (int j=0; j<20; j++){
          A.coeffRef(i, j) = 1.0/(i+j+1);
      }
  }
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) += 2.0;
      if (i < n-1) A.coeffRef(i, i+1) += -1.0;
      if (i > 0) A.coeffRef(i,i-1) += -1.0;
  }

  double tol = 1.e-9;                 // Convergence tolerance
  int result, maxit = 999;            // Maximum iterations for CG
  SpMat B = SpMat(A.transpose()) - A;  // Check symmetry
  std::cout<<"Norm of A-A.t: "<<B.norm()<<std::endl;

  // Create Rhs b
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());
  Eigen::DiagonalPreconditioner<double> D(A);

  // First with Eigen Choleski direct solver
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;  // LDLT factorization
  solver.compute(A);
  if(solver.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl;
      return 0;
  }
  x = solver.solve(b);                                         // solving
  std::cout << "Solution with Eigen Choleski:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // Then with Eigen SparseLU solver
  Eigen::SparseLU<Eigen::SparseMatrix<double> > solvelu;        // LU factorization
  solvelu.compute(A);
  if(solvelu.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl;  // decomposition failed
      return 0;
  }

  x = solvelu.solve(b);                                         // solving
  std::cout << "Solution with Eigen LU:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // Now with hand-made CG
  x=0*x;
  result = CG(A, x, b, D, maxit, tol);              // Call CG function
  std::cout << "Solution with Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl;
  std::cout << "Error norm: "<<(x-e).norm()<< std::endl;

  return result;
}
```

## Exercise 3

Download the matrices `navierstokes_mat.mtx` and `navierstokes_sym.mtx` from the webeep folder "Codes - Labs". 

We want to compare the `SparseLU` and `SimplicialLDLT` direct solvers with respect to the hand-made `cg` and `cgs` iterative methods for the linear system $Ax = b$, where $A$ corresponds to the downloaded matrices, while $b$ is obtained by taking $x = (1,1,\ldots, 1)$ as exact solution.

To avoid multiple compiling for changes in the input matrix, we provide the matrix file name as an input the the main program.

We observe that all the methods works when tested with the `navierstokes_sym.mtx` and both the $LU$ and $LDL^{\rm T}$ give a good precision. On the other hand, the `SimplicialLDLT` Eigen function does not work for the linear system defined by the non-symmetric `navierstokes_mat.mtx`. Indeed, as expected from the theory, the Choleski factorization works fine only for SPD matrices.

```
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <iostream>
#include <string>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/SparseExtra>

#include "cg.hpp"
#include "cgs.hpp"

int main(int argc, char *argv[]){
  using namespace Eigen;
  using namespace LinearAlgebra;

  if(argc != 2)
    {
      std::cerr << " Usage: provide matrix filename" << std::endl;
      return 1;
    }
  std::string matrixFile(argv[1]);
  // Some useful alias
  using SpMat = Eigen::SparseMatrix<double>;
  using SpVec = Eigen::VectorXd;

  // Read matrix
  SpMat A;
  Eigen::loadMarket(A, matrixFile);
  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<std::endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<std::endl;

  double tol = 1.e-8;                  // Convergence tolerance
  int result, maxit = 2000;            // Maximum iterations for CGS
  SpMat B = SpMat(A.transpose()) - A;  // Check symmetry
  std::cout<<"Norm of A-A.t: "<<B.norm()<<std::endl;

  // Create Rhs b
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());
  Eigen::DiagonalPreconditioner<double> D(A);

  // First with Eigen Choleski direct solver
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;   // LDLT factorization
  solver.compute(A);
  if(solver.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl;
      return 0;
  }
  x = solver.solve(b);                                         // solving
  std::cout << "Solution with Eigen Choleski:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // Then with Eigen SparseLU solver
  Eigen::SparseLU<Eigen::SparseMatrix<double> > solvelu;        // LU factorization
  solvelu.compute(A);
  if(solvelu.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl;  // decomposition failed
      return 0;
  }

  x = solvelu.solve(b);                                         // solving
  std::cout << "Solution with Eigen LU:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // Now with hand-made CGS method
  x=0*x;
  // result = CG(A,x,b,D,maxit,tol);
  result = CGS(A, x, b, D, maxit, tol);          // Call CG - CGS function
  std::cout << "Solution with (Squared) Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl;
  std::cout << "Error norm: "<<(x-e).norm()<< std::endl;

  return result;
}
```

## Exercise 4

In this exercise we construct block preconditioner for iterative solvers in Eigen. The preconditioned is built by using direct solvers (such as LU, QR, or Choleski factorization) to approximate the inverse of a block of the matrix $A$ defining the linear system $Ax = b$, where $b = (1,1,\ldots, 1)$.
```
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <iostream>
#include <string>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/SparseExtra>
#include "cg.hpp"

int main(int argc, char *argv[]){
  using namespace Eigen;
  using namespace LinearAlgebra;

  if(argc != 2)
    {
      std::cerr << " Usage: provide matrix filename" << std::endl;
      return 1;
    }
  std::string matrixFileA(argv[1]);
  // Some useful alias
  using SpMat = Eigen::SparseMatrix<double>;
  using SpVec = Eigen::VectorXd;

  // Read matrix and check properties
  SpMat A;
  Eigen::loadMarket(A, matrixFileA);
  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<std::endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<std::endl;
  SpMat B = SpMat(A.transpose()) - A;  // Check symmetry
  std::cout << "Norm of A-A.t: " << B.norm() << std::endl << std::endl;

  // Create Rhs b
  SpVec b = SpVec::Ones(A.rows());
  SpVec x(A.rows());
  SpVec y(A.rows());

  // Solution with hand-made CG method
  double tol = 1.e-10;                  // Convergence tolerance
  int result, maxit = 1000;             // Maximum iterations

  MatrixXd I = MatrixXd::Identity(A.rows(),A.cols());
  Eigen::DiagonalPreconditioner<double> Id(I); // preconditioner
  result = CG(A,x,b,Id,maxit,tol);
  std::cout << "Solution with Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl << std::endl;

  // Compute block preconditioner with Eigen Choleski direct solver
  int m = 200;
  I.topLeftCorner(A.rows()-m, A.cols()-m) = A.topLeftCorner(A.rows()-m,A.cols()-m);
  SpMat P = I.sparseView();

  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
  solver.compute(P);
  if(solver.info()!=Eigen::Success) {                          // sanity check
      std::cout << "cannot factorize the matrix" << std::endl;
      return 0;
  }
  y = solver.solve(b);                                         // solving
  std::cout << "Solution with Eigen Choleski:" << std::endl;
  std::cout << "relative residual: "<<(P*y-b).norm()<< std::endl << std::endl;

  // Solution with preconditioned CG method
  SpVec x2(A.rows()); tol = 1.e-10; maxit = 1000;
  result = CG(A,x2,b,solver,maxit,tol);
  std::cout << "Solution with precond Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl;
  std::cout << "comparison solutions : " << (x-x2).norm() << std::endl;

  return result;
}
```