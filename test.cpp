#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "omp.h"

#define NUM_THREADS 4

using namespace std;

int main()
{
    // Set number of threads
    omp_set_num_threads(NUM_THREADS);

    vector<int> data;

    #pragma omp parallel for
    for(vector<int>::iterator it = data.begin(); it != data.end(); it++)
    {
        printf("Thread %d filling num %d\n", omp_get_thread_num(), it);
        *it = rand();
    }

    return 0;
}
