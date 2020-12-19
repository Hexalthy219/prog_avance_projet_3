/***********************************************************************
 * Compute the mapping to do the compression.
 ***********************************************************************/

#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "Mapping.h"


/*************************************************************************
 * Compute the mapping which represents the image compression.
 *
 * PARAMETERS
 * img          A valid pointer to a PGM structure (the image to compress)
 *
 * RETURN
 * mapping     A pointer to a Mapping. It must be deleted by calling
 *             `freeMapping`
 * NULL        In case of error
 *************************************************************************/
Mapping* computeMapping(const Histogram* histogram, size_t nLevels);



#endif // !_COMPRESSION_H_

