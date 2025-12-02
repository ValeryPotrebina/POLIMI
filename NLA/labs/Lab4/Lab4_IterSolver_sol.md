# Iterative solvers with Eigen

In this lab we aim at evaluating some hand-made implementation of the most common iterative methods for solving linear systems. 

## Hand-made Conjugate Gradient method

First, we create a new directory called `iter_sol++` and we copy the following implementation of the Conjugate Gradient method.

```
namespace LinearAlgebra
{
template <class Matrix, class Vector, class Preconditioner>
int CG(const Matrix &A, Vector &x, const Vector &b, const Preconditioner &M,
   int &max_iter, typename Vector::Scalar &tol)
{
  using Real = typename Matrix::Scalar;
  Real   resid;
  Vector p(b.size());
  Vector z(b.size());
  Vector q(b.size());
  Real   alpha, beta, rho;
  Real   rho_1(0.0);

  Real   normb = b.norm();
  Vector r = b - A * x;

  if(normb == 0.0)
    normb = 1;

  if((resid = r.norm() / normb) <= tol)
    {
      tol = resid;
      max_iter = 0;
      return 0;
    }

  for(int i = 1; i <= max_iter; i++)
    {
      z = M.solve(r);
      rho = r.dot(z);

      if(i == 1)
        p = z;
      else
        {
          beta = rho / rho_1;
          p = z + beta * p;
        }

      q = A * p;
      alpha = rho / p.dot(q);

      x += alpha * p;
      r -= alpha * q;

      if((resid = r.norm() / normb) <= tol)
        {
          tol = resid;
          max_iter = i;
          return 0;
        }

      rho_1 = rho;
    }

  tol = resid;
  return 1;
}
} // namespace LinearAlgebra
```

### Exercise 1: Test the CG method

In order to test the hand-made CG implementation, we want to compare it with the built-in `Eigen::ConjugateGradient` solver. First in the preamble of the `test_cg.cpp` file we include the required module:

```
#include <cstdlib>                      // System includes
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>

using std::endl;
using std::cout;

#include "cg.hpp"                       // Include the template cg
```

We test the conjugate gradient method on a tri-diagonal linear system assuming that the exact solution as all the coefficients equal to 1. The matrix `A` is built as follows:

```
int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  int n = 1000;
  SpMat A(n,n);                       // define matrix
  A.reserve(2998);
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) = 2.0*(i+1);
      if(i>0) A.coeffRef(i, i-1) = -i;
      if(i<n-1) A.coeffRef(i, i+1) = -(i+1);
  }
```

Then, we set the parameters for the conjugate gradient routine (desired tolerance, initial guess, and maximum number of iterations allowed for convergence). As precoditioner we take the simple diagonal preconditioner provided by Eigen (also called the Jacobi preconditioner).

```
double tol = 1.e-10;                // Convergence tolerance
  int result, maxit = 1000;           // Maximum iterations

  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<endl;

  SpMat B = SpMat(A.transpose()) - A;  // Check symmetry
  std::cout<<"Norm of A-A.t: "<<B.norm()<<endl;

  // Create Rhs b
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());
  Eigen::DiagonalPreconditioner<double> D(A); // Create diagonal preconditioner
```

We call both the `Eigen::ConjugateGradient` solver and the `CG` class template. Finally, we display and compare the results.

```
  // First with eigen CG
  Eigen::ConjugateGradient<SpMat, Eigen::Lower|Eigen::Upper> cg;
  cg.setMaxIterations(maxit);
  cg.setTolerance(tol);
  cg.compute(A);
  x = cg.solve(b);
  std::cout <<" Eigen native CG"<< endl;
  std::cout << "#iterations:     " << cg.iterations() << endl;
  std::cout << "estimated error: " << cg.error()      << endl;
  std::cout << "effective error: "<<(x-e).norm()<< endl;

  // Now with hand-made CG
  x=0*x;
  result = CG(A, x, b, D, maxit, tol);        // Solve system

  std::cout <<" hand-made CG "<< endl;
  cout << "CG flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;

  return result;
}
```

Since the convergence history is the same for the two implementation, we can conclude that our hand-made implementation is equivalent to the one available in Eigen. 

### Exercise 2 

