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

  printf("\n %s total elapsed time = %ld (usec)",
    desc, (elapsed.tv_sec*1000000 + elapsed.tv_usec) / niters);
}

// Functor to sum the numbers
template<typename T>
struct sum_functor {

  // Constructor
  sum_functor() : m_sum(0) {
  }

  void operator() (int& num) {
    m_sum +=num;
  }

  T get_sum() const {
    return m_sum;
  }

  protected:

  T m_sum;
};

void up_sweep(vector<long> &data)
{
  int n = data.size();
  int h = (int) log2(n);

  for(int i = 1; i <= h; i++)
  {
    int step = pow(2,i);

    for(int j = n-1; j >= 0; j -= step)
    {
      data[j] += data[j - step/2];
    }
  }
}

void down_sweep(vector<long> &data)
{
  int n = data.size();
  int h = (int) log2(n);

  for(int i = h-1; i > 0; i--)
  {
    int step = pow(2, i);
    for(int j = step-1; j < n-2; j += step)
    {
      data[j + step/2] += data[j];
    }
  }
  
}

void prefix_sum(vector<long> &data)
{
  //up_sweep(data, results);
  //down_sweep(data, results);
  up_sweep(data);
  down_sweep(data);
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

  /* Allocate shared memory, enough for each thread to have numints*/
  data.resize(numints);

  /* Allocate shared memory for partial_sums */
  //partial_sums.resize(numints);

  /*****************************************************
   * Generate the random ints in parallel              *
   *****************************************************/

  #pragma omp parallel for

    for(int i = 0; i < data.size(); i++)
    {
      //data[i] = rand();
      data[i] = i+1;
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

  //prefix_sum(data, results);
  prefix_sum(data);

  /*
  for(int iteration=0; iteration < numiterations; ++iteration) {

    #pragma omp parallel shared(numints,data,partial_sums,total_sum)
    {
      int tid;

      // get the current thread ID in the parallel region
      tid = omp_get_thread_num();

      // Compute the local partial sum
      long partial_sum = 0;

      vector<int>::iterator it_cur = data.begin();
      std::advance(it_cur, tid * numints);

      vector<int>::iterator it_end = it_cur;
      std::advance(it_end, numints);

      sum_functor<long> result = std::for_each(it_cur, it_end, sum_functor<long>());

      // Write the partial result to share memory
      partial_sums[tid] = result.get_sum();
    }

    // Compute the sum of the partial sums
    total_sum = 0;
    int max_threads = omp_get_max_threads();
    for(int i = 0; i < max_threads ; ++i) {

      total_sum += partial_sums[i];
    }
  }
  */

  gettimeofday(&end,&tzp);

  printf("Prefix sums:\n");
  for(int i = 0; i < data.size(); i++) printf("%ld, ", data[i]);
  printf("\n");
  /*****************************************************
   * Output timing results                             *
   *****************************************************/

  //print_elapsed("Summation", &start, &end, numiterations);
  //printf("\n Total sum = %6ld\n", total_sum);

  return 0;
}

