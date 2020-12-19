#include <stddef.h>
#include <stdlib.h>
#include <float.h>

#include "Mapping.h"

/*-----------------------------------------------------------------------------+
|                               HISTOGRAM                                      |
+-----------------------------------------------------------------------------*/
Histogram* createEmptyHistogram(size_t length)
{
    unsigned long long *count = calloc(length, sizeof(unsigned long long));
    Histogram* hist = malloc(sizeof(Histogram));
    if(!hist || !count)
    {
        free(hist);
        free(count);
        return NULL;
    }

    hist->length = length;
    hist->count = count;

    return hist;
}



void freeHistogram(Histogram* hist){
    if(!hist) return;
    free(hist->count);
    free(hist);
}



/*-----------------------------------------------------------------------------+
|                                MAPPING                                       |
+-----------------------------------------------------------------------------*/

Mapping* createUninitializedMapping(size_t nLevels)
{
    Mapping *mapping = malloc(sizeof(Mapping));
    size_t *thresholds = calloc(nLevels, sizeof(size_t));
    uint16_t *levels = calloc(nLevels, sizeof(uint16_t));
    if(!mapping || !thresholds || !levels)
    {
        free(mapping);
        free(thresholds);
        free(levels);
        return NULL;
    }

    mapping->nLevels = nLevels;
    mapping->thresholds = thresholds;
    mapping->levels = levels;

    return mapping;
}


void freeMapping(Mapping *mapping)
{
    if(!mapping)return;
    free(mapping->thresholds);
    free(mapping->levels);
    free(mapping);
}


uint16_t* mapping2Lookup(const Mapping *mapping, uint16_t maxValue)
{
    if(!mapping)
        return NULL;

    uint16_t* lookUpTable = calloc((maxValue+1), sizeof(uint16_t));
    if(!lookUpTable)
        return NULL;

    size_t k = 0;
    for(size_t i=0; i<(size_t)(maxValue+1); i++)
    {
        if(mapping->thresholds[k] == i)
            k++;
        lookUpTable[i] = mapping->levels[k];
    }

    return lookUpTable;


}



double computeError(const Mapping *mapping, const Histogram *originalHistogram)
{
    if(!mapping || !originalHistogram)
        return DBL_MAX;

    uint16_t *lookUpTable = mapping2Lookup(mapping, originalHistogram->length-1);
    if(!lookUpTable)
        return DBL_MAX;

    double err = 0, oldValue, newValue, delta;
    for(size_t i=0; i<originalHistogram->length; i++)
    {
        oldValue = i;
        newValue = lookUpTable[i];
        delta = oldValue - newValue;
        err += (originalHistogram->count[i] * (delta*delta)) ;
    }

    free(lookUpTable);
    return err;


}
