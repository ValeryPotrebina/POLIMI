#include <Eigen/Dense>
#include <iostream>
 
using namespace std;
using Eigen::VectorXd;
using Eigen::MatrixXd;
 
int main()
{
  int n = 100;
  MatrixXd A = MatrixXd::Zero(n,n);
    for (int i=0; i<n; i++) {
        A(i, i) = 2.0;
        if(i>0) A(i, i-1) = 1.0;
        if(i<n-1) A(i, i+1) = -1.0;
    }

  VectorXd v = VectorXd::Constant(50, 1);     // define vector
  cout << "matrix vector multiplication =" << endl << A.topLeftCorner(50,50)*v << endl;

  cout << "norm of A = " << A.norm() << endl;
  cout << "norm of symmetric part " << (A.transpose() + A).norm() << endl;
  cout << "dot product " << v.dot((A.row(0)).head(50)) << endl;
}
