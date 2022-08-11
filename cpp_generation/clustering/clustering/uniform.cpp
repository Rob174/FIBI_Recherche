#include "uniform.h"
double* uniform_points(Config* config) {
    const int GRID_COORD_MIN = 0, GRID_COORD_MAX = 100;
    std::mt19937 gen_points(config->SEED_POINTS);
    std::uniform_real_distribution<> dis(GRID_COORD_MIN, GRID_COORD_MAX);
    double* points = new double[config->NUM_POINTS*config->NUM_DIM];
    for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++) {
        points[i] = dis(gen_points);
    }
    return points;
}