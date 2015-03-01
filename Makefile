#
# makefile for prefix sum programs
#
# prompt> make
#
CC     = g++     # the c compiler to use
MPICC  = mpic++  # the MPI cc compiler
CFLAGS = -O3     # optimize code
DFLAGS =         # common defines

default:all

all: psum_openmp psum_mpi psum_mpi2

#
# OpenMP prefix sum program
#

psum_openmp:psum_openmp.cpp
	$(CC) $(CFLAGS) $(DFLAGS) -fopenmp  -o $@ $@.cpp

#
# MPI prefix sum program
#

psum_mpi:psum_mpi.cpp
	$(MPICC) $(CFLAGS) $(DFLAGS) -o $@ $@.cpp

psum_mpi2:psum_mpi2.cpp
	$(MPICC) $(CFLAGS) $(DFLAGS) -o $@ $@.cpp

#
# clean up
#
clean:
	rm psum_openmp psum_mpi psum_mpi2 > /dev/null 2>&1
