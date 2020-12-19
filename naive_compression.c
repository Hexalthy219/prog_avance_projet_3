/***********************************************************************
 * Implementation of a naive algorithm that compress an image with
 * equal bins.
 ***********************************************************************/

#include "compression.h"

Mapping* computeMapping(const Histogram* histogram, size_t nLevels)
{
    Mapping *mapping = createUninitializedMapping(nLevels);
    if(!mapping)
        return NULL;

    double binWidth = histogram->length / (double)nLevels;
    double shift = binWidth / 2.;
    double t;

    for(size_t i=0; i<nLevels; i++)
    {
        t = (i+1) * binWidth;
        mapping->thresholds[i] = (size_t)t;
        mapping->levels[i] = (size_t)(t-shift);
    }

    return mapping;

}
