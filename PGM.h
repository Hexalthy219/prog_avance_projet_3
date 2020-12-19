/***********************************************************************
 * Representation of grayscale image.
 * PGM stands for Portable Gray Map.
 * File format specification: http://netpbm.sourceforge.net/doc/pgm.html
 *
 * Note: if you have trouble with one of the type (ascii/binary) try the
 * other. There seems to be some portability issues.
 ***********************************************************************/

#ifndef _PGM_H_
#define _PGM_H_

#include <stddef.h>
#include <stdint.h>

/* Types */

/* File encoding of an image */
typedef enum
{
  ASCII = 2,
  BINARY = 5
} PGMType;

/* Representation of a PGM image */
typedef struct
{
  PGMType type;     // Encoding format (ASCII or BINARY)
  size_t width;                 // Number of columns of array
  size_t height;                // Number of rows of array
  uint16_t maxValue;            // Maximum gray value (do not edit)
  uint16_t** array;             // Image of size 'height x width'
} PGM;

/* Functions */

/***********************************************************************
 * Create an image from a file.
 * The image must later be deleted by calling deleteImage().
 *
 * PARAMETERS
 * filename     File name of a pgm image
 *
 * RETURN
 * NULL         if any error
 * image        The read image
 ***********************************************************************/
PGM* createImageFromFile(const char* filename);

/***********************************************************************
 * Save an image to a file.
 *
 * PARAMETERS
 * image        The image to save
 * filename     Destination file name
 *
 * RETURN
 * 0            If no error
 * non-0        Otherwise
 ***********************************************************************/
int saveImageToFile(const PGM* image, const char* filename);

/***********************************************************************
 * Create an empty image of specified dimension.
 * The image must later be deleted by calling deleteImage().
 *
 * PARAMETERS
 * width        The width of the image
 * height       The height of the image
 * numLevels    Number of gray levels of the image
 *
 * RETURN
 * NULL         if any error
 * image        A new image where each pixel is initialized to 0
 ***********************************************************************/
PGM* createEmptyImage(size_t width, size_t height,
                                  size_t numLevels);

/***********************************************************************
 * Delete an image.
 *
 * PARAMETERS
 * image        The image to destroy.
 ***********************************************************************/
void freeImage(PGM* image);

#endif // !_PGM_H_

