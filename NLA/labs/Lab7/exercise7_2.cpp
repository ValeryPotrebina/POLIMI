#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/Dense>
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

  // Read matrix
  SpMat A;
  loadMarket(A, matrixFile);
  MatrixXd Ad = MatrixXd(A); // convert to dense for SVD

  // compute Eigen SVD
  Eigen::BDCSVD<Eigen::MatrixXd> svd (Ad, Eigen::ComputeFullU | Eigen::ComputeFullV);
  VectorXd W = svd.singularValues();
  std::cout << "singular values: " << W << "\n";
  //MatrixXd Asvd = svd.matrixU() * svd.singularValues().asDiagonal() * svd.matrixV().transpose();
  //MatrixXd diff = Asvd - Ad;
  //std::cout << "diff:\n" << diff.norm() << "\n";
  std::cout << "Size of U: " << svd.matrixU().rows() << " x " << svd.matrixU().cols() << std::endl;  
  std::cout << "Size of V: " << svd.matrixV().rows() << " x " << svd.matrixV().cols() << std::endl;

  // Compute Eigenvalues
  // Eigen::SelfAdjointEigenSolver<MatrixXd> eigensolver(A);
  // if (eigensolver.info() != Eigen::Success) abort();
  // std::cout << "The eigenvalues of A are:\n" << eigensolver.eigenvalues() << std::endl;
  return 1;
}
