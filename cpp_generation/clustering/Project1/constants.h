#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>
#include "constants.h"
typedef struct _Config Config;
typedef struct _Config
{
    int NUM_POINTS;
    int NUM_DIM;
    int POINTS_DISTR;
    int NUM_CLUST;
    int CLUST_IMPR;
    int IMPR_CLASS;
    int IT_ORDER;
    long SEED_POINTS;
    long SEED_ASSIGN;
} Config;

#endif