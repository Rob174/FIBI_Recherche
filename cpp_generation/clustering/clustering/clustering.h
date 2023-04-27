#pragma once
#include "constants.h"
#include <list>
#include <utility>

class Clustering {
public:
    double* p_c; // points coor
    int* c_a;     // cluster assignements
    int* n_p_p_c; // num_pts_per_clust
    double* c_c; // cluster_centroids

    Clustering() {};
    Clustering(double* points, int* assignements, Config* config);
    ~Clustering();
    Clustering* deepcopy(Config* config);
    void print_with_coords(Config* conf, int pt_modif = -1, int from = -1, int to = -1);
    void print_with_ids(Config* conf, int pt_modif = -1, int from = -1, int to = -1);
};