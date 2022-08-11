#ifndef KMEANS_H
#define KMEANS_H
#include "constants.h"
#include "clustering.h"
#include "utils.h"
#include <math.h>
#include <iostream>
#include <tuple>
#include <set>
#include <list>


void improve_kmeans(Clustering* clustering, Config* config);
void update_centroid(Clustering* clustering, Config* config);
#endif
