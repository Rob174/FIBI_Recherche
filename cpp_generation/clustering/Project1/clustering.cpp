#include "clustering.h"

Clustering::Clustering(double* points, int* assignements, Config* config) : p_c(points), c_a(assignements) {
    this->n_p_p_c = new int[config->NUM_POINTS];
    for (int i = 0; i < config->NUM_POINTS; i++) {
        this->n_p_p_c[i] = 0;
    }
    this->c_c = new double[config->NUM_POINTS * config->NUM_DIM];
    for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++) {
        this->c_c[i] = 0.;
    }
}

Clustering::~Clustering() {
    delete[] this->p_c;
    delete[] this->c_a;
    delete[] this->n_p_p_c;
    delete[] this->c_c;
}

Clustering* Clustering::deepcopy(Config*config) {
    Clustering* new_clustering = new Clustering();
    // Initialize points coordinates
    new_clustering->p_c = new double[config->NUM_POINTS * config->NUM_DIM];
    for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++)
        new_clustering->p_c[i] = this->p_c[i];
    // Initialize number of points per cluster
    new_clustering->n_p_p_c = new int[config->NUM_CLUST];
    for (int i = 0; i < config->NUM_CLUST; i++)
        new_clustering->n_p_p_c[i] = this->n_p_p_c[i];
    // Initialize cluster intial assignements
    new_clustering->c_a = new int[config->NUM_POINTS];
    for (int i = 0; i < config->NUM_POINTS; i++)
        new_clustering->c_a[i] = this->c_a[i];
    // Initialize cluster centroids
    new_clustering->c_c = new double[config->NUM_CLUST * config->NUM_DIM];
    for (int i = 0; i < config->NUM_CLUST * config->NUM_DIM; i++)
        new_clustering->c_c[i] = this->c_c[i];
    return new_clustering;
}