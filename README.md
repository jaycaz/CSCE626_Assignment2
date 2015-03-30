# CSCE626_Assignment2
A parallel implementation of the prefix sum algorithm, using OpenMP and MPI. Now with sequential algorithm and full experiemental setup.

## Instructions

### OpenMP
**Usage:** ./psums_openmp *numprocs* *numints* [*numiters*] [*debugmode*]
- numprocs: Number of processors to run with
- numints: Number of integers to generate
- numiters (Optional): number of iterations to run
- debugmode (Optional): option to print debug information during runtime
  * 0: no
  * 1: yes


## MPI
**Usage:** mpirun -np *numprocs* psum_mpi *numints* [*numiters*] [*debugmode*]
- Arguments are the same as above

