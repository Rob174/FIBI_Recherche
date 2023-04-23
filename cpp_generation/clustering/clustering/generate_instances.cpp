#include "generate_instances.h"


double* generate_gaussian(const double* center, const int num_points, const double* stds, Config* conf) {
	std::mt19937 gen_points(conf->SEED_POINTS);
	double* points = new double[num_points * conf->NUM_DIM];
	for (int j = 0; j < conf->NUM_DIM; j++) {
		std::normal_distribution<> d{ center[j],stds[j]};
		for (int i = 0; i < num_points; i++) {
			points[i * conf->NUM_DIM+j] = d(gen_points);
		}
	}
	return points;
}

double* random_centers(const int num_centers, Config* conf) {
	const double MIN_x = 0;
	const double MAX_x = 100;

	std::mt19937 gen_points(conf->SEED_POINTS);
	std::uniform_real_distribution<double> d{ MIN_x,MAX_x };

	double* points = new double[num_centers * conf->NUM_DIM];
	for (int i = 0; i < num_centers; i++)
	{
		for (int j = 0; j < conf->NUM_DIM; j++)
		{
			points[i * conf->NUM_DIM + j] = d(gen_points);
		}
	}
	return points;
}
double* linspace_p(const double start, const double end, const int num_points) {
	double* points = new double[num_points];
	double p = start;
	const double step = (end - start) / (double)(num_points - 1);
	for (int i = 0; i < num_points; i++)
	{
		points[i] = p+i*step;
	}
	return points;
}
double* grid_centers(const int num_centers, const int num_pts_per_clust, Config* conf) {
	const double n = sqrt(num_centers);
	if (n != (int)n) {
		std::cout << "num_center must be a squared integer" << std::endl;
	}
	const double MIN_x = 0;
	const double MAX_x = 100;

	std::mt19937 gen_max(conf->SEED_POINTS);
	std::uniform_real_distribution<double> d{ MAX_x/2.,MAX_x };

	const double MAX = d(gen_max);
	double* centers = new double[conf->NUM_POINTS * conf->NUM_DIM];
	for (int i = 0; i < conf->NUM_DIM; i++)
	{
		const double* centers_axis = linspace_p(MIN_x, MAX, num_pts_per_clust);
		for (int j = 0; j < num_pts_per_clust; j++)
		{
			centers[j * conf->NUM_DIM + i] = centers_axis[j];
		}
		delete[] centers_axis;
	}
	
	return centers;
}

