#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <iostream>
#include <string>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/SparseExtra>
#include "cg.hpp"
#include "gmres.hpp"

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
  double tol = 1.e-12;                  // Convergence tolerance
  int result, maxit = 1000;            // Maximum iterations 
  MatrixXd I = MatrixXd::Identity(A.rows(),A.cols());
  Eigen::DiagonalPreconditioner<double> D(I); // preconditioner
  result = CG(A,x,b,D,maxit,tol);
  std::cout << "Solution with Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl << std::endl;

  // Compute block preconditioner with Eigen Choleski direct solver
  int m = 10100;
  I.topLeftCorner(A.rows()-m, A.cols()-m) = A.topLeftCorner(A.rows()-m,A.cols()-m);
  I.bottomRightCorner(m,m) = A.bottomRightCorner(m,m);
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
  SpVec x2(A.rows()); tol = 1.e-12; maxit = 1000;      
  result = CG(A,x2,b,solver,maxit,tol);
  std::cout << "Solution with precond Conjugate Gradient:" << std::endl;
  std::cout << "iterations performed: " << maxit << std::endl;
  std::cout << "tolerance achieved  : " << tol << std::endl;
  std::cout << "comparison solutions : " << (x-x2).norm() << std::endl;

  return result;
}
  
