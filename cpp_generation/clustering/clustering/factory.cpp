#include "factory.h"
void run_algorithm(Config* config) {
	using namespace std;
	Result* result = new Result(config);
	result->set_time_start();
	//Points
	double* points = NULL;
	if (config->POINTS_DISTR == 0) {
		points = uniform_points(config);
	}
	else if (config->POINTS_DISTR == 1) {
		points = blobs(config);
	}
	else if (config->POINTS_DISTR == 2) {
		points = moons(config);
	}
	else if (config->POINTS_DISTR == 3) {
		points = tsplib_sample_points(config);
	}
	else {
		cout << "Wrong POINTS_DISTR " << config->POINTS_DISTR << endl;
		exit(1);
	}
	if (config->NUM_POINTS <= config->NUM_CLUST) {
		delete result;
		delete[] points;
		return;
	}
	//Initial clustering
	int* assignements = random_clust(config);
	//Create  object representing a clustering
	Clustering* clustering = new Clustering(points, assignements,config);
	//Compute centroids
	centroids(clustering, config);
	//Improvement (0 = no improvement)
	if (config->CLUST_IMPR == 1) {
		improve_kmeans(clustering, config);
	}
	else if (config->CLUST_IMPR > 1) {
		cout << "Wrong CLUST_IMPR " << config->CLUST_IMPR << endl;
		exit(1);
	}
	// Iteration order
	IterationOrder* order = NULL;
	if (config->IMPR_CLASS == 1 && config->IT_ORDER == 0) {
		order = new BACK(config);
	}
	else if (config->IMPR_CLASS == 1 && config->IT_ORDER == 1) {
		order = new CURR(config);
	}
	else if (config->IMPR_CLASS == 1 && config->IT_ORDER == 2) {
		order = new RANDOM(config);
	}
	//Launch algorithm
	if (config->IMPR_CLASS == 0) {
		run_BI(clustering, result, config);
	}
	else if (config->IMPR_CLASS == 1) {
		run_FI(clustering, result, order, config);
	}
	result->set_time_end();
	//Check no empty cluster
	/*
	bool* seen = new bool[config->NUM_CLUST];
	for (int i = 0; i < config->NUM_POINTS; i++) {
		seen[result->get_finalAssign()[i]] = true;
	}
	for (int i = 0; i < config->NUM_POINTS; i++) {
		if (!seen[result->get_finalAssign()[i]]) {
			std::cout << "GLOB_SEED:" << config->SEED_GLOB << ", empty cluster: " << i << std::endl;
		}
	}
	delete[] seen;*/
	create_dataset(result, clustering);
	// Cleaning
	delete result;
	delete clustering;//delete points and initial clustering too
	if(config->IMPR_CLASS == 1)
		delete order;
}