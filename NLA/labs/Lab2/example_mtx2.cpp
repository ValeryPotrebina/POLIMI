#include <iostream>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

using namespace std;
using namespace Eigen;

int main(int argc, char** argv)
{
    // Load matrix
    SparseMatrix<double> mat;
    loadMarket(mat, "mhd416a.mtx");
    VectorXd b = VectorXd::Zero(mat.rows());
    loadMarketVector(b, "b.mtx");

    VectorXd xe = VectorXd::Constant(mat.rows(), 1);      
    VectorXd b2 = mat*xe; // compute new right-hand side
    cout << "difference = " << (b-b2).norm() << endl;
    return 0;    
}
