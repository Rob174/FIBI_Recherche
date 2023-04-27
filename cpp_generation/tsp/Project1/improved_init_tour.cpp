#include "improved_init_tour.h"
int* improved_tour(Config* config, DistanceMatrix* distances) {
    int* tour = new int[config->NUM_POINTS];
    // Add a set to avoid duplicates (a points can be the nearest one for two different points)
    std::set<int> available_points;
    for (int i = 0; i < config->NUM_POINTS; i++) {
        available_points.insert(i);
    }
    std::mt19937 gen_dist(config->SEED_ASSIGN);
    std::uniform_int_distribution<> dis_choice(0, config->NUM_POINTS - 1);
    int start_point = dis_choice(gen_dist);
    tour[0] = start_point;
    available_points.erase(start_point);
    for (int i = 0; i < config->NUM_POINTS-1; i++) {
        // Find the argmin of distance
        double min = std::numeric_limits<double>::max();
        int argmin = -1;
        for (int j : available_points) {
            double d = distances->get(tour[i], j);
            if (d < min) {
                min = d;
                argmin = j;
            }
        }
        tour[i+1] = argmin;
        // Erase the point to avoid duplicates
        available_points.erase(argmin);
    }
    return tour;
}