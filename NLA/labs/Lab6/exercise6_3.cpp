#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <unsupported/Eigen/SparseExtra>

using namespace std;
using namespace Eigen;

int main(int argc, char** argv)
{
  // Load matrix
  SparseMatrix<double> mat;
  loadMarket(mat, "nos1.mtx");
  // Check matrix properties
  std::cout << "Matrix size:" << mat.rows() << " X " << mat.cols() << std::endl;
  std::cout << "Non zero entries:" << mat.nonZeros() << std::endl;
  SparseMatrix<double> B = SparseMatrix<double>(mat.transpose()) - mat; 
  std::cout << "Norm of skew-symmetric part: " << B.norm()<< std::endl;

  // Compute Eigenvalues of the original matrix
  MatrixXd A;
  A = MatrixXd(mat);
  EigenSolver<MatrixXd> eigensolver(A);
  if (eigensolver.info() != Eigen::Success) abort();
  std::cout << "The eigenvalues of A are:\n" << eigensolver.eigenvalues() << std::endl;

  // Compute Eigenvalues of symmetric matrix
  SparseMatrix<double> C = 0.5*(SparseMatrix<double>(mat.transpose()) + mat);
  SelfAdjointEigenSolver<MatrixXd> saeigensolver(C);
  if (saeigensolver.info() != Eigen::Success) abort();
  std::cout << "The eigenvalues of A_symm are:\n" << saeigensolver.eigenvalues() << std::endl;
  return 0;
}
