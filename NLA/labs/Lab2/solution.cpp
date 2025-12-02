#include <iostream>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

using namespace std;

int main(int argc, char** argv)
{
    // Load matrix
    Eigen::SparseMatrix<double> mat;
    Eigen::loadMarket(mat, "Asym.mtx");
    
    // Check matrix properties
    std::cout << "Matrix size:"<< mat.rows() << "X" << mat.cols() << endl;
    std::cout << "Non zero entries:" << mat.nonZeros() << endl;
    Eigen::SparseMatrix<double> B = Eigen::SparseMatrix<double>(mat.transpose()) - mat;
    std::cout << "Norm of skew-symmetric part: " << B.norm() << endl;

    // Create Rhs b
    Eigen::VectorXd e = Eigen::VectorXd::Ones(mat.rows());    // Define exact solution
    Eigen::VectorXd b = mat*e;                      // Compute rhs
    Eigen::VectorXd x(mat.rows());

    // Set parameters for solver
    double tol = 1.e-8;                 // Convergence tolerance
    int result, maxit = 1000;           // Maximum iterations
    Eigen::DiagonalPreconditioner<double> D(mat); // Create diag preconditioner

    // Solving 
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower|Eigen::Upper> cg;
    cg.setMaxIterations(maxit);
    cg.setTolerance(tol);
    cg.compute(mat);
    x = cg.solve(b);
    std::cout << " Eigen native CG" << endl;
    std::cout << "#iterations:     " << cg.iterations() << endl;
    std::cout << "relative residual: " << cg.error()      << endl;
    std::cout << "effective error: " << (x-e).norm() << endl;

    return 0;    
}