- Export the matrix constructed in the previous example using the `SparseExtra` module and repeat the previous test using the LIS library.

- Try different preconditioning techniques (`jacobi`, `ssor`, `ilu`, ...)


### Solution

Move the matrix in the LIS test folder `lis-2.0.34/test/`. Then load the LIS module and compile test1.c as following

```
module load lis
mpicc -DUSE_MPI -I${mkLisInc} -L${mkLisLib} -llis test1.c -o test1
```

Compare the solution with and without preconditioners:

```
mpirun -n 4 ./test1 AtestCG.mtx 2 sol.mtx hist.txt -i cg 
mpirun -n 4 ./test1 AtestCG.mtx 2 sol.mtx hist.txt -i cg -p jacobi

mpirun -n 4 ./test1 AtestCG.mtx 2 sol.mtx hist.txt -i cg -p ssor 
mpirun -n 4 ./test1 AtestCG.mtx 2 sol.mtx hist.txt -i cg -p ssor -ssor_omega 0.8

mpirun -n 4 ./test1 AtestCG.mtx 2 sol.mtx hist.txt -i cg -p ilu 
./test1 AtestCG.mtx 2 sol.mtx hist.txt -i cg -p ilu
```

## Hand-made Jacobi iterative solver

In the folder `iter_sol++` we create a new file called `jacobi.hpp` where we aim to write the implementation of the Jacobi iterative method for solving linear system as a `c++` class template. The preamble and input parameter is the same as the one for the `cg.hpp` function. We exploit the built-in Eigen diagonal preconditioner to define the Jacobi iteration.

```
namespace LinearAlgebra
{
template <class Matrix, class Vector, class Preconditioner>
int Jacobi(const Matrix &A, Vector &x, const Vector &b, const Preconditioner &M,
   int &max_iter, typename Vector::Scalar &tol)
{
  using Real = typename Matrix::Scalar;
  Real   resid;
  Real   normb = b.norm();
  Vector r = b - A * x;

  if(normb == 0.0) normb = 1;
  if((resid = r.norm() / normb) <= tol)
    {
      tol = resid;
      max_iter = 0;
      return 0;
    }

  for(int i = 1; i <= max_iter; i++)
    {
      x = M.solve(r) + x;
      r = b - A * x;
      if((resid = r.norm() / normb) <= tol)
        {
          tol = resid;
          max_iter = i;
          return 0;
        }
    }

  tol = resid;
  return 1;
}
} // namespace LinearAlgebra
```

### Exercise 3: Test the Jacobi method

We evaluate the implemented Jacobi iterative method on the usual tridiagonal square matrix corresponding to a finite difference discretization of the 1D Laplacian. We compare the performance of the Jacobi method with respect to the conjugate gradient. 

```
#include <cstdlib>                      // System includes
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>

using std::endl;
using std::cout;

#include "cg.hpp"
#include "jacobi.hpp"

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  int n = 100;
  SpMat A(n,n);                      // define matrix
  A.reserve(298);
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) = 2.0;
      if(i>0) A.coeffRef(i, i-1) = -1.0;
      if(i<n-1) A.coeffRef(i, i+1) = -1.0;
  }

  double tol = 1.e-6;                // Convergence tolerance
  int result, maxit = 100;          // Maximum iterations

  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<std::endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<std::endl;
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());

  // Solve with CG method
  x=0*x;
  Eigen::DiagonalPreconditioner<double> D(A);// Create diagonal preconditioner
  result = CG(A, x, b, D, maxit, tol);
  cout << "CG   flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  cout << "Error:                " << (x-e).norm()<< endl;

  // Solve with Jacobi method
  x=0*x; maxit = 20000; tol = 1.e-5;
  result = Jacobi(A, x, b, D, maxit, tol);
  cout << "Jacobi flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  cout << "Error:                " << (x-e).norm()<< endl;

  return 0;
}
```

We can observe that the Jacobi scheme requires many more iteration compared to the CG Krylov solver. 

## Other iterative solvers for non-symmetric systems

Carefully read the implementation of the CGS, GMRES, and BiCGSTAB respectively in the implemented `cgs.hpp`, `gmres.hpp`, and `bcgstab.hpp`functions. We mention that the Conjugate Gradient Squared (CGS) can be seen as a preconditioned Conjugate Gradient were the preconditioner is taken as $A^T$ in order to end up with the symmetric linear system $$A^T\ A \boldsymbol{x} = A^T \boldsymbol{b}.$$

### Exercise 4: Compare the CGS, GMRES, and BiCGSTAB solvers

Compare the iterative linear solvers on the non-symmetric square matrix of the `test5.c` file availble on the LIS library. For the CGS method we can use the Squared Diagonal preconditioner provided by Eigen, whereas for the two other methods we adopt again the usual Jacobi preconditioner. 

```
// ... usual include here ...

#include "cgs.hpp"
#include "bcgstab.hpp"
#include "gmres.hpp"

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  int n = 1000;
  double gam = -0.5;
  SpMat A(n,n);                      // define matrix
  A.reserve(2997);
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) = 2.0;
      if(i>1) A.coeffRef(i, i-2) = gam;
      if(i<n-1) A.coeffRef(i, i+1) = 1.0;
  }

  double tol = 1.e-8;                // Convergence tolerance
  int result, maxit = 100;           // Maximum iterations
  int restart = 30;                 // Restart for gmres

  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<std::endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<std::endl;
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());
  Eigen::LeastSquareDiagonalPreconditioner<double> SD(A);

  // Solve with CGS method
  x=0*x;
  result = CGS(A, x, b, SD, maxit, tol);
  cout << "CGS   flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  cout << "Error:                " << (x-e).norm()<< endl;

  // Solve with BiCGSTAB method
  x=0*x; maxit = 100; tol = 1.e-8;
  Eigen::DiagonalPreconditioner<double> D(A);
  result = BiCGSTAB(A, x, b, D, maxit, tol);
  cout << "BiCGSTAB   flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  cout << "Error:                " << (x-e).norm()<< endl;

  // Solve with GMRES method
  x=0*x; maxit = 100; tol = 1.e-8;
  result = GMRES(A, x, b, D, restart, maxit, tol);
  cout << "GMRES   flag = " << result << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  cout << "Error:                " << (x-e).norm()<< endl;

  return 0;
}
```

In the previous example, the results with the three methods are very similar. The GMRES scheme requires a few more iterations to reach the desired tolerance.


### Exercise: implement the Gradient method and assess its performance 

```
#include <cstdlib>                      // System includes
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>

using std::endl;
using std::cout;

#include "grad.hpp"
#include "gmres.hpp"

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double,Eigen::RowMajor>;
  using SpVec=Eigen::VectorXd;

  int n = 400;
  SpMat A(n,n);                       // define matrix
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) = 2.0*(i+1);
      if(i>0) A.coeffRef(i, i-1) -= i;
      if(i<n-1) A.coeffRef(i, i+1) -= (i+1);
  }

  double tol = 1.e-8;                  // Convergence tolerance
  int result, maxit = 10000;           // Maximum iterations

  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<endl;

  // Create Rhs b
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());
  Eigen::DiagonalPreconditioner<double> D(A);

  // First with Eigen CG
  Eigen::ConjugateGradient<SpMat, Eigen::Lower|Eigen::Upper> cg;
  cg.setMaxIterations(maxit);
  cg.setTolerance(tol);
  cg.compute(A);
  x = cg.solve(b);
  std::cout <<" Eigen CG" << endl;
  std::cout << "#iterations:     " << cg.iterations() << endl;
  std::cout << "relative residual: " << cg.error()      << endl;
  std::cout << "absolute error: "<<(x-e).norm()<< endl;

  // with Gradient Method
  x=0*x; tol = 1.e-8;
  result = GRAD(A, x, b, D, maxit, tol);   
  std::cout <<" Gradient method " << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "relative residual  : " << tol << endl;
  std::cout << "absolute error: "<<(x-e).norm()<< endl;

  // Solve with GMRES method
  x=0*x; maxit = 1000; tol = 1.e-8;
  int restart = 200;
  result = GMRES(A, x, b, D, restart, maxit, tol);
  cout << " GMRES method " << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "relative residual  : " << tol << endl;
  cout << "absolute error:      " << (x-e).norm()<< endl;

  return result;
}
```