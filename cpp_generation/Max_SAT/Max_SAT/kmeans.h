#ifndef KMEANS_H
#define KMEANS_H
#define EPSILON 1e-5
#include <random>
#include <math.h>
#include <H5Cpp.h>
#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <random>
#include "constants.h"
#include "Container.h"
#include "utils.h"

void kmeansPlusPlus(ClusteringContainer *clustering, ClusteringConfig *config);
void compKmeansCentroids(ClusteringContainer *clustering, ClusteringConfig *config);
bool affectToNearestCluster(ClusteringContainer *clustering, ClusteringConfig *config);
#endif