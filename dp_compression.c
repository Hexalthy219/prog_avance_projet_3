/***********************************************************************
 * Implementation of a naive algorithm that compress an image with
 * equal bins.
 ***********************************************************************/
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "compression.h"

// typedef struct
// {
//     size_t nLevels;     // The number of levels after compression (k)
//     size_t *thresholds; // Array containing the thresholds (p_1, ..., p_k=n)
//     uint16_t *levels;   // Values between thresholds (v_1, ..., v_k)

// } Mapping;

static void k_intervalle(const Histogram *histogram, size_t k, size_t numero_intervalle, Mapping *mapping_inter, Mapping *mapping_min, size_t fin_precedent, int **valeur_g, int err_min);

static unsigned int calcul_g(int debut, int fin, const Histogram *histogram);

static int calcul_erreur(Mapping *mapping_inter, Mapping *mapping_min, int err_min, const Histogram *histogram);

static void k_intervalle(const Histogram *histogram, size_t k, size_t numero_intervalle, Mapping *mapping_inter, Mapping *mapping_min, size_t fin_precedent, int **valeur_g, int err_min){
    //cas de base
    if(numero_intervalle==k){
        mapping_inter->thresholds[k] = histogram->length-1;
        //vérifie si calcul pour l'intervalle existe déjà
        if(valeur_g[fin_precedent][histogram->length-1]==-1)
            mapping_inter->levels[numero_intervalle - 1] = valeur_g[fin_precedent][histogram->length - 1] = calcul_g(fin_precedent, histogram->length - 1, histogram);
        calcul_erreur(mapping_inter, mapping_min, err_min, histogram);
        return;
    }
    

    for(size_t i = fin_precedent; i<histogram->length-k+numero_intervalle-1; i++){
        if(valeur_g[fin_precedent][i]==-1){
            mapping_inter->levels[numero_intervalle - 1] = valeur_g[fin_precedent][i] = calcul_g(fin_precedent, i, histogram);
        }

        mapping_inter->thresholds[numero_intervalle] = i;
        k_intervalle(histogram, k, numero_intervalle+1, mapping_inter, mapping_min, i, valeur_g, err_min);
    }
}

static int calcul_erreur(Mapping *mapping_inter, Mapping *mapping_min, int err_min, const Histogram *histogram){
    int erreur = 0;
    for (size_t i = 0; i < mapping_inter->nLevels; i++){
        for(size_t j = mapping_inter->thresholds[i]; j<mapping_inter->thresholds[i+1]; j++){
            erreur += histogram->count[j] * (j - mapping_inter->levels[i]) * (j - mapping_inter->levels[i]);
        }
    }
    if(err_min<=erreur)
        return err_min;
    for(size_t i = 0; i<mapping_min->nLevels; i++){
        mapping_min->levels[i] = mapping_inter->levels[i];
        mapping_min->thresholds[i] = mapping_inter->thresholds[i];
    }
    mapping_min->thresholds[mapping_min->nLevels] = mapping_inter->thresholds[mapping_inter->nLevels];
    return erreur;
}

static unsigned int calcul_g(int debut, int fin, const Histogram *histogram){
    unsigned int g = 0;
    size_t test_erreur;
    size_t err_min = INT_MAX;

    for(int i=debut; i<fin; i++){
        test_erreur = 0;
        for(int j = debut; j<fin; j++)
            test_erreur += histogram->count[j] * (j-i)*(j-i);

        if(test_erreur<err_min){
            err_min = test_erreur;
            g = i;
        }
    }

    printf("g : %u\n", g);

    return g;
}

Mapping *computeMapping(const Histogram *histogram, size_t nLevels){
    int err_min = INT_MAX;
    printf("bite\n");

    //Allocation dynamique
    Mapping *mapping = createUninitializedMapping(nLevels);
    if (!mapping)
        return NULL;

    //Initialisation
    mapping->nLevels = nLevels;
    for(size_t i = 0; i<nLevels; i++){
        mapping->thresholds[i] = 0;
        mapping->levels[i] = 0;
    }
    mapping->thresholds[nLevels] = 0;


    Mapping *mapping_intermediaire = createUninitializedMapping(nLevels);

    //allocation matrice valeur g pour différentes valeurs intervalles
    int **valeur_g = malloc(sizeof(int*)*histogram->length);
    if(!valeur_g)
        return NULL;
    for(size_t i = 0; i<histogram->length; i++){
        valeur_g[i] = calloc(histogram->length, sizeof(int));
        if(!valeur_g[i]){
            for(int j=i-1; j>=0; j--)
                free(valeur_g[j]);
            free(valeur_g);
            return NULL;
        }
        for(size_t j = 0; j<histogram->length; j++)
            valeur_g[i][j] = -1;
    }


    //appel recursif création intervalles
    mapping_intermediaire->nLevels = nLevels;

    for (size_t i = 1; i <= histogram->length - nLevels; i++){
        mapping_intermediaire->thresholds[0] = 0;
        mapping_intermediaire->thresholds[1] = i;

        k_intervalle(histogram, nLevels, 2, mapping_intermediaire, mapping, i, valeur_g, err_min);
    }

    printf("couille\n");
    return mapping;
}