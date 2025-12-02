#include <cstdlib>                      // System includes
#include <iostream>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/SparseExtra>

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
  Eigen::IdentityPreconditioner I;
  Eigen::IncompleteCholesky<double> ichol(A);

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
  result = GRAD(A, x, b, ichol, maxit, tol);   
  std::cout <<" Gradient method " << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "relative residual  : " << tol << endl;
  std::cout << "absolute error: "<<(x-e).norm()<< endl;

  // Solve with GMRES method
  x=0*x; maxit = 1000; tol = 1.e-8;
  int restart = 30;
  result = GMRES(A, x, b, ichol, restart, maxit, tol);
  cout << " GMRES method " << endl;
  cout << "iterations performed: " << maxit << endl;
  cout << "relative residual  : " << tol << endl;
  cout << "absolute error:      " << (x-e).norm()<< endl;

  // Export matrix A and vector b for LIS
  std::string matrixFileOut("./AtestGrad.mtx");
  std::string vectorFileOut("./btestGrad.mtx");
  Eigen::saveMarket(A, matrixFileOut);
  Eigen::saveMarketVector(b, vectorFileOut);
  std::cout << "\n==================================" << endl;
  std::cout << "Matrix exported to: " << matrixFileOut << endl;
  std::cout << "Vector b exported to: " << vectorFileOut << endl;
  std::cout << "==================================" << endl;

  return result;
}