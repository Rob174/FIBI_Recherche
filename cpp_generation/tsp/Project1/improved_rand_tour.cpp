#include "improved_rand_tour.h"	

int* improved_rand_tour(Config* config, DistanceMatrix* distances) {
    const int N = 4;
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
    for (int i = 0; i < config->NUM_POINTS - 1; i++) {
        // Prepare for the top N
        std::vector<std::pair<double, int>> top_x_data;
        for (int j : available_points) {
            double d = distances->get(tour[i], j);
            top_x_data.push_back(std::make_pair(d, j));
        }
        // Take top N
        const int size_top = std::min(N,(int)(top_x_data.size()));
        std::nth_element(top_x_data.begin(), top_x_data.begin() + size_top, top_x_data.end(),
            [](auto& l, auto& r) {return l.first < r.first; });
        std::uniform_int_distribution<> dis_top(0, size_top-1);
        const int selected_of_top_id = dis_top(gen_dist);
        tour[i + 1] = top_x_data.at(selected_of_top_id).second;
        // Erase the point to avoid duplicates
        available_points.erase(top_x_data.at(selected_of_top_id).second);
    }
    return tour;
}