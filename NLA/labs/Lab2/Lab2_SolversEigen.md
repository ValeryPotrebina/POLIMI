# 2. Linear systems 

We aim to solve a linear system of equations `Ax = b`, where `A` and `b` are matrices (b could be a vector, as a special case). In Eigen we can choose between various decompositions, direct solvers, or iterative solvers depending on the properties of  matrix `A` and on the desired speed and accuracy. 

A table of the decomposition methods available in Eigen is reported [here](https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html). 

If you know more about the properties of your matrix, you can use the above table to select the best method. For example, a good choice for solving linear systems with a non-symmetric matrix of full rank is `PartialPivLU`. If you know that your matrix is also symmetric and positive definite, the above table says that a very good choice is the LLT or LDLT decomposition. 

In case of very sparse matrices, iterative solvers can provide a good precision with less computational cost. 

### 2.1 Assess the quality of the approximate solution

Only you know what error margin you want to allow for a solution to be considered valid. So Eigen lets you do this computation for yourself, if you want to, as in this example:

```
#include <iostream>
#include <Eigen/Dense>
 
using Eigen::MatrixXd;
 
int main()
{
   MatrixXd A = MatrixXd::Random(100,100);
   MatrixXd b = MatrixXd::Random(100,50);
   MatrixXd x = A.fullPivLu().solve(b);
   double relative_residual = (A*x - b).norm() / b.norm(); // norm() is L2 norm
   std::cout << "The relative residual is:\n" << relative_residual << std::endl;
}
```

### 2.2 Sparse linear systems

The class `SparseMatrix` is the main sparse matrix representation of Eigen's sparse module; it offers high performance and low memory usage. It implements a more versatile variant of the widely-used Compressed Column (or Row) Storage scheme.

The `SparseMatrix` and `SparseVector` classes take three template arguments: the scalar type (e.g., double) the storage order (ColMajor or RowMajor, the default is ColMajor) the inner index type (default is int). As for dense Matrix objects, constructors takes the size of the object. Here are some examples:

```
SparseMatrix<double,RowMajor> mat(1000,2000);              
// declares a 1000x2000 row-major compressed sparse matrix of double

SparseVector<double,RowMajor> vec(1000);                   
// declares a row sparse vector of double of size 1000
```

