/***********************************************************************
 * Implementation of a naive algorithm that compress an image with
 * equal bins.
 ***********************************************************************/
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "compression.h"


static unsigned int calcul_g(int debut, int fin, const Histogram *histogram);

static unsigned int calcul_g(int debut, int fin, const Histogram *histogram){
    unsigned int g = 0;
    size_t test_erreur;
    size_t err_min = INT_MAX;

    for (int i = debut; i < fin; i++){
        test_erreur = 0;
        for (int j = debut; j < fin; j++)
            test_erreur += histogram->count[j] * (j - i) * (j - i);

        if (test_erreur < err_min){
            err_min = test_erreur;
            g = i;
        }
    }


    return g;
}

Mapping *computeMapping(const Histogram *histogram, size_t nLevels){

    //Allocation dynamique
    Mapping *mapping = createUninitializedMapping(nLevels);
    if (!mapping)
        return NULL;

    //Initialisation
    mapping->nLevels = nLevels;
    for (size_t i = 0; i < nLevels; i++){
        mapping->thresholds[i] = 0;
        mapping->levels[i] = 0;
    }

    int nombre_pixel = 0;

    for(int i = 0; i<histogram->length; i++)
        nombre_pixel += histogram->count[i];
    
    double nombre_pixel_par_intervalle = nombre_pixel/nLevels;
    nombre_pixel = 0;
    int j = 0;
    for(int i = 0; i<nLevels; i++){
        while(nombre_pixel<(i+1)*nombre_pixel_par_intervalle){
            nombre_pixel+=histogram->count[j];
            j++;
        }
        mapping->thresholds[i] = j;
        printf("inter : %d\n", j);
    }

    mapping->levels[0] = calcul_g(0,mapping->thresholds[0], histogram);
    for(int i = 1; i<nLevels; i++)
        mapping->levels[i] = calcul_g(mapping->thresholds[i-1], mapping->thresholds[i], histogram);



    return mapping;
}