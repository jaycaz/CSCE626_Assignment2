#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "omp.h"

#define NUM_THREADS 4

using namespace std;

int main(int argc, char **argv)
{
    // Set number of threads
    omp_set_num_threads(NUM_THREADS);

    vector<int> data;
    int num = atoi(argv[1]);

    #pragma omp parallel for
    for(int i = 0; i < num; i++)
    {
        printf("Thread %d claiming number %d\n", omp_get_thread_num(), i);
    }

    return 0;
}
