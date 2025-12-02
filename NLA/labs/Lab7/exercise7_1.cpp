#include <Eigen/SparseCore>
#include <Eigen/SparseQR>
#include <iostream>
#include <string>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/SparseExtra>

int main(int argc, char *argv[]){
  using namespace Eigen;

  if(argc != 2)
    {
      std::cerr << " Usage: provide matrix filename" << std::endl;
      return 1;
    }
  std::string matrixFile(argv[1]);

  // Some useful alias
  using SpMat = SparseMatrix<double>;
  using SpVec = VectorXd;

  // Read matrix
  SpMat A;
  loadMarket(A, matrixFile);

  // Create Rhs b
  std::cout << "Size of A: " << A.rows() << " x " << A.cols() << std::endl;
  SpVec e = SpVec::Ones(A.cols());
  SpVec b = A*e;
  SpVec x(A.cols());

  // solve with Eigen QR factorization
  Eigen::SparseQR<Eigen::SparseMatrix<double>, COLAMDOrdering<int>> solver; 
  solver.compute(A);
  if(solver.info()!=Eigen::Success) {                   
      std::cout << "cannot factorize the matrix" << std::endl;
      return 0;
  }
  x = solver.solve(b);                                 
  std::cout << "Solution with Eigen QR:" << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;

  // solve with Eigen LeastSquareConjugateGradient solver
  LeastSquaresConjugateGradient<SparseMatrix<double> > lscg;
  lscg.compute(A);
  x = lscg.solve(b);
  std::cout << "Solution with Eigen LSCG:" << std::endl;
  std::cout << "#iterations:     " << lscg.iterations() << std::endl;
  std::cout << "relative residual: " << lscg.error()      << std::endl;
  std::cout << "effective error: "<<(x-e).norm()<< std::endl;
  return 1;
}
  
