/*
 *  sum_mpi.cpp - Demonstrates parallelism via random fill and sum routines.
 *                This program uses MPI.
 */

/*---------------------------------------------------------
 *  Parallel Summation 
 *
 *  1. each processor generates mynumints random integers (in parallel)
 *  2. each processor sums his mynumints random integers (in parallel)
 *  3  Time for processor-wise sums
 *  3.1  All the processes send their sum to Processor 0
 *  3.2  Processor 0 receives the local sum from all the other processes.
 *  3.3  Processor 0 adds up the processor-wise sums (sequentially)
 *  3.4  Processor 0 sends the result to all other processors
 *
 *---------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include <fstream>
#include <cmath>

using namespace std;

/*==============================================================
 * p_generate_random_ints (processor-wise generation of random ints)
 *==============================================================*/
void p_generate_random_ints(vector<long>& memory, int n, int id) {
  
  // generate & write this processor's random integers
  for (int i = 0; i < n; ++i) {

    //memory.push_back(rand());
    memory.push_back((n*id) + (i+1));
  }
  return;
}

/*==============================================================
 * print_elapsed (prints timing statistics)
 *==============================================================*/
 void print_elapsed(const char* desc, struct timeval* start, struct timeval* end) {

  struct timeval elapsed;
  // calculate elapsed time
  if(start->tv_usec > end->tv_usec) {

    end->tv_usec += 1000000;
    end->tv_sec--;
  }
  elapsed.tv_usec = end->tv_usec - start->tv_usec;
  elapsed.tv_sec  = end->tv_sec  - start->tv_sec;

  printf("\n %s total elapsed time = %ld (usec)\n",
    desc, (elapsed.tv_sec*1000000 + elapsed.tv_usec));
}

void seq_prefix_sum(int id, vector<long> &data)
{
    int sum = 0;
    
    for(int i = 0; i < data.size(); i++)
    {
        sum += data[i];
        data[i] = sum;
    }
    printf("Node %d completed sequential prefix sum\n", id);
}

void pass_prev_sums(int id, vector<long> &data, long *sum)
{
    MPI_Status status;
    int p; // number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if(id > 0)
    {
        // Wait to receive local sum from lower neighbor proc
        MPI_Recv(sum, 1, MPI_LONG, id-1, 0, MPI_COMM_WORLD, &status);
        //printf("Node %d received sum %d from node %d\n", id, *sum, id-1);

        // Calculate new sum
        long newsum = *sum;
        newsum += data[data.size() - 1];
        *sum = newsum;

        // Pass new sum to upper neighbor proc
        if(id < p-1)
        {
          MPI_Send(sum, 1, MPI_LONG, id+1, 0, MPI_COMM_WORLD);
          //printf("Node %d sent sum %d to node %d\n", id, *sum, id+1);
        }
    }
    else
    {
        // Master proc, send local sum to neighbor proc
        *sum = data[data.size() - 1];

        // sums will cascade upwards through neighbor procs
        MPI_Send(sum, 1, MPI_LONG, 1, 0, MPI_COMM_WORLD);
        //printf("Master node sent sum %d to node 1.\n", *sum);
    }

    //*sum = 0;
}

void sweep_prev_sum(int id, vector<long> &data, long *sum)
{
    *sum = *sum - data[data.size() - 1];

    if(id > 0)
    {
        for(int i = 0; i < data.size(); i++)
        {
            data[i] += *sum;
        }
    }
}

void prefix_sum(int id, vector<long> &data, long *buffer)
{
    seq_prefix_sum(id, data);
    pass_prev_sums(id, data, buffer);
    sweep_prev_sum(id, data, buffer);
}


/*==============================================================
 * check_sums (Verifies prefix sum algorithm works with data)
 *==============================================================*/
bool check_sums(const vector<long> &data, const vector<long> &prefix_sums, long offset)
{
  long sum = offset;

  for(int i = 0; i < data.size(); i++)
  {
    sum += data[i];
    if(prefix_sums[i] != sum)    
    {
      printf("Algorithm incorrect at index %d:\n", i);
      printf("\tExpected: %d; Actual: %d\n", sum, data[i]);
      return false;
    }
  }

  return true;
}

