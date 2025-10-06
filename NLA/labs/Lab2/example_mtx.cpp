#include <iostream>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

using namespace std;
using namespace Eigen;

int main(int argc, char** argv)
{
    // Load matrix
    SparseMatrix<double,RowMajor> mat;
    loadMarket(mat, "mhd416a.mtx");

    VectorXd xe = VectorXd::Constant(mat.rows(), 1);      
    // define exact solution
    VectorXd b = mat*xe;                 // compute right-hand side
    cout << b.size() << endl;
    
    // Export vector in .mtx format
    Eigen::saveMarketVector(b, "./b.mtx");
    int n = b.size();
    FILE* out = fopen("rhs.mtx","w");
    fprintf(out,"%%%%MatrixMarket vector coordinate real general\n");
    fprintf(out,"%d\n", n);
    for (int i=0; i<n; i++) {
       fprintf(out,"%d %f\n", i ,b(i));
    }
    fclose(out);

    return 0;    
}
