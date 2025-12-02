# Domain Decomposition preconditioners

In this lab we want to explore the Additive Schwarz and Algebraic Multigrid methods implemented in the LIS library in order to define advanced preconditioners for sparse linear systems.

## Additive Schwarz preconditioner

Domain decomposition (DD) methods are techniques based on a decomposition of the spatial domain of the problem into several subdomains. Such reformulations are usually motivated by the need to create solvers which are easily parallelized on coarse grain parallel computers, though sometimes they can also reduce the complexity of solvers on sequential computers.

In order to obtain a domain decomposition for the linear system $Ax = b$, one needs to decompose the unknowns in the vector $x$ into subsets. 
For the sake of simplicity, we consider only a two subdomain case. Let $\mathcal{N}_i$ , $i = 1,2$ be a partition of the indices corresponding to the vector $x$ and  $R_i$ , $i = 1,2$ denote the matrix that, when applied to the vector $x$, returns only those values associated with the indices in $N_i$. The matrices $R_i$ are often referred to as the restriction operators, while $R^{\rm T}_i$ are the interpolation matrixes.

One can define the restriction of the matrix $A$ to the first and second unknowns using the restriction matrices:
$$
A_j = R_j A R^{\rm T}_j,\quad j = 1, 2.
$$
Thus the matrix $A_j$ is simply the subblock of A associated with the indices in $\mathcal{N}_j$. The preconditioned system obtained by applying the Additive Schwarz method reads
$$
(R^{\rm T}_1 A^{-1}_1 R_1 + R^{\rm T}_2 A^{-1}_2 R_2) A x = (R^{\rm T}_1 A^{-1}_1 R_1 + R^{\rm T}_2 A^{-1}_2 R_2) b.
$$
Since the sizes of the matrices $A_1$ and $A_2$ are smaller than the original matrix $A$, the inverse matrices $A_1^{-1}$ and $A_2^{-1}$ are easier to compute. Using this preconditioner for a stationary iterative method yields
$$
x^{n+1} = x^n + (R^{\rm T}_1 A^{-1}_1 R_1 + R^{\rm T}_2 A^{-1}_2 R_2) (b- Ax^n).
$$

Even if the matrices $A_i$, $i=1,2$, are easier to be inverted, in LIS it is preferred not to compute the inverse matrices exactly. Another preconditioned technique must be used together with the Additive Schwarz method in order to define the matrices $B_i$ approximating $A^{-1}_i$. The corresponding preconditioner is computed as $R^{\rm T}_1 B_1 R_1 + R^{\rm T}_2 B_2 R_2$. We can check this by the following test:

```
mpirun -n 4 ./test1 testmat0.mtx 2 sol.mtx hist.txt -i cg

mpirun -n 4 ./test1 testmat0.mtx 2 sol.mtx hist.txt -i cg -adds true
```
We observe that the option `-adds true` needed to specify the use of the Additive Schwarz method does not provide any preconditioner. In both cases, we get the following result

```
number of processes = 4
matrix size = 100 x 100 (460 nonzero entries)

initial vector x      : all components set to 0
precision             : double
linear solver         : CG
preconditioner        : none
convergence condition : ||b-Ax||_2 <= 1.0e-12 * ||b-Ax_0||_2
matrix storage format : CSR
linear solver status  : normal end
CG: number of iterations = 15
```

If instead we write `mpirun -n 4 ./test1 testmat0.mtx 2 sol.mtx hist.txt -i cg -adds true -p jacobi`, we get
```
initial vector x      : all components set to 0
precision             : double
linear solver         : CG
preconditioner        : Jacobi + Additive Schwarz
convergence condition : ||b-Ax||_2 <= 1.0e-12 * ||b-Ax_0||_2
matrix storage format : CSR
linear solver status  : normal end

CG: number of iterations = 14
```

## Some examples:

Assess the performances of the Additive Schwarz preconditioner implemented in LIS on different sparse linear systems:

```
mpirun -n 2 ./test1 testmat2.mtx 2 sol.mtx hist.txt -i gmres
mpirun -n 2 ./test1 testmat2.mtx 2 sol.mtx hist.txt -i gmres -adds true -p ssor
```
In this case the number of iterations for reaching the desired tolerance reduces from 36 to 14! 

```
mpirun -n 2 ./test1 testmat2.mtx 2 sol.mtx hist.txt -i bicgstab
mpirun -n 2 ./test1 testmat2.mtx 2 sol.mtx hist.txt -i bicgstab -adds true -p ilu -ilu_fill 2
```
In this case the number of iterations reduces from 36 to 6 and also the total elapsed time for the computation is reduced. 

