#pragma once
#include "constants.h"

class Clustering {
public:
    double* p_c; // points coor
    int* c_a;     // cluster assignements
    int* n_p_p_c; // num_pts_per_clust
    double* c_c; // cluster_centroids

    Clustering();
    Clustering(double* points, int* assignements, Config* config);
    ~Clustering();
    Clustering* deepcopy(Config* config);
};