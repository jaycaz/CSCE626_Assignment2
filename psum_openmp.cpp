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

#include <fstream>
#include <cmath>

#define DEFAULT_NUM_INTS 10000000
#define DEFAULT_NUM_ITERS 50
#define DEFAULT_NUM_THREADS 1


using namespace std;

/*==============================================================
 * get_elapsed (retrieve time elapsed between start, end)
 *==============================================================*/
long get_elapsed(struct timeval* start, struct timeval* end) {

  struct timeval elapsed;

  // calculate elapsed time
  if(start->tv_usec > end->tv_usec) {

    end->tv_usec += 1000000;
    end->tv_sec--;
  }
  elapsed.tv_usec = end->tv_usec - start->tv_usec;
  elapsed.tv_sec  = end->tv_sec  - start->tv_sec;

  long elapsedusec = (elapsed.tv_sec * 1000000) + elapsed.tv_usec;

  return elapsedusec;
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
    //int step = (int) pow(2.0, i);
    int step = 1 << i;

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
    //int step = (int) pow(2.0, i);
    int step = 1 << i;

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

  // Command Line Args
  int numints = DEFAULT_NUM_INTS;
  int numiters = DEFAULT_NUM_ITERS;
  int numthreads = DEFAULT_NUM_THREADS;
  bool debugmode = false;

  vector<long> prefix_sums;

  struct timeval start, end;   // gettimeofday stuff
  struct timezone tzp;
  vector<long> times;  // Store times for each trial

  // Command line arguments
  numints = DEFAULT_NUM_INTS;
  numiters = DEFAULT_NUM_ITERS;
  if(argc < 2) {
    printf("Usage: %s [nthreads] [numints] [optional: numiters] [optional: debugmode]\n\n", argv[0]);
    exit(1);
  }
  numthreads = atoi(argv[1]);
  if(argc >= 3)
  {
    numints = atoi(argv[2]);
  }
  if(argc >= 4)
  {
    numiters = atoi(argv[3]);
  }
  if(argc >= 5 && atoi(argv[4]) == 1)
  {
    debugmode = true;
  }

  omp_set_num_threads(numthreads);

  if(debugmode)
  {
    printf("\nExecuting %s: nthreads=%d, numints=%d, numiters=%d\n",
             argv[0], omp_get_max_threads(), numints, numiters);
  }

  // Allocate shared memory for new prefix sums
  prefix_sums.resize(numints);

  // Generate random ints
  {
    srand(omp_get_thread_num() * int(time(NULL)));

    for(int i = 0; i < prefix_sums.size(); i++)
    {
      int num = rand();
      prefix_sums[i] = num;
    }
  }

  if(debugmode)
  {
    printf("Calculating prefix sum...\n");
    fflush(stdout);
  }

  // Iterate <numiters> times
  for(int i = 0; i < numiters; i++)
  {
    if(debugmode)
    {
      printf("\tIteration %d...", i);
      fflush(stdout);
    }

    // Start timing
    gettimeofday(&start, &tzp);

    // Perform prefix sum
    prefix_sum(prefix_sums);

    // End timing
    gettimeofday(&end,&tzp);

    long elapsed = get_elapsed(&start, &end);
    times.push_back(elapsed);

    if(debugmode)
    {
      printf("done (%d usec).\n", elapsed);
      fflush(stdout);
    }
  }

  // Calculate average time elapsed
  long totaltime = 0;
  for(int i = 0; i < times.size(); i++)
  {
      totaltime += times[i];
  }
  double avgtime = (double) totaltime / times.size(); 

  if(debugmode)
  {
    printf("done (avg. time: %f usec).\n", avgtime);
    fflush(stdout);
  }
  else
  {
    printf("%f", avgtime);
  }

  return 0;
}


