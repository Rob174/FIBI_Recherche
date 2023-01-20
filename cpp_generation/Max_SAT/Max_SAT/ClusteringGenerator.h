#ifndef CLUSTERINGGENERATOR_H
#define CLUSTERINGGENERATOR_H
#include <random>
#include <math.h>
#include <H5Cpp.h>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <random>
#include "constants.h"


using namespace H5;

double *uniform_points(const int num_points, const int num_dims, const int seed, const int minV = 0, const int maxV = 100);
int* random_clust(ClusteringConfig* config);
double* open_points_1(const int i, std::string filename, ClusteringConfig* config, bool load_num_clust);
double* test_points();
int* test_clusters();
#endif