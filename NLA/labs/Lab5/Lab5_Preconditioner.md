
# Preconditioners with Eigen and LIS

In the following exercise we aim to compare different preconditioning strategies. In Eigen there is a built-in incomplete LU preconditioner that we can provide as an argument for the available linear solver. This ILU preconditioners as two parameters (`fill_in` and `tol`) which can be used to define the accuracy of the approximated LU factorization with respect to the original matrix. 

### Exercise 1: Diagonal preconditioner vs. Eigen ILU preconditioner

In the following example, we can observe that the default ILU preconditioner provided by Eigen is very close to a full LU factorization. Indeed, using the `Eigen::IncompleteLUT`, the CG method convergences in just two iterations. 

```
// ... usual include modules
#include "cg.hpp"

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  // Load matrix
  SpMat A;
  Eigen::loadMarket(A, "bcsstm12.mtx");
  // Transform the loaded matrix into a symmetric one
  A = SpMat(A.transpose()) + A;

  double tol = 1.e-14;                 // Convergence tolerance
  int result, maxit = 5000;            // Maximum iterations

  std::cout<<"Matrix size:"<<A.rows()<<"X"<<A.cols()<<std::endl;
  std::cout<<"Non zero entries:"<<A.nonZeros()<<std::endl;

  // Create Rhs b
  SpVec e = SpVec::Ones(A.rows());
  SpVec b = A*e;
  SpVec x(A.rows());

  // Create preconditioners
  Eigen::DiagonalPreconditioner<double> D(A);
  Eigen::IncompleteLUT<double> ILU(A);

  // Hand-made CG with diagonal precond
  x = 0*x;
  result = CG(A, x, b, D, maxit, tol);        // Solve system

  std::cout <<" hand-made CG "<<std::endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  std::cout << "Error norm: "<<(x-e).norm()<<std::endl;

  // Hand-made CG with ILU precond
  x = 0*x;
  result = CG(A, x, b, ILU, maxit, tol);
  std::cout <<" hand-made CG "<< std::endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "tolerance achieved  : " << tol << endl;
  std::cout << "Error norm: "<<(x-e).norm()<<std::endl;

  return result;
}

```

### Exercise 2: ILU preconditioner in LIS

It is also possible to assess the performances of the ILU preconditioner available in the LIS library and compare it with other preconditioning techniques. Some examples are reported below. 


```
wget https://suitesparse-collection-website.herokuapp.com/MM/HB/bcsstm12.tar.gz
... 

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -maxiter 5000 -tol 1e-12

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -tol 1e-12 -p ilu

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -tol 1e-12 -p ilu -ilu_fill 2
```
