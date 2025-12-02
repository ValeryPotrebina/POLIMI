#include <Eigen/Core>
#include <Eigen/SparseQR>
#include <Eigen/SVD>
#include <iostream>
#include <string>
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
  MatrixXd Ad = MatrixXd(A); // convert to dense for SVD

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

  // solve with Eigen SVD
  Eigen::BDCSVD<Eigen::MatrixXd> svd (Ad, Eigen::ComputeThinU | Eigen::ComputeThinV);
  VectorXd W = svd.singularValues();
  std::cout << "singular values: " << W << "\n";
  MatrixXd Asvd = svd.matrixU() * svd.singularValues().asDiagonal() * svd.matrixV().transpose();
  MatrixXd diff = Asvd - Ad;
  std::cout << "diff:\n" << diff.norm() << "\n";
  
  x = svd.solve(b);
  std::cout << "Solution with Eigen SVD:" << std::endl;
  std::cout << "absolute error: " << (x-e).norm()  << std::endl;
  return 1;
}
  
