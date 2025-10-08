#include <iostream>
#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;

MatrixXd make_diagonal(int n) {
    MatrixXd A = MatrixXd::Zero(n, n);

    A.diagonal().setConstant(2.0);
    A.diagonal(1).setConstant(-1.0);
    A.diagonal(-1).setConstant(1.0);
    return A;
}


int main() {
    MatrixXd A = MatrixXd::Random(3, 3);
    MatrixXd B = MatrixXd::Random(3, 3);
    cout << "A = " << A << endl;
    cout << "B = " << B << endl;
    VectorXd z = VectorXd::Random(3);
    cout << "z = " << z << endl;

    MatrixXd C = A * B;
    cout << "C = " << C << endl;

    VectorXd x = z.head(2);
    cout << "x = " << x << endl;

    MatrixXd I = MatrixXd::Identity(3, 3);
    cout << "I = " << I << endl;

    MatrixXd E = MatrixXd::Constant(3, 3, 1.0);
    cout << "E: \n" << E << endl;

    VectorXd v(3);
    v << 1, 2, 3;
    cout << "v = " << v << endl;

    MatrixXd D(5, 5);
    D << 1, 2, 3, 4, 5,
         6, 7, 8, 9, 10,
         11, 12, 13, 14, 15,
         16, 17, 18, 19, 20,
         21, 22, 23, 24, 25;
    cout << "D = " << D << endl;

    cout << "--------" << endl;

    VectorXd t(6);
    t << 1, 2, 3, 4, 5, 6;
    cout << "t = " << t << endl;
    cout << "t.head(3) = \n" << t.head(3) << endl;
    cout << "t.tail(3) = \n" << t.tail(3) << endl;
    cout << "t.segment(1, 4) = \n" << t.segment(1, 4) * 2 << endl;
    cout << "t = \n" << t << endl;

    MatrixXd Q = make_diagonal(10);
    cout << "Q = \n" << Q << endl;
    double norm_Q = Q.norm();
    cout << "norm_Q = " << norm_Q << endl;

    VectorXd d = VectorXd::Ones(10);

    return 0;
}