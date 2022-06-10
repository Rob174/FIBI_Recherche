#include "factory.h"
void run_algorithm(Config* config) {
	using namespace std;
	Result* result = new Result(config);
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
	else {
		cout << "Wrong POINTS_DISTR " << config->POINTS_DISTR << endl;
		exit(1);
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
	// Algorithm choice
	ImprovementChoice* impr = NULL;
	if (config->IMPR_CLASS == 0) {
		impr = new BestImpr();
	}
	else if (config->IMPR_CLASS == 1) {
		impr = new FirstImpr();
	}
	else {
		cout << "Wrong IMPR_CLASS " << config->IMPR_CLASS << endl;
		exit(1);
	}
	// Iteration order
	IterationOrder* order = NULL;
	if (config->IMPR_CLASS == 0) {
		order = new BACK(config);
	}
	else if (config->IT_ORDER == 0) {
		order = new BACK(config);
	}
	else if (config->IT_ORDER == 1) {
		order = new CURR(config);
	}
	else if (config->IT_ORDER == 2) {
		order = new RANDOM(config);
	}
	Clustering* initial_clustering = clustering->deepcopy(config);
	run(clustering, result, impr, order, config);
	result->set_time_end();
	create_dataset(result, initial_clustering, clustering);
	// Cleaning
	delete result;
	delete clustering;//delete points and initial clustering too
	delete impr;
	delete order;
	delete initial_clustering;
}