/* ========================================================================= *
 * File parsing and Main Function

 * ------------------------------------------------------------------------- *
 * NOM
 *      quantizer
 * SYNOPSIS
 *      quantizer inputImg k outputName
 * DESCIRPTION
 *      Quantizes the input image on k levels and save it.
 * USAGE
 *      ./quantizer lena.pgm 4 lena_4.pgm
 *          Will compress the image lena.pgm on 4 levels and save it under
 *          the name "lena_4.pgm".
 * ------------------------------------------------------------------------- *
 * ========================================================================= */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <float.h>

#include "PGM.h"
#include "Mapping.h"
#include "compression.h"



/*-----------------------------------------------------------------------------+
|                              COMPRESSION                                     |
+-----------------------------------------------------------------------------*/
typedef struct
{
    PGM *compressed;
    double error;

} Compression;



/*************************************************************************
 * Apply the mapping to the images to create a compressed images.
 * The compressed images must be free with `freeImage`.
 *
 * PARAMETERS
 * mapping      A valid pointer to a Mapping
 * image        A valid pointer to a PGM image
 *
 * RETURN
 * comp         A Compression structure. In case of error, the `compressed`
 *              field will be set to NULL. Otherwise, contains the
 *              compressed image and the associated compression error
 *************************************************************************/
static Compression applyMapping(const Mapping *mapping, const PGM *image)
{
    PGM* compressedImg = createEmptyImage(image->width,
                                                      image->height,
                                                      image->maxValue);

    uint16_t *lookUpTable = mapping2Lookup(mapping, image->maxValue);

    if(!compressedImg || !lookUpTable)
    {
        freeImage(compressedImg);
        free(lookUpTable);
        return (Compression){NULL, DBL_MAX};
    }

    double err = 0, delta;
    uint16_t old, new;

    // Apply compression to image
    for(size_t i=0; i<image->height; i++)
    {
        for(size_t j=0; j<image->width; j++)
        {
            // Compute error
            old = image->array[i][j];
            new = lookUpTable[old];
            delta = (double)old - (double)new;
            err += (delta*delta);

            // Actually apply compression
            compressedImg->array[i][j] = new;
        }
    }


    free(lookUpTable);

    return (Compression){compressedImg, err};
}



/***********************************************************************
 * Compute the histogram of the given image
 *
 * PARAMETERS
 * img          A valid pointer to a PGM structure
 *
 * RETURN
 * histo       A pointer to a Histogram. It must be deleted by calling
 *             `freeHistogram`
 * NULL        In case of error
 ***********************************************************************/
static Histogram* image2histogram(const PGM* img)
{
    if(!img)
        return NULL;

    Histogram *hist = createEmptyHistogram(img->maxValue+1);
    if(!hist)
        return NULL;

    for(size_t i=0; i<img->height; i++)
        for(size_t j=0; j<img->width; j++)
            hist->count[img->array[i][j]]++;

    return hist;
}



/***********************************************************************
 * Free the memory allocated by the given inputs
 *
 * PAREMETERS
 * h      A pointer to a Histogram
 * m      A pointer to a Mapping
 * pgm    A pointer to a PGM
 ***********************************************************************/
static inline void freeAll(Histogram *h, Mapping *m, PGM* pgm)
{
    freeHistogram(h);
    freeMapping(m);
    freeImage(pgm);

}

/***********************************************************************
 * Compress the given image on `nLevels` levels.
 *
 * PAREMETERS
 * image      A valid pointer to a Histogram
 * nLevels    The number of levels
 *
 * RETURN
 * comp         A Compression structure. In case of error, the `compressed`
 *              field will be set to NULL. Otherwise, contains the
 *              compressed image and the associated compression error
 ***********************************************************************/
static Compression compressImage(const PGM *image, size_t nLevels)
{
    if(nLevels == 0 || !image)
        return (Compression){NULL, DBL_MAX};

    Histogram *hist = NULL;
    Mapping *mapping = NULL;
    PGM* compressedImg = NULL;


    hist = image2histogram(image);
    if(!hist)
    {
        freeAll(hist, mapping, compressedImg);
        return (Compression){NULL, DBL_MAX};
    }


    mapping = computeMapping(hist, nLevels);
    if(!mapping)
    {
        freeAll(hist, mapping, compressedImg);
        return (Compression){NULL, DBL_MAX};
    }

    Compression compression = applyMapping(mapping, image);
    compressedImg = compression.compressed;
    if(!compressedImg)
    {
        freeAll(hist, mapping, compressedImg);
        return (Compression){NULL, DBL_MAX};
    }


    // Free local resources

    freeAll(hist, mapping, NULL);

    return compression;
}


/*-----------------------------------------------------------------------------+
|                                  MAIN                                        |
+-----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    // Checking arguments
    if (argc != 4)
    {
        /*
         * argv[0]: name of the executable
         * argv[1]: name of the input file
         * argv[2]: number of levels
         * argv[3]: name of the output file
         */
        fprintf(stderr, "Usage: %s <PGM input image> <unsgined int> "
                        "<PGM output name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse arguments
    size_t nbLevels = 0;
    if(sscanf(argv[2], "%zu", &nbLevels) != 1)
    {
        fprintf(stderr, "Aborting; number of levels should be unsigned int. "
                        "Got '%s'.\n", argv[2]);
        return EXIT_FAILURE;
    }

    // Load input Image
    PGM* inputImg = createImageFromFile(argv[1]);
    if(!inputImg)
    {
        fprintf(stderr, "Aborting; error while loading input image '%s'\n",
                argv[1]);
        return EXIT_FAILURE;
    }


    // Compress
    Compression compression = compressImage(inputImg, nbLevels);
    PGM* outputImg = compression.compressed;
    if(!outputImg)
    {
        fprintf(stderr, "Aborting; error while computing the reduction\n");
        freeImage(inputImg);
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Compression error: %lf\n", compression.error);


    // Save output image
    if(saveImageToFile(outputImg, argv[3]) != 0)
    {
        fprintf(stderr, "Aborting; error while saving output image in '%s'\n",
                argv[3]);
        freeImage(inputImg);
        freeImage(outputImg);
        return EXIT_FAILURE;
    }

    freeImage(inputImg);
    freeImage(outputImg);
    return EXIT_SUCCESS;
}
