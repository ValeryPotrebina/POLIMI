#include <iostream>
#include <Eigen/Sparse>

using namespace std;
using namespace Eigen;

int main(int argc, char** argv)
{
    int n = 200;	
    SparseMatrix<double> mat(n,n);                           // define matrix
    for (int i=0; i<n; i++) {
        mat.coeffRef(i, i) = 2.0;
	if(i>0) mat.coeffRef(i, i-1) = -1.0;
        if(i<n-1) mat.coeffRef(i, i+1) = -1.0;	
    }

    VectorXd xe = VectorXd::Constant(mat.rows(), 1);         // define sol
    VectorXd b = mat*xe;                                     // compute rhs

    // Solving 
    ConjugateGradient<SparseMatrix<double>, Lower|Upper> solver;
    solver.setTolerance(1.0e-4);
    solver.compute(mat);
    VectorXd x = solver.solve(b);
    std::cout << "#iterations:     " << solver.iterations() << std::endl;
    cout << x << endl;                                       // display sol

    double relative_error = (x-xe).norm()/(xe).norm();       // compute err 
    cout << relative_error << endl;
    return 0;    
}
