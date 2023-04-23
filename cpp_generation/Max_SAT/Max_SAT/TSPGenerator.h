#ifndef TSPGENERATOR_H
#define TSPGENERATOR_H

#include "H5Cpp.h"
#include <string>
#include <random>
#include <set>
#include "Container.h"

using namespace H5;
double *open_file(std::string filename, const int instance_idx, TSPConfig* conf);
int *random_tour(const int num_towns, const int seed);
int *improved_tour(const int num_towns, const int seed, DistanceMatrix *distances);
int *improved_rand_tour(const int num_towns, const int seed, DistanceMatrix *distances, const int topk);
double* test_towns();
int* test_tour();
#endif