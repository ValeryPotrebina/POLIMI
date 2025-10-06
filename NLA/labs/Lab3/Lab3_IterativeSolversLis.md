# Numerical linear algebra with the LIS library

Read the documentation available at

- [LIS documentation](https://www.ssisc.org/lis/index.en.html)

## 1. Basic linear algebra with LIS

Lis (Library of Iterative Solvers for linear systems) is a parallel software library for solving discretized linear equations and eigenvalue problems that arise in the numerical solution of partial differential equations using iterative methods. 

As a first example of the usage of LIS we aim to run the first test reported in the user guide. To do so, follow the next steps:

1. To enable `mk` modules, you should type each time you open the terminal 
```bash
source /u/sw/etc/bash.bashrc
``` 

2. Once enabled, they provide the command module. Load the LIS module with:
```bash
module load gcc-glibc
module load lis
```

3. Download the [latest version](https://www.ssisc.org/lis/dl/lis-2.1.10.zip) of LIS and copy the unzipped folder into you shared folder;

4. Move into the test folder: `cd lis-2.1.10/test/`

5. Compile the file `test1.c` by typing 

```
mpicc -DUSE_MPI -I${mkLisInc} -L${mkLisLib} -llis test1.c -o test1
```

6. Run the test using the commands

```
./test1 testmat0.mtx 1 sol.txt hist.txt

./test1 testmat0.mtx testvec0.mtx sol.txt hist.txt
```

6. (b) To run the code with multiprocessors using mpi, type 

```
mpirun -n 4 ./test1 testmat0.mtx 1 sol.txt hist.txt
```

The option `-n` sets the number of processes (the default value is 4).

- Exercise:  Following the instructions available on the Lis user-guide, compile and run test2.c

## 1.1 Download matrices from the web

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
gzip -dk gr_30_30.mtx.gz

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt
```

- 2. Solve the resulting linear system using the LIS library. Explore different iterative solvers (Jacobi, Gauss-Seidel, Conjugate Gradient...).

```
mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i jacobi -maxiter 2000

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i gs -tol 1.0e-10

mpirun -n 4 ./test1 gr_30_30.mtx 2 sol.txt hist.txt -i cg 
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

- Solve the linear system $A\boldsymbol{x} = \boldsymbol{b}$ using the Conjugate Gradient method of the LIS library setting a tolerance of $10^{-13}$. 

- Report the iteration counts and the relative residual at the last iteration.

### Exercise 3

- 1. Following the instructions available on the LIS user-guide, compile and run test4.c

- 2. Modify the implementation by changing the size of the linear system to 120 and by setting the conjugate gradient method as iterative solver

- 3. Print the relative error


### Exercise 4

- 1. Following the instructions available on the LIS user-guide, compile and run test5.c

- 2. Set n = 100 and test different values of `gamma` and different iterative solvers

### Exercise 5

- Repeat Exercise 1 by considering some of the matrices available [here](https://sparse.tamu.edu/?per_page=All)

The importance of choosing a proper preconditioning technique can be observed by testing the solvers with and without preconditioners (cf. `-p option`). This will be the goal of the next labs.