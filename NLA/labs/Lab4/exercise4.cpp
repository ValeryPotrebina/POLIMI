#include <cstdlib>                      // System includes
#include <iostream>                      
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/SparseExtra>

#include "cgs.hpp"
#include "bcgstab.hpp"
#include "gmres.hpp"

using std::endl;
using std::cout;

int main(int argc, char** argv)
{
  using namespace LinearAlgebra;
  // Some useful alias
  using SpMat=Eigen::SparseMatrix<double>;
  using SpVec=Eigen::VectorXd;

  // Matrix parameters from test5.c
  int n = 1000;
  double gamma = -0.5;
  
  SpMat A(n,n);                      // define matrix
  
  // Reserve space for non-zero entries
  // Diagonal: n elements
  // Superdiagonal (i, i+1): n-1 elements
  // (i, i-2): n-2 elements (for i>1)
  A.reserve(3*n - 3);
  
  // Build matrix as in test5.c:
  // for(ii=is;ii<ie;ii++)
  // {
  //     if( ii>1 )    { jj = ii - 2; value[k++] = gamma;}
  //     if( ii<gn-1 ) { jj = ii + 1; value[k++] = 1.0;}
  //     value[k++] = 2.0;  // diagonal
  // }
  
  for (int i=0; i<n; i++) {
      A.coeffRef(i, i) = 2.0;                    // Diagonal
      if(i > 1) A.coeffRef(i, i-2) = gamma;      // Element (i, i-2)
      if(i < n-1) A.coeffRef(i, i+1) = 1.0;      // Superdiagonal
  }

  // Parameters
  double tol = 1.e-8;                // Convergence tolerance
  int result, maxit = 1000;          // Maximum iterations
  int restart = 30;                  // Restart for GMRES

  std::cout<<"========================================"<<std::endl;
  std::cout<<"Matrix size: "<<A.rows()<<" X "<<A.cols()<<std::endl;
  std::cout<<"Non zero entries: "<<A.nonZeros()<<std::endl;
  std::cout<<"Gamma = "<<gamma<<std::endl;
  std::cout<<"========================================"<<std::endl<<std::endl;
  
  // Check if matrix is symmetric
  SpMat B = SpMat(A.transpose()) - A;
  std::cout<<"Norm of A-A^T: "<<B.norm()<<std::endl;
  if(B.norm() > 1e-10) {
      std::cout<<"Matrix is NON-SYMMETRIC (as expected for test5.c)"<<std::endl;
  }
  std::cout<<std::endl;

  // Create exact solution and RHS
  SpVec e = SpVec::Ones(n);
  SpVec b = A * e;
  SpVec x(A.rows());
  
  // Create preconditioners
  // For CGS: Least Square Diagonal Preconditioner
  Eigen::LeastSquareDiagonalPreconditioner<double> SD(A);
  // For BiCGSTAB and GMRES: Jacobi (Diagonal) Preconditioner
  Eigen::DiagonalPreconditioner<double> D(A);

  // ==========================================
  // 1. Solve with CGS method
  // ==========================================
  std::cout<<"========== CGS METHOD =========="<<std::endl;
  x = 0*x;  // Reset initial guess
  maxit = 1000;
  tol = 1.e-8;
  result = CGS(A, x, b, SD, maxit, tol);
  
  cout << "Convergence flag: " << result;
  if(result == 0) cout << " (SUCCESS)";
  else if(result == 1) cout << " (NO CONVERGENCE)";
  else if(result == 2) cout << " (BREAKDOWN: rho=0)";
  cout << endl;
  cout << "Number of iterations: " << maxit << endl;
  cout << "Relative residual: " << tol << endl;
  cout << "Effective error ||x - e||: " << (x - e).norm() << endl;
  cout << endl;

  // ==========================================
  // 2. Solve with BiCGSTAB method
  // ==========================================
  std::cout<<"========== BiCGSTAB METHOD =========="<<std::endl;
  x = 0*x;  // Reset initial guess
  maxit = 1000;
  tol = 1.e-8;
  result = BiCGSTAB(A, x, b, D, maxit, tol);
  
  cout << "Convergence flag: " << result;
  if(result == 0) cout << " (SUCCESS)";
  else if(result == 1) cout << " (NO CONVERGENCE)";
  else if(result == 2) cout << " (BREAKDOWN: rho=0)";
  else if(result == 3) cout << " (BREAKDOWN: omega=0)";
  cout << endl;
  cout << "Number of iterations: " << maxit << endl;
  cout << "Relative residual: " << tol << endl;
  cout << "Effective error ||x - e||: " << (x - e).norm() << endl;
  cout << endl;

  // ==========================================
  // 3. Solve with GMRES method
  // ==========================================
  std::cout<<"========== GMRES METHOD =========="<<std::endl;
  x = 0*x;  // Reset initial guess
  maxit = 1000;
  tol = 1.e-8;
  result = GMRES(A, x, b, D, restart, maxit, tol);
  
  cout << "Restart parameter: " << restart << endl;
  cout << "Convergence flag: " << result;
  if(result == 0) cout << " (SUCCESS)";
  else if(result == 1) cout << " (NO CONVERGENCE)";
  cout << endl;
  cout << "Number of iterations: " << maxit << endl;
  cout << "Relative residual: " << tol << endl;
  cout << "Effective error ||x - e||: " << (x - e).norm() << endl;
  cout << endl;

  // ==========================================
  // Summary comparison
  // ==========================================
  std::cout<<"========================================"<<std::endl;
  std::cout<<"SUMMARY: All three methods should converge"<<std::endl;
  std::cout<<"for this non-symmetric system."<<std::endl;
  std::cout<<"========================================"<<std::endl;

  return 0;
}