We can also set the number of iterations of the Additive Schwarz method using the `adds_iter` option. The effect of this parameter can be observed in the following example.
```
mpirun -n 4 ./test2 100 100 0 sol.mtx hist.txt -i gmres -p ssor -adds true -adds_iter 2
mpirun -n 4 ./test2 100 100 0 sol.mtx hist.txt -i gmres -p ssor -adds true -adds_iter 3
mpirun -n 4 ./test2 100 100 0 sol.mtx hist.txt -i gmres -p ssor -adds true -adds_iter 4
```
The number of iterations drops from 104 to 81 (using `adds_iter = 3`) and 70 (using `adds_iter = 4`). However we can observe that the total time for the solution of the linear system is similar because the time required for computing the preconditioner increases.


### Exercise 1

Evaluate and compare the effect of the Additive Schwarz preconditioner for solving the linear systems deefined by the matrices [bcsstm12.mtx](https://suitesparse-collection-website.herokuapp.com/MM/HB/bcsstm12.tar.gz) and [nos1.mtx](https://math.nist.gov/pub/MatrixMarket2/Harwell-Boeing/lanpro/nos1.mtx.gz).

```
wget https://suitesparse-collection-website.herokuapp.com/MM/HB/bcsstm12.tar.gz
tar -xf bcsstm12.tar.gz
mv bcsstm12/bcsstm12.mtx .
rm -rf bcsstm12 bcsstm12.tar.gz 

wget https://math.nist.gov/pub/MatrixMarket2/Harwell-Boeing/lanpro/nos1.mtx.gz
gzip -dk nos1.mtx.gz 

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -maxiter 5000 -tol 1e-10

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -tol 1e-10 -p sainv 

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -tol 1e-10 -p sainv -adds true

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -tol 1e-10 -p sainv -adds true -adds_iter 2

mpirun -n 4 ./test1 bcsstm12.mtx 2 sol.txt hist.txt -i bicgstab -tol 1e-10 -p sainv -adds true -adds_iter 3
```
In this case we can see that the number of iterations required to convergence does not depend monotonically on the choice of `adds_iter`.

```
wget https://math.nist.gov/pub/MatrixMarket2/Harwell-Boeing/lanpro/nos1.mtx.gz
gzip -dk nos1.mtx.gz 

mpirun -n 4 ./test1 nos1.mtx 1 sol.txt hist.txt -i cg
mpirun -n 4 ./test1 nos1.mtx 1 sol.txt hist.txt -i cg -p ssor
mpirun -n 4 ./test1 nos1.mtx 1 sol.txt hist.txt -i cg -p ssor -adds true -adds_iter 2
mpirun -n 4 ./test1 nos1.mtx 1 sol.txt hist.txt -i cg -p jacobi -adds true -adds_iter 5
```


## Algebraic Multigrid preconditioner

Algebraic multigrid (AMG) solves linear systems based on multigrid principles, but in a way that only depends on the coefficients in the underlying matrix. The AMG method determines coarse “grids”, inter-grid transfer operators, and coarse-grid equations based solely on the matrix entries.

The AMG preconditioner implemented in LIS is called `SA-AMG` and can be specified as an option by typing `-p saamg`. Unfortunately this preconditioner is incompatible with the use of mpi for multi-processors computations. Therefore, a serial reconfiguration of the LIS installation together with a fortran compiler is needed in order to test the AMG method. 


### Exercise 2

Assess the performances of the SA-AMG preconditioner on the matrices considered in the previous examples.

```
gfortran test1.c -I${mkLisInc} -L${mkLisLib} -llis -o test1
./test1 stokes_sym.mtx 2 sol.mtx hist.txt -i cg
./test1 stokes_sym.mtx 2 sol.mtx hist.txt -i cg -p saamg
./test1 stokes_sym.mtx 2 sol.mtx hist.txt -i cg -p saamg -adds true
./test1 stokes_sym.mtx 2 sol.mtx hist.txt -i cg -p saamg -adds true -adds_iter 4

./test1 testmat2.mtx 1 sol.mtx hist.txt -i gmres
./test1 testmat2.mtx 1 sol.mtx hist.txt -i gmres -p saamg
./test1 testmat2.mtx 1 sol.mtx hist.txt -i gmres -p saamg -saamg_unsym true
./test1 testmat2.mtx 1 sol.mtx hist.txt -i gmres -p saamg -adds true


gfortran test2.c -I${mkLisInc} -L${mkLisLib} -llis -o test2
./test2 100 100 1 sol.mtx hist.txt -i cg
./test2 100 100 1 sol.mtx hist.txt -i cg -p saamg
./test2 100 100 1 sol.mtx hist.txt -i bicgstab -p saamg
./test2 100 100 1 sol.mtx hist.txt -i bicgstab -p saamg -adds true
```