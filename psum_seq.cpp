/*---------------------------------------------------------
 *  Sequential Prefix Summation
 *---------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>

#include <cmath>

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
 * prefix_sum (combines up-sweep and down-sweep steps)
 *==============================================================*/
void prefix_sum(vector<long> &nums)
{
    long sum = 0;

    for(int i = 0; i < nums.size(); i++)
    {
        sum += nums[i];
        nums[i] = sum;
    }
}

/*==============================================================
 *  Main Program (Parallel Prefix Summation)
 *==============================================================*/
int main(int argc, char *argv[]) {

  // Initialize values

  int numints = 0;

  vector<long> data;

  struct timeval start, end;   // gettimeofday stuff
  struct timezone tzp;

  // Command line arguments

  if( argc < 2) {
    printf("Usage: %s [numints] \n\n", argv[0]);
    exit(1);
  }

  numints = atoi(argv[1]);

  printf("\nExecuting %s: numints=%d\n",
            argv[0], 1, numints);

  // Allocate shared memory for original data and new prefix sums
  printf("Allocating %ld bytes of input memory...", numints * sizeof(int) * 2);
  fflush(stdout);
  data.resize(numints);
  printf("done.\n");

  // Generate random ints in parallel
  printf("Generating input data...");
  fflush(stdout);
  for(int i = 0; i < data.size(); i++)
  {
    int num = rand();
    data[i] = num;
    data[i] = num;
  }
  printf("done.\n");

  printf("Calculating prefix sum...");
  fflush(stdout);

  // Begin timing
  gettimeofday(&start, &tzp);

  // Calculate prefix sums for generated data
  prefix_sum(data);

  // End timing
  gettimeofday(&end,&tzp);

  printf("done.\n");

  // Display timing results
  print_elapsed("Prefix Sum", &start, &end);

  return 0;
}

