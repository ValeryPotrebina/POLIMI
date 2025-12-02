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

  int n = 600;
  SpMat A(n,n);                       // define matrix
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) = 2.0*(i+1);
      if(i>0) A.coeffRef(i, i-1) -= i;
      if(i<n-1) A.coeffRef(i, i+1) -= (i+1);
  }

  double tol = 1.e-8;                  // Convergence tolerance
  int result, maxit = 20000;           // Maximum iterations

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
  int restart = 400;
  result = GMRES(A, x, b, D, restart, maxit, tol);
  cout << " GMRES method " << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "relative residual  : " << tol << endl;
  cout << "absolute error:      " << (x-e).norm()<< endl;

  return result;
}
