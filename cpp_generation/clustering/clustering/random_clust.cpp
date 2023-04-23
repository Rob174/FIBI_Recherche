#include "random_clust.h"

int* random_clust(Config* config) {
    int * clusters = new int[config->NUM_POINTS];
    std::mt19937 gen_assign(config->SEED_ASSIGN);
    std::uniform_int_distribution<> distrib(0, config->NUM_CLUST - 1);
    for (int i = 0; i < config->NUM_POINTS; i++) {
        clusters[i] = distrib(gen_assign);
    }
    return clusters;
}