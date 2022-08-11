#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>
#include <iostream>
typedef struct _Config Config;
typedef struct _Config
{
    int NUM_POINTS;//
    int NUM_DIM;//
    int POINTS_DISTR;//
    int NUM_CLUST;//
    int CLUST_IMPR;//
    int IMPR_CLASS;//
    int IT_ORDER;//
    long SEED_POINTS;//
    long SEED_ASSIGN;//
    long SEED_GLOB;
    void print()
    {
        std::cout
            << "SEED_GLOB:" << SEED_GLOB << ","
            << "NUM_POINTS:" << NUM_POINTS << ","
            << "POINTS_DISTR:" << POINTS_DISTR << ","
            << "NUM_CLUST:" << NUM_CLUST << ","
            << "CLUST_IMPR:" << CLUST_IMPR << ","
            << "IMPR_CLASS:" << IMPR_CLASS << ","
            << "IT_ORDER:" << IT_ORDER << ","
            << "SEED_POINTS:" << SEED_POINTS << ","
            << "SEED_ASSIGN:" << SEED_ASSIGN << std::endl;
    }
} Config;

#endif