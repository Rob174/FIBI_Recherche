#include "blobs.h"

double* blobs(Config* config) {
    const float std_min = 0.01, std_max = 0.1;
    const int GRID_COORD_MIN = 0, GRID_COORD_MAX = 100;
    const float scale_factor = GRID_COORD_MAX - GRID_COORD_MIN;
    const float dist_thresh = float(GRID_COORD_MAX - GRID_COORD_MIN) / 10.;
    std::mt19937 gen_points(config->SEED_POINTS);
    std::uniform_real_distribution<double> dis_std(std_min, std_max);
    const int NUM_CENTERS = 2;
    double centers[NUM_CENTERS][2];
    double stds[NUM_CENTERS];
    std::uniform_real_distribution<double> dis_centers(GRID_COORD_MIN, GRID_COORD_MAX);
    for (int i_cent = 0; i_cent < config->NUM_DIM; i_cent++) {
        centers[0][i_cent] = dis_centers(gen_points);
    }
    for (int i = 0; i < NUM_CENTERS; i++) {
        stds[i] = dis_std(gen_points) * scale_factor;
    }
    double distance = 0;
    int counter = 1000;
    while (distance < dist_thresh && counter > 0) {
        for (int i_cent = 0; i_cent < config->NUM_DIM; i_cent++) {
            centers[1][i_cent] = dis_centers(gen_points);
        }
        distance = sqrt(dist(centers[0], centers[1], config)) - 2 * (stds[0]+stds[1]);
        counter--;
    }
    double* points = new double[config->NUM_POINTS * config->NUM_DIM];
    for (int i_clust = 0; i_clust < config->NUM_CLUST; i_clust++) {
        double* center = centers[i_clust];
        std::normal_distribution<double> dis_norm(0, 1);
        // We will shift the points afterwards 
        /*
        if X ~ N(0,1) ; with Y=aX+b -> Y ~ N(a x 0 + b, a^2 x 1^2)
        We want N(mu,sigma^2)
        -> b=mu
        -> a=sigma
        */
        const int offset_pt = i_clust * config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST;
        for (int i_pt = 0; i_pt < config->NUM_DIM * config->NUM_POINTS / config->NUM_CLUST; i_pt++) {// equal number of points for each cluster
            points[offset_pt + i_pt] = dis_norm(gen_points) * stds[i_clust] + center[i_pt % 2];
        }
    }
    return points;
}