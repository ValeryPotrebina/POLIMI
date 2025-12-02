#include <iostream>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

using namespace std;

int main(int argc, char** argv){
    Eigen::SparseMatrix<double> mat;
    Eigen::loadMarket(mat, "Asym.mtx");

    cout << "Matrix size: " << mat.rows() << endl;
    cout << "Non zero entries: " << mat.nonZeros() << endl;
    
    Eigen::SparseMatrix<double> b = Eigen::SparseMatrix<double>(mat.transpose()) - mat;
    cout << "Norm " << b.norm() << endl;
    Eigen::VectorXd exactSolution = Eigen::VectorXd::Ones(mat.rows());
    Eigen::VectorXd result = mat * exactSolution;

    Eigen::VectorXd x(mat.rows());

    double tol = 1e-8;
    int maxit = 1000;
    Eigen::DiagonalPreconditioner<double> D(mat);
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower|Eigen::Upper> solver;

    solver.setMaxIterations(maxit);
    solver.setTolerance(tol);

    solver.compute(mat);
    x = solver.solve(result);
    cout << "Eigen native CG" << endl;
    cout << "Number of iterations: " << solver.iterations() << endl;
    cout << "Relative residual: " << solver.error() << endl;
    cout << "Effective error: " << (x - exactSolution).norm() << endl;

    // export the solution to a file
    Eigen::saveMarketVector(x, "solution_vector.mtx");
    return 0;

}