#include "centroids.h"
void centroids(Clustering* clust, Config* config) {
    for (int i = 0; i < config->NUM_POINTS; i++)
    {
        int cluster_id = clust->c_a[i];
        for (int j = 0; j < config->NUM_DIM; j++)
        {
            clust->c_c[cluster_id * config->NUM_DIM + j] += clust->p_c[i * config->NUM_DIM + j];
        }
        clust->n_p_p_c[cluster_id]++;
    }
    for (int i = 0; i < config->NUM_CLUST; i++)
    {
        for (int j = 0; j < config->NUM_DIM; j++)
        {
            clust->c_c[i * config->NUM_DIM + j] = clust->n_p_p_c[i] > 0 ? clust->c_c[i * config->NUM_DIM + j] / clust->n_p_p_c[i] : 0.;
        }
    }
}