In Eigen, there are several methods available to solve linear systems whenever the matrix is sparse. Because of the special representation of this class of matrices, special care should be taken in order to get a good performance. [This page](https://eigen.tuxfamily.org/dox/group__TopicSparseSystems.html) lists the sparse solvers available in Eigen. All the available solvers follow the same general concept.

```
#include <Eigen/RequiredModuleName>
// ...
SparseMatrix<double> A;
// fill A
VectorXd b, x;
// fill b
// solve Ax = b
SolverClassName<SparseMatrix<double> > solver;
solver.compute(A);
if(solver.info()!=Success) {
  // decomposition failed
  return;
}
x = solver.solve(b);
if(solver.info()!=Success) {
  // solving failed
  return;
}
// solve for another right hand side:
x1 = solver.solve(b1);
```

A simple example:

```
#include <iostream>
#include <Eigen/Sparse>

using namespace std;
using namespace Eigen;
typedef Eigen::Triplet<double> T;

int main(int argc, char** argv)
{
       SparseMatrix<double> mat(10,10);       // define matrix
    // for (int i=0; i<10; i++) {
    //     mat.coeffRef(i, i) = 1.0;          // or mat.insert(i, i) = 1.0;
    // }

    // Alternative way to fill the matrix
    std::vector<T> tripletList;
    tripletList.reserve(10);
    for(int i=0; i<10; i++) {
      tripletList.push_back(T(i, i, 1.0));
    }
    mat.setFromTriplets(tripletList.begin(), tripletList.end());

    VectorXd b = VectorXd::Constant(mat.rows(), 1); // define right-hand side

    // Solving 
    SimplicialLDLT<Eigen::SparseMatrix<double> > solver(mat);   // factorization 
    solver.compute(mat);
    if(solver.info()!=Success) {                                // sanity check 
        cout << "cannot factorize the matrix" << endl;          
        return 0;
    }
    
    VectorXd x = solver.solve(b);                   // solving
    cout << x << endl;                              // display solution
    return 0;    
}
```

N.B. Block operations works also for sparse matrix, but it is recommended not to use for WRITING or MODIFYING existing SPARSE matrices. We will mainly use these operations to extract blocks.

### 2.3 Load and export sparse matrices

The matrix market format is a common way to store spare matrices. It is suported by most of the existing Numerical Linear Algebra libraries (Eigen and LIS in particular). 

To export your matrices and right-hand-side vectors in the matrix-market format, we can use the `unsupported SparseExtra` module.

```
#include <unsupported/Eigen/SparseExtra>
...
Eigen::saveMarket(A, "filename.mtx");
Eigen::saveMarketVector(B, "filename_b.mtx");
```

To load a matrix in the matrix market format, follow the instructions below:

- in the terminal, use `wget` to download a matrix from the matrix market, e.g. `wget https://math.nist.gov/pub/MatrixMarket2/NEP/mhd/mhd416a.mtx.gz`.

- unzip the file by typing `gzip -dk mhd416a.mtx.gz`

- in Eigen, include the `unsupported SparseExtra` module and use 

```
SparseMatrix<double> mat;
loadMarket(mat, "mhd416a.mtx");
```

To export a matrix in the matrix market format, follow the instructions below:

```
std::string matrixFileOut("./matrixName.mtx");
Eigen::saveMarket(mat, matrixFileOut);
```

### Exercise

1. Compile and test the following example 

```
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

    VectorXd xe = VectorXd::Constant(mat.rows(), 1);      
    // define exact solution
    VectorXd b = mat*xe;                 // compute right-hand side
    cout << b << endl;
    
    // Export vector in .mtx format
    int n = b.size();
    // Eigen::saveMarketVector(b, "./rhs.mtx");
    FILE* out = fopen("rhs.mtx","w");
    fprintf(out,"%%%%MatrixMarket vector coordinate real general\n");
    fprintf(out,"%d\n", n);
    for (int i=0; i<n; i++) {
        fprintf(out,"%d %f\n", i ,b(i));
    }
    fclose(out);

    return 0;    
}
```

2. Following the procedure adopted in Lab 1, remove the entries smaller than 1e-15 from the matrix file `mhd416a.mtx`. Modify the previus script in order to load the modified matrix in a new sparse matrix `mat_2` in Eigen and repeat the previous computations. Compute the norm of the difference between the previous right-hand side `b` and the new one `b_2`.

## 3. The CG Sparse iterative solver of Eigen

Eigen provides a built-in `Eigen::ConjugateGradient` solver. This class allows to solve for $A\boldsymbol{x} = \boldsymbol{b}$ linear problems using an iterative conjugate gradient algorithm. The matrix A must be selfadjoint. The matrix A and the vectors x and b can be either dense or sparse.

This class follows the sparse solver concept and has the following inputs:
- `MatrixType_`	the type of the matrix $A$, can be a dense or a sparse matrix.
- `UpLo_` the triangular part that will be used for the computations. It can be Lower, Upper, or Lower|Upper in which the full matrix entries will be considered. 
- `Preconditioner_` the type of the preconditioner. Default is DiagonalPreconditioner

The maximal number of iterations and tolerance value can be controlled via the setMaxIterations() and setTolerance() methods. The defaults are the size of the problem for the maximal number of iterations and NumTraits<Scalar>::epsilon() for the tolerance.

The tolerance corresponds to the relative residual error: 
$$
tol = |A\boldsymbol{x}- \boldsymbol{b}|/|\boldsymbol{b}|
$$

N.B. Even though the default value of `UpLo_` is `Lower`, significantly higher performance is achieved when using a complete matrix and `Lower|Upper` as the `UpLo_` template parameter.

### 3.1 Example

- Download the matrix `Asym.mtx` from webeep Lab2 folder and move it to the working directory.
- Display the size of the matrix and check if it is symmetric. 
- Take as exact solution a vector `xe` defined as in the previous example and compute the right-hand side `b`. 
- Solve the resulting linear system using the Conjugate Gradient (CG) solver available in Eigen. 
- Compute and display the relative error between the exact solution `xe` and the approximated solution.

```
#include <iostream>
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

using namespace std;

// Some useful alias
using SpMat=Eigen::SparseMatrix<double>;
using SpVec=Eigen::VectorXd;

int main(int argc, char** argv)
{
    // Load matrix
    SpMat mat;
    Eigen::loadMarket(mat, "Asym.mtx");
    
    // Check matrix properties
    std::cout << "Matrix size:"<< mat.rows() << "X" << mat.cols() << endl;
    std::cout << "Non zero entries:" << mat.nonZeros() << endl;
    SpMat B = SpMat(mat.transpose()) - mat;  // Check symmetry
    std::cout << "Norm of skew-symmetric part: " << B.norm() << endl;

    // Create Rhs b
    SpVec e = SpVec::Ones(mat.rows());    // Define exact solution
    SpVec b = mat*e;                      // Compute rhs
    SpVec x(mat.rows());

    // Set parameters for solver
    double tol = 1.e-8;                 // Convergence tolerance
    int result, maxit = 1000;           // Maximum iterations
    Eigen::DiagonalPreconditioner<double> D(mat); // Create diag preconditioner

    // Solving 
    Eigen::ConjugateGradient<SpMat, Eigen::Lower|Eigen::Upper> cg;
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
```

### 3.2 Exercise

- In Eigen, construct the $50\times 50$ symmetric matrix $A$ defined such that
$$ 
A = \begin{pmatrix}
    2 & -1 & 0 & 0&\ldots & 0  \\
    -1 & 2 & -1 & 0& \ldots & 0  \\
    0 & -1 & \ddots  & \ddots &\ldots  & \vdots \\
    0 & 0 & \ddots  & \ddots  & \ddots & 0 \\
   \vdots& \vdots &  \vdots &\ddots &\ddots  & -1\\
    0 & 0  &\ldots & 0& -1   & 2
\end{pmatrix}.
$$
- Display the number of nonzero entries and check if it is symmetric. 
- Take as exact solution a vector `xe` defined as in the previous example and compute the right-hand side `b`. 
- Solve the resulting linear system using the Conjugate Gradient (CG) solver available in Eigen. 
- Compute and display the relative error between the exact solution `xe` and the approximated solution.
- Export matrix $A$ and the right-hand side vector $\boldsymbol{b}$ in the `.mtx` format. Move the files in the test folder of the LIS library and repeat the previous exercise using LIS.

### HOMEWORK
Repeat the previous exercise using the BiCGSTAB solver of Eigen.