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

#define DEFAULT_NUM_INTS 10000000
#define DEFAULT_
#define DEFAULT_NUM_ITERS 1

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

  // Command Line Args
  int numints = DEFAULT_NUM_INTS;
  int numiters = DEFAULT_NUM_ITERS;
  bool debugmode = false;

  vector<long> data;
  vector<long> prefix_sums;

  struct timeval start, end;   // gettimeofday stuff
  struct timezone tzp;
  vector<long> times;  // Store times for each trial

  // Command line arguments
  numints = DEFAULT_NUM_INTS;
  numiters = DEFAULT_NUM_ITERS;
  if(argc < 2) {
    printf("Usage: %s [numints] [optional: numiters] [optional: debugmode]\n\n", argv[0]);
    exit(1);
  }
  if(argc >= 2)
  {
    numints = atoi(argv[1]);
  }
  if(argc >= 3)
  {
    numiters = atoi(argv[2]);
  }
  if(argc >= 4 && atoi(argv[3]) == 1)
  {
    debugmode = true;
  }

  if(debugmode)
  {
    printf("\nExecuting %s: numints=%d, numiters=%d\n",
             argv[0], numints, numiters);
  }

  // Allocate shared memory for original data and new prefix sums
  data.resize(numints);
  prefix_sums.resize(numints);

  // Generate random ints
  {
    for(int i = 0; i < data.size(); i++)
    {
      int num = rand();
      data[i] = num;
      prefix_sums[i] = num;
    }
  }

  // Write out initial data
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

  // Write out prefix sum data
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
