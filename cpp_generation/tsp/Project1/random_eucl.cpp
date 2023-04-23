#include "random_eucl.h"

double* random_eucl_points_uniform(Config* config)
{
	const double MIN_GRID = 0, MAX_GRID = 100;
	double* points = new double[config->NUM_POINTS * config->NUM_DIM];

	std::uniform_real_distribution<double> distrib(MIN_GRID, MAX_GRID);
	std::mt19937 gen_assign(config->SEED_POINTS);
	for (int i = 0; i < config->NUM_POINTS * config->NUM_DIM; i++) {
		points[i] = distrib(gen_assign);
	}
	return points;
}

double * random_eucl_points_blobs(Config* config) {
    const float std_min = 0.01;
    const float std_max = 0.1;
    const double MIN_GRID = 0, MAX_GRID = 100;
    double* points = new double[config->NUM_POINTS * config->NUM_DIM];

    const float scale_factor = MAX_GRID - MIN_GRID;
    const float dist_thresh = float(MAX_GRID - MIN_GRID) / 10.;
    std::mt19937 gen_points(config->SEED_POINTS);
    std::uniform_real_distribution<double> dis_std(std_min, std_max);
    double centers[2][2];
    double stds[2];
    std::uniform_real_distribution<double> dis_centers(MIN_GRID, MAX_GRID);
    for (int i_cent = 0; i_cent < config->NUM_DIM; i_cent++) {
        centers[0][i_cent] = dis_centers(gen_points);
    }
    for (int i = 0; i < 2; i++) {
        stds[i] = dis_std(gen_points) * scale_factor;
    }
    double distance = 0;
    int counter = 1000;
    while (distance < dist_thresh && counter > 0) {
        for (int i_cent = 0; i_cent < config->NUM_DIM; i_cent++) {
            centers[1][i_cent] = dis_centers(gen_points);
        }
        distance = dist(centers[0], centers[1], config) - 2 * stds[0] - 2 * stds[1];
        counter--;
    }
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