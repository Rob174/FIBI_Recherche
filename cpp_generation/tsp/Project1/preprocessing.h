#pragma once
#include "constants.h"
#include "utils.h"
#include <stdio.h>
#include <iostream>
#include <tuple>
#include <set>
class DistanceMatrix
{
protected:
    double** d;
    Config* conf;
    bool initialized;
public:
    DistanceMatrix(Config*config);
    ~DistanceMatrix();
    void compute(double *points);
    double get(int pt1_idx, int pt2_idx);
};

class GeneratedDistanceMatrix : public DistanceMatrix {
public:
    GeneratedDistanceMatrix(Config* config) : DistanceMatrix(config) {};
    void compute();
};

class OrderedDistanceMatrix {
private:
    double** d_prime;
    int** r;
    int** p;

    Config* conf;
public:

    OrderedDistanceMatrix(DistanceMatrix* distances, Config* conf);
    ~OrderedDistanceMatrix();
    int rank_of_edge(int from_town, int to_town);
    int town_of_rank(int from_town, int rank);
};