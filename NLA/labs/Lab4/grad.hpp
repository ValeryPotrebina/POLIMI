namespace LinearAlgebra {
template <class Matrix, class Vector, class Preconditioner>

int GRAD(const Matrix &A, Vector &x, const Vector &b, const Preconditioner &M, int &max_iter, typename Vector::Scalar &tol) {
    using Real = typename Matrix::Scalar;

    Real resid;
    Real alpha, rho;
    Vector r(b.size()); // residual = b - A * x
    Vector z(b.size()); // preconditioned residual M * z = r 
    Vector q(b.size()); // A * z

    Real normb = b.norm();
    r = b - A * x;

    if(normb == 0.0)
        normb = 1;

    if ((resid = r.norm() / normb) <= tol) {
        tol = resid;
        max_iter = 0;
        return 0;
    }

    for (int i = 1; i < max_iter; i++) {
        z = M.solve(r); // M * z = r  -> z = M^-1 * r
        rho = r.dot(z); // r^T * z
        q = A * z; // A * z 

        alpha = rho / z.dot(q); // alpha = r^T * z / z^T * A * z

        //r = r - alpha * A * z -> r = r - alpha * q
        x = x + alpha * z;
        r = r - alpha * q;

        if ((resid = r.norm() / normb) <= tol) {
            tol = resid;
            max_iter = i;
            return 0;
        }
    }
    tol = resid;
    return 1;
}
}