#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>
#include <iostream>
typedef struct _Config Config;
typedef struct _Config
{
    int NUM_POINTS; //
    int NUM_CLUST;
    int NUM_DIM;    //
    int IMPR_CLASS; //
    int IT_ORDER;
    long SEED_POINTS; //
    long SEED_ASSIGN; //
    int DATASET;      //
    int TOUR_ALGO;    //
    long SEED_GLOB;
    std::string out_dataset;
    void print()
    {
        std::cout
            << "SEED_GLOB:" << SEED_GLOB << ","
            << "NUM_POINTS:" << NUM_POINTS << ","
            << "NUM_CLUST:" << NUM_CLUST << ","
            << "IMPR_CLASS:" << IMPR_CLASS << ","
            << "IT_ORDER:" << IT_ORDER << ","
            << "SEED_POINTS:" << SEED_POINTS << ","
            << "SEED_ASSIGN:" << SEED_ASSIGN << ","
            << "DATASET:" << DATASET << ","
            << "TOUR_ALGO:" << TOUR_ALGO << std::endl;
    }
} Config;

#endif