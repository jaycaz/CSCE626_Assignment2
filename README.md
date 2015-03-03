# CSCE626_Assignment1
A parallel implementation of the prefix sum algorithm, using OpenMP and MPI

## Instructions

### OpenMP
**Usage:** ./psums_openmp *numprocs* *numints* [*writedata*]
- numprocs: Number of processors to run with
- numints: Number of integers to generate
- writedata (Optional): Option to write out data.
  * 0: do not write data
  * 1: write data


## MPI
**Usage:** mpirun -np *numprocs* psum_mpi *numints* [*writedata*]
- Arguments are the same as above

