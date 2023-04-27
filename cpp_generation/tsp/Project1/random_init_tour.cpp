#include "random_init_tour.h"
int* random_tour(Config* config)
{
    std::mt19937 gen_dist(config->SEED_ASSIGN);
    int* tour = new int[config->NUM_POINTS];
    for (int i = 0; i < config->NUM_POINTS; i++)
        tour[i] = i;
    for (int i = 0; i < config->NUM_POINTS-2; i++)
    {
        std::uniform_int_distribution<> dis_choice(i, config->NUM_POINTS-1);
        int r = dis_choice(gen_dist);
        int tmp = tour[i];
        tour[i] = tour[r];
        tour[r] = tmp;
    }
    return tour;
}