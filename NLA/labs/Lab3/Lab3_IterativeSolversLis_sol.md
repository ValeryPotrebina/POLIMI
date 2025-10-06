# Numerical linear algebra with the LIS library

Read the documentation available at

- [LIS documentation](https://www.ssisc.org/lis/index.en.html).

## 1. Basic linear algebra with LIS

Lis (Library of Iterative Solvers for linear systems) is a parallel software library for solving discretized linear equations and eigenvalue problems that arise in the numerical solution of partial differential equations using iterative methods. 

### Example: Download matrices from the web

- in the terminal, use `wget` to download a matrix from the matrix market, e.g. `wget https://math.nist.gov/pub/MatrixMarket2/NEP/mhd/mhd416a.mtx.gz`.

- unzip the file by typing `gzip -dk mhd416a.mtx.gz`

- Run again test1 using the command 

```
mpirun -n 4 ./test1 mhd416a.mtx 2 sol.mtx hist.txt
```

## 2. Exercises

### Exercise 1

- 1. Using `wget` download and unzip the matrix [here](https://math.nist.gov/pub/MatrixMarket2/Harwell-Boeing/laplace/gr_30_30.mtx.gz). Take as exact solution a vector with all the coefficients equal to 1. Using the LIS example `test1` solve the corresponding linear system. 

```
wget https://math.nist.gov/pub/MatrixMarket2/Harwell-Boeing/laplace/gr_30_30.mtx.gz
gzip gr_30_30.mtx.gz

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt
```

- 2. Solve the resulting linear system using the LIS library. Explore different iterative solvers (Jacobi, Gauss-Seidel, Conjugate Gradient...).

```
mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i cg 

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i jacobi -maxiter 2000

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i gs -tol 1.0e-10
```

- 3. Set different `options` (tolerance, maximum number of iterations, restart...)

```
mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -tol 1.0e-14

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i bicgstab -maxiter 100

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i gmres -restart 20

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i bicg -p jacobi
```

### Exercise 2

- Download the matrix A (`matA.mtx` in  matrix market format) and vector b (`vecB.mtx` in  matrix market format) from the Lab3 folder in webeep and move them to the folder `lis-2.1.10/test`. 

- Solve the linear system $A\boldsymbol{x} = \boldsymbol{b}$ using the Conjugate Gradient method of the LIS library setting a tolerance of $10^{-13}. 

- Report the iteration counts and the relative residual at the last iteration.

```
mpirun -n 4 ./test1 matA.mtx vecB.mtx sol.txt hist.txt -i cg -tol 1.0e-13
```

### Exercise 3

- 1. Following the instructions available on the LIS user-guide, compile and run test4.c

- 2. Modify the implementation by changing the size of the linear system to 120 and by setting the conjugate gradient method as iterative solver

- 3. Print the relative error

```
#include <stdio.h>
#include "lis.h"
LIS_INT main(int argc, char* argv[])
{
    LIS_Comm comm;  
    LIS_MATRIX A;
    LIS_VECTOR b,x,u;
    LIS_SOLVER solver;
    int nprocs,my_rank;
    LIS_INT err,i,n,gn,is,ie,iter;
    LIS_REAL resid;

    n = 120;
    lis_initialize(&argc, &argv);
    comm = LIS_COMM_WORLD;

#ifdef USE_MPI
    MPI_Comm_size(comm,&nprocs);
    MPI_Comm_rank(comm,&my_rank);
#else
    nprocs  = 1;
    my_rank = 0;
#endif

    lis_printf(comm,"\n");
    lis_printf(comm,"number of processes = %d\n",nprocs);

#ifdef _OPENMP
    lis_printf(comm,"max number of threads = %d\n",omp_get_num_procs());
    lis_printf(comm,"number of threads = %d\n",omp_get_max_threads());
#endif

    lis_matrix_create(comm,&A); 
    err = lis_matrix_set_size(A,0,n);
    CHKERR(err);
    lis_matrix_get_size(A,&n,&gn);
    lis_matrix_get_range(A,&is,&ie);
    for(i=is;i<ie;i++)
    {
        if( i>0   )  lis_matrix_set_value(LIS_INS_VALUE,i,i-1,-1.0,A);
        if( i<gn-1 ) lis_matrix_set_value(LIS_INS_VALUE,i,i+1,-1.0,A);
        lis_matrix_set_value(LIS_INS_VALUE,i,i,2.0,A);
    }
    lis_matrix_set_type(A,LIS_MATRIX_CSR);
    lis_matrix_assemble(A);

    lis_vector_duplicate(A,&u);
    lis_vector_duplicate(A,&b);
    lis_vector_duplicate(A,&x);
    lis_vector_set_all(1.0,u);
    lis_matvec(A,u,b);
    lis_solver_create(&solver);
    lis_solver_set_option("-i cg -p jacobi -tol 1e-14",solver);
    err = lis_solver_set_optionC(solver);
    CHKERR(err);    
    lis_solve(A,b,x,solver);
    lis_solver_get_iter(solver,&iter);
    lis_solver_get_residualnorm(solver,&resid);
    lis_printf(comm,"number of iterations = %D\n",iter);
    lis_printf(comm,"relative residual = %e\n",(double)resid);
    lis_printf(comm,"\n");
    lis_vector_print(x);

    lis_matrix_destroy(A);
    lis_vector_destroy(b);
    lis_vector_destroy(x);
    lis_vector_destroy(u);
    lis_solver_destroy(solver);
    lis_finalize();
    return 0;
}
```

```
mpicc -DUSE_MPI -I${mkLisInc} -L${mkLisLib} -llis test4.c -o test4
mpirun -n 4 ./test4
```

### Exercise 4

- 1. Following the instructions available on the LIS user-guide, compile and run test5.c

- 2. Set n = 100 and test different values of `gamma` and different iterative solvers

```
mpicc -DUSE_MPI -I${mkLisInc} -L${mkLisLib} -llis test5.c -o test5

./test5 100 0.1 -i jacobi

mpirun -n 2 ./test5 100 -1.0 -i gs 

mpirun -n 2 ./test5 100 12.0 -i gmres -restart 30  

mpirun -n 4 ./test5 100 9.9 -i bicgstab -p sainv
```

### Exercise 5

- Repeat Exercise 1 by considering some of the matrices available [here](https://sparse.tamu.edu/?per_page=All)

```
wget https://suitesparse-collection-website.herokuapp.com/MM/HB/bcsstm12.tar.gz
tar -xf bcsstm12.tar.gz
mv bcsstm12/bcsstm12.mtx .
rm -rf bcsstm12 bcsstm12.tar.gz 

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -maxiter 5000 -tol 1e-11

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i gmres -tol 1e-10

mpirun -n 4 ./test1 bcsstm12.mtx 1 sol.txt hist.txt -i bicg -p ilu
```

The importance of choosing a proper preconditioning technique can be observed by testing the solvers with and without preconditioners (cf. `-p option`). This will be the goal of the next labs.