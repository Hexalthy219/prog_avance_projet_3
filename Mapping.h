/***********************************************************************
 * Data structures and utils for compression
 * - Histogram
 * - Mapping
 ***********************************************************************/

#ifndef _MAPPING_H_
#define _MAPPING_H_

#include <stdint.h>

/*-----------------------------------------------------------------------------+
|                               HISTOGRAM                                      |
+-----------------------------------------------------------------------------*/
typedef struct
{
    size_t length;                  // Length of the histogram
    unsigned long long *count;      // Array of occurences

} Histogram;


/***********************************************************************
 * Create an empty histogram; all count will be set to zero.
 *
 * PARAMETERS
 * length       The histogram size
 *
 * RETURN
 * histogram    A pointer to an Histogram. It must be deleted by
 *              calling `freeHistogram`
 * NULL         In case of error
 ***********************************************************************/
Histogram* createEmptyHistogram(size_t length);


/***********************************************************************
 * Free the memory allocated by this histogram
 *
 * PAREMETERS
 * hist         A pointer to a Histogram
 ***********************************************************************/
void freeHistogram(Histogram* hist);


/*-----------------------------------------------------------------------------+
|                                MAPPING                                       |
+-----------------------------------------------------------------------------*/
typedef struct
{
    size_t nLevels;        // The number of levels after compression (k)
    size_t *thresholds;    // Array containing the thresholds (p_1, ..., p_k=n)
    uint16_t *levels;      // Values between thresholds (v_1, ..., v_k)

} Mapping;



/***********************************************************************
 * Create a unitiliazed Mapping. Only `nLevels` will be set.
 *
 * PARAMETERS
 * nLevels     The number of levels the mapping should have
 *
 * RETURN
 * mapping     A pointer to a Mapping. It must be deleted by calling
 *             `freeMapping`
 * NULL        In case of error
 ***********************************************************************/
Mapping* createUninitializedMapping(size_t nLevels);

/***********************************************************************
 * Free the memory allocated by the mapping
 *
 * PAREMETERS
 * mapping      A pointer to a Mapping
 ***********************************************************************/
void freeMapping(Mapping *mapping);


/*************************************************************************
 * Compute the lookup table associated with the mapping
 * The compressed images must be free with `freeImage`.
 *
 * PARAMETERS
 * mapping      A valid pointer to a Mapping
 * maxValue     The maximum value an image on which the mapping will be
 *              used can take
 *
 * RETURN
 * lookUpTable  The lookup table or NULL in case of error
 *************************************************************************/
uint16_t* mapping2Lookup(const Mapping *mapping, uint16_t maxValue);


/*************************************************************************
 * Compute the error of using the given Mapping on the given Histogram.
 *
 * PARAMETERS
 * mapping            A valid pointer to a Mapping
 * originalHistogram  A valid pointer to an Histogram
 *
 * RETURN
 * error              The error associated to the compression or DBL_MAX
 *                    in case of error.
 *************************************************************************/
double computeError(const Mapping *mapping, const Histogram *originalHistogram);






#endif // !_MAPPING_H_
