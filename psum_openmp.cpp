/*
 *  sum_openmp.cpp - Demonstrates parallelism via random fill and sum routines
 *                   This program uses OpenMP.
 */

/*---------------------------------------------------------
 *  Parallel Prefix Summation
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

using namespace std;

/*==============================================================
 * print_elapsed (prints timing statistics)
 *==============================================================*/
void print_elapsed(const char* desc, struct timeval* start, struct timeval* end, int niters) {

  struct timeval elapsed;
  /* calculate elapsed time */
  if(start->tv_usec > end->tv_usec) {

    end->tv_usec += 1000000;
    end->tv_sec--;
  }
  elapsed.tv_usec = end->tv_usec - start->tv_usec;
  elapsed.tv_sec  = end->tv_sec  - start->tv_sec;

  printf("\n %s total elapsed time = %ld (usec)\n",
    desc, (elapsed.tv_sec*1000000 + elapsed.tv_usec) / niters);
}

void up_sweep(vector<long> &nums)
{
  int n = nums.size();
  int h = (int) log2(n);

  for(int i = 1; i <= h; i++)
  {
    int step = pow(2,i);

    for(int j = n-1; j >= 0; j -= step)
    {
      nums[j] += nums[j - step/2];
    }
  }
}

void down_sweep(vector<long> &nums)
{
  int n = nums.size();
  int h = (int) log2(n);

  for(int i = h-1; i > 0; i--)
  {
    int step = pow(2, i);
    for(int j = step-1; j < n-2; j += step)
    {
      nums[j + step/2] += nums[j];
    }
  }
  
}

void prefix_sum(vector<long> &nums)
{
  up_sweep(nums);
  down_sweep(nums);
}

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
 *  Main Program (Parallel Summation)
 *==============================================================*/
int main(int argc, char *argv[]) {

  // Initialize values

  int numints = 0;
  int numiterations = 0;

  vector<long> data;
  vector<long> prefix_sums;
  //vector<long> results;

  struct timeval start, end;   /* gettimeofday stuff */
  struct timezone tzp;

  if( argc < 3) {
    printf("Usage: %s [numints] [numiterations]\n\n", argv[0]);
    exit(1);
  }

  numints       = atoi(argv[1]);
  numiterations = atoi(argv[2]);

  printf("\nExecuting %s: nthreads=%d, numints=%d, numiterations=%d\n",
            argv[0], omp_get_max_threads(), numints, numiterations);

  // Allocate shared memory, enough for each thread to have numints
  data.resize(numints);

  // Allocate shared memory for prefix_sums
  prefix_sums.resize(numints);

  /*****************************************************
   * Generate the random ints in parallel              *
   *****************************************************/

  #pragma omp parallel for

    for(int i = 0; i < data.size(); i++)
    {
      int num = i+1;
      data[i] = num;
      prefix_sums[i] = num;
    }

  printf("Data:\n");
  for(int i = 0; i < data.size(); i++) printf("%ld, ", data[i]);
  printf("\n");


  /*****************************************************
   * Generate the sum of the ints in parallel          *
   * NOTE: Repeated for numiterations                  *
   *****************************************************/
  // Begin timing
  gettimeofday(&start, &tzp);

  prefix_sum(prefix_sums);

  gettimeofday(&end,&tzp);

  // Display results
  printf("Prefix sums:\n");
  for(int i = 0; i < data.size(); i++) printf("%ld, ", prefix_sums[i]);
  printf("\n");

  printf("Checking correctness...\n");
  if(check_sums(data, prefix_sums))
  {
    printf("Correctness confirmed!\n");
  }
  else
  {
    printf("Algorithm is not correct.\n");
  }

  /*****************************************************
   * Output timing results                             *
   *****************************************************/

  print_elapsed("Summation", &start, &end, numiterations);

  return 0;
}

