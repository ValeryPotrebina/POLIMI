#include <iostream>
#include <Eigen/Sparse>
 
using namespace Eigen;
using namespace std;

typedef Eigen::Triplet<double> T;
 
int main()
{
    SparseMatrix<double> mat(10, 10);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            mat.coeffRef(i, j) = i + j;
        }
    }
    cout << mat << endl;

    std::vector<T> tripletList;
    tripletList.reserve(10);
    for (int i = 0; i < 10; i++) {
        tripletList.push_back(T(i, i, 1));
    }
    mat.setFromTriplets(tripletList.begin(), tripletList.end());
    std::cout << mat << std::endl;

    VectorXd b = VectorXd::Constant(mat.rows(), 1); // define right-hand side

    // Solving
    SimplicialLDLT<Eigen::SparseMatrix<double> > solver(mat);   // factorization
    solver.compute(mat);
    if(solver.info()!=Success) {                                // sanity check
        cout << "cannot factorize the matrix" << endl;
        return 0;
    }

    VectorXd x = solver.solve(b);                   // solving
    cout << "Result: " << x << endl;                              // display solution
    return 0;
}