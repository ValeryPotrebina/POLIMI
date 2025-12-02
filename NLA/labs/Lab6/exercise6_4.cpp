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
  loadMarket(mat, "stokes_sym.mtx");
  // loadMarket(mat, "lapsocial.mtx");
  // Check matrix properties
  std::cout << "Matrix size:" << mat.rows() << " X " << mat.cols() << std::endl;
  std::cout << "Non zero entries:" << mat.nonZeros() << std::endl;
  SparseMatrix<double> B = SparseMatrix<double>(mat.transpose()) - mat;
  std::cout << "Norm of skew-symmetric part: " << B.norm()<< std::endl;

  // Compute Eigenvalues of original matrix
  SelfAdjointEigenSolver<MatrixXd> eigensolver(mat);
  if (eigensolver.info() != Eigen::Success) abort();
  std::cout << "The eigenvalues of A are:\n" << eigensolver.eigenvalues() << std::endl;

  // Modify the matrix by adding a tridiagonal matrix
  for (int i=0; i<mat.rows(); i++) {
    mat.coeffRef(i, i) += 2.0;
    if(i>0) mat.coeffRef(i, i-1) += -1.0;
    if(i<mat.rows()-1) mat.coeffRef(i, i+1) += -1.0;
  }
 
  // Compute Eigenvalues of modified matrix
  SelfAdjointEigenSolver<MatrixXd> saeigensolver(mat);
  if (saeigensolver.info() != Eigen::Success) abort();
  std::cout << "The eigenvalues of A_mod are:\n" << saeigensolver.eigenvalues() << std::endl;

  // Export the modified matrix in the matrix market format
  std::string matrixFileOut("./mat_out.mtx");
  Eigen::saveMarket(mat, matrixFileOut);
  return 0;
}
