/***********************************************************************
 * Utility to measure compression time
 * gcc emp_time.c naive_compression.c Mapping.c --std=c99 --pedantic -Wall -Wextra -Wmissing-prototypes -DNDEBUG -lm -o timeit
 ***********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Histogram.h"
#include "compression.h"

/*-----------------------------------------------------------------------------+
|                          HISTOGRAM GENERATION                                |
+-----------------------------------------------------------------------------*/
static Histogram* histoGen(size_t length, unsigned long long totalCount)
{
    Histogram *hist = createEmptyHistogram(length);
    if(!hist)
        return NULL;

    // Distribute occurences uniformly with excess on first slot
    unsigned long long perBin = totalCount / length;
    hist->count[0] = totalCount - (perBin * length);
    for(size_t i=0; i<length; i++)
        hist->count[i] += perBin;

    // Perturb the distribution
    // 1. remove randomly from each bin
    unsigned long long excess = 0, rem;
    for(size_t i=0; i<length; i++)
    {
        rem = rand() % hist->count[i];
        hist->count[i] -= rem;
        excess += rem;
    }
    fprintf(stderr, "Excess: %llu\n", excess); // TODO

    // 2. redistribute excess as uniformly as possible
    perBin = excess / length;
    for(size_t i=0; i<length; i++)
        hist->count[i] += perBin;
    excess = excess - (perBin * length);

    while(excess-- > 0)
            hist->count[rand() % length] += 1;

    return hist;
}



/*-----------------------------------------------------------------------------+
|                                  MAIN                                        |
+-----------------------------------------------------------------------------*/
/***********************************************************************
 * Measure the time needed to compute the mapping for the compression
 *
 * PARAMETERS
 * histogram   A valid pointer to an Histogram
 * nLevels     The number of levels for the compression
 *
 * RETURN
 * duration    The duration of the computation in seconds
 ***********************************************************************/
static double cpuTimeUsed(const Histogram* histogram, size_t nLevels)
{
    clock_t start = clock();
    Mapping* mapping = computeMapping(histogram, nLevels);
    clock_t end = clock();
    freeMapping(mapping);

    return ((double) (end - start)) / CLOCKS_PER_SEC;
}

int main(void)
{
    srand(time(NULL));//Use an integer seed to get a fix sequence

    /*
     * Do your experiment here. You can use `histoGen` to generate histograms
     * and cpuTimeUsed to measure the time needed to build the mapping
     */


    return EXIT_SUCCESS;
}

