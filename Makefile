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

all: psum_seq psum_openmp psum_mpi psum_mpi2 

#
# One-thread version of prefix sum program
#

psum_seq:psum_seq.cpp
	$(CC) $(CFLAGS) $(DFLAGS) -fopenmp  -o $@ $@.cpp

#
# OpenMP prefix sum program
#

psum_openmp:psum_openmp.cpp
	$(CC) $(CFLAGS) $(DFLAGS) -fopenmp  -o $@ $@.cpp
	cp $@ ompjobs

#
# MPI prefix sum program
#

psum_mpi:psum_mpi.cpp
	$(MPICC) $(CFLAGS) $(DFLAGS) -o $@ $@.cpp
	cp $@ mpijobs

psum_mpi2:psum_mpi2.cpp
	$(MPICC) $(CFLAGS) $(DFLAGS) -o $@ $@.cpp
	cp $@ mpi2jobs

#
# clean up
#
clean:
	rm psum_seq > /dev/null 2>&1
	rm psum_openmp ompjobs/psum_openmp > /dev/null 2>&1
	rm psum_mpi mpijobs/psum_mpi > /dev/null 2>&1
	rm psum_mpi2 mpi2jobs/psum_mpi2 > /dev/null 2>&1