void write_all_data(int id, const vector<long> &data, fstream &f)
{
    char baton = 0;
    MPI_Status status;
    int p; // Total num processors
    MPI_Comm_size(MPI_COMM_WORLD, &p); // Get number of processors

    // Non-master nodes: wait to receive baton from prev node
    if(id > 0)
    {
        MPI_Recv(&baton, 1, MPI_BYTE, id-1, 0, MPI_COMM_WORLD, &status);
    }

    // Master node: print data
    for(int i = 0; i < data.size(); i++)
    {
        f << data[i] << endl;
    }

    // Pass baton to next node
    if(id < p-1)
    {
        MPI_Send(&baton, 1, MPI_BYTE, id+1, 0, MPI_COMM_WORLD);
    }
}


/*==============================================================
 *  Main Program (Parallel Summation)
 *==============================================================*/
int main(int argc, char **argv) {

  int nprocs, totalnumints; // command line args

  int my_id, mynumints;

  long sum;             // sum of each individual processor
  long total_sum;       // Total sum 

  fstream datafile, psumfile; // Streams to data storage files
  vector<long> mymemory; // Vector to store processes numbers       
  long* buffer;         // Buffer for inter-processor communication

  struct timeval gen_start, gen_end; // gettimeofday stuff
  struct timeval start, end;         // gettimeofday stuff
  struct timezone tzp;

  MPI_Status status;              // Status variable for MPI operations

  /*---------------------------------------------------------
   * Initializing the MPI environment
   * "nprocs" copies of this program will be initiated by MPI.
   * All the variables will be private, only the program owner could see its own variables
   * If there must be a inter-procesor communication, the communication must be
   * done explicitly using MPI communication functions.
   *---------------------------------------------------------*/

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id); // Getting the ID for this process

  /*---------------------------------------------------------
   *  Read Command Line
   *  - check usage and parse args
   *---------------------------------------------------------*/

  if(argc < 2) {

    if(my_id == 0)
      printf("Usage: %s [numints]\n\n", argv[0]);

    MPI_Finalize();
    exit(1);
  }

  totalnumints       = atoi(argv[1]);

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // Get number of processors

  if(my_id == 0)
    printf("\nExecuting %s: nprocs=%d, totalnumints=%d\n",
            argv[0], nprocs, totalnumints);

  /*---------------------------------------------------------
   *  Initialization
   *  - allocate memory for work area structures and work area
   *---------------------------------------------------------*/

  if(my_id == 0)
  {
    datafile.open("data.txt", fstream::out | fstream::trunc);
    datafile.close();
    psumfile.open("psums.txt", fstream::out | fstream::trunc);
    psumfile.close();
  }

  datafile.open("data.txt", fstream::out | fstream::app);
  psumfile.open("psums.txt", fstream::out | fstream::app);

  // Determine how many ints to allocate for this node
  // Scheme: give each <totalnumints / nprocs>, 
  //    then give one extra to the first <totalnumints % nprocs> nodes
  if(my_id < totalnumints % nprocs)
  {
      mynumints = ceil((double) totalnumints / nprocs);
  }
  else
  {
      mynumints = floor((double) totalnumints / nprocs);
  }

  printf("Processor %d is allocating %d nodes.\n", my_id, mynumints);
  mymemory.reserve(mynumints);
  buffer = (long *) malloc(sizeof(long));

  if(buffer == NULL) {

    printf("Processor %d - unable to malloc()\n", my_id);
    MPI_Finalize();
    exit(1);
  }

  // Generate data
  gettimeofday(&gen_start, &tzp);
  srand(my_id + time(NULL));                  // Seed rand functions
  p_generate_random_ints(mymemory, mynumints, my_id);  // random parallel fill
  gettimeofday(&gen_end, &tzp);

  if(my_id == 0) {

    print_elapsed("Input generated", &gen_start, &gen_end);
  }


  // Write out data before prefix sum
  write_all_data(my_id, mymemory, datafile);
  datafile.close();
  if(my_id == 0)
  {
    printf("Initial data written to 'data.txt'.\n");
  }

  MPI_Barrier(MPI_COMM_WORLD); // Global barrier

  // Start timing
  gettimeofday(&start, &tzp);

  // Perform prefix sum
  prefix_sum(my_id, mymemory, buffer);

  // End timing
  gettimeofday(&end,&tzp);


  // Write out data after prefix sum
  write_all_data(my_id, mymemory, psumfile);
  psumfile.close();

  MPI_Barrier(MPI_COMM_WORLD); // Global barrier
  if(my_id == 0);
  {
    printf("Prefix sums written to 'psums.txt'.\n");
    print_elapsed("Prefix Sum", &start, &end);
  }


  /*---------------------------------------------------------
   *  Cleanup
   *  - free memory
   *---------------------------------------------------------*/

  // free memory
  free(buffer);

  MPI_Finalize();

  return 0;
} // main()
