/*---------------------------------------------------------
 *  Parallel Prefix Summation (using OpenMP)
 *
 *  1. An array of $numints random integers is created, initialized with random values
 *  2. The up-sweep is performed, splitting tasks among threads as evenly as possible
 *  3. The down-sweep is performed, splitting tasks among threads as evenly as possible
 *
 *---------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <omp.h>
#include <vector>
#include <algorithm>

#include <cmath>

//#include "papi.h"

using namespace std;

/*==============================================================
 * print_elapsed (prints timing statistics)
 *==============================================================*/
void print_elapsed(const char* desc, struct timeval* start, struct timeval* end)
{
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

/*==============================================================
 * up_sweep (performs up-sweep step of prefix sum alg.)
 *==============================================================*/
void up_sweep(vector<long> &nums)
{
  int n = nums.size();
  int nceil = (int) pow(2, ceil(log2(n)));
  int h = (int) log2(nceil);

  for(int i = 1; i <= h; i++)
  {
    int step = (int) pow(2.0, i);

    #pragma omp parallel for 
      for(int j = step-1; j < n; j += step)
      {
        //printf("Up-sweep %d: Thread %d working on value %d\n", i, omp_get_thread_num(), j);
        //fflush(stdout);
        nums[j] += nums[j - step/2];
      }
  }
}

/*==============================================================
 * down_sweep (performs down-sweep step of prefix sum alg.)
 *==============================================================*/
void down_sweep(vector<long> &nums)
{
  int n = nums.size();
  int nceil = (int) pow(2, ceil(log2(n)));
  int h = (int) log2(nceil);

  for(int i = h-1; i > 0; i--)
  {
    int step = (int) pow(2.0, i);

    #pragma omp parallel for
      for(int j = step-1; j < n - step/2; j += step)
      {
        //printf("Down-sweep %d: Thread %d working on value %d\n", i, omp_get_thread_num(), j);
        //fflush(stdout);
        nums[j + step/2] += nums[j];
      }
  }
  
}

/*==============================================================
 * prefix_sum (combines up-sweep and down-sweep steps)
 *==============================================================*/
void prefix_sum(vector<long> &nums)
{
  up_sweep(nums);
  down_sweep(nums);
}

/*==============================================================
 * check_sums (Verifies prefix sum algorithm works with data)
 *==============================================================*/
bool check_sums(const vector<long> &data, const vector<long> &prefix_sums)
{
  long sum = 0;

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

/*==============================================================
 *  Main Program (Parallel Prefix Summation)
 *==============================================================*/
int main(int argc, char *argv[]) {

  // Initialize values

  int numints = 0;
  int numthreads = 1;

  vector<long> data;
  vector<long> prefix_sums;

  struct timeval start, end;   // gettimeofday stuff
  struct timezone tzp;

  // Command line arguments

  if( argc < 3) {
    printf("Usage: %s [nthreads] [numints] \n\n", argv[0]);
    exit(1);
  }

  numthreads = atoi(argv[1]);
  numints = atoi(argv[2]);

  omp_set_num_threads(numthreads);

  printf("\nExecuting %s: nthreads=%d, numints=%d\n",
            argv[0], omp_get_max_threads(), numints);

  // Allocate shared memory for original data and new prefix sums
  printf("Allocating %ld bytes of input memory...", numints * sizeof(int) * 2);
  fflush(stdout);
  data.resize(numints);
  prefix_sums.resize(numints);
  printf("done.\n");

  // Generate random ints in parallel
  printf("Generating input data...");
  fflush(stdout);
  //#pragma omp parallel
  {
    srand(omp_get_thread_num() * int(time(NULL)));

    //#pragma omp for
    for(int i = 0; i < data.size(); i++)
    {
      int num = rand();
      data[i] = num;
      prefix_sums[i] = data[i];
    }
  }

  printf("done.\n");

  printf("Calculating prefix sum...");
  fflush(stdout);

  // Begin timing
  gettimeofday(&start, &tzp);

  // Calculate prefix sums for generated data
  prefix_sum(prefix_sums);

  // End timing
  gettimeofday(&end,&tzp);

  printf("done.\n");

  // Display checksum results
  printf("Checking correctness...");
  fflush(stdout);
  if(check_sums(data, prefix_sums))
  {
    printf("Correctness confirmed!\n");
  }
  else
  {
    printf("Algorithm is not correct.\n");
  }
  printf("done.\n");

  // Display timing results
  print_elapsed("Prefix Sum", &start, &end);

  return 0;
}

