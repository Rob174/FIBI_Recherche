#include "factory.h"

void run_algorithm(Config* conf)
{
	Result* res = new Result(conf);
	//Points
	double* points = NULL;
	if (conf->DATASET == 0)
	{
		points = random_eucl_points_uniform(conf);
	}
	else if (conf->DATASET == 1) {
		points = random_eucl_points_blobs(conf);
	}
	else if (conf->DATASET == 2) {
		using namespace H5;
		const H5std_string FILE_NAME("tsplib.hdf5");
		points = tsplib_sample_points(FILE_NAME,conf);
	}
	else if (conf->DATASET == 3) {
		using namespace H5;
		const H5std_string FILE_NAME("tsplib_1000_stop.hdf5");
		points = tsplib_sample_points(FILE_NAME,conf);
	}
	else if (conf->DATASET == 4) {
		using namespace H5;
		const H5std_string FILE_NAME("tsplib_extended.hdf5");
		points = tsplib_sample_points(FILE_NAME, conf);
	}
	else if (conf->DATASET == 5) {
		using namespace H5;
		const H5std_string FILE_NAME("tsplib_paper_moreThan1000.hdf5");
		points = tsplib_sample_points(FILE_NAME, conf);
	}
	else if(conf->DATASET > 6) {
		std::cout << "Wrong dataset value " << conf->DATASET << std::endl;
		exit(1);
	}
	//Preprocessing: distance matrix
	DistanceMatrix* distances;
	if (conf->DATASET == 6) {
		GeneratedDistanceMatrix* distancesGen = new GeneratedDistanceMatrix(conf);
		distancesGen->compute();
		distances = distancesGen;
	}
	else if(points != NULL) {
		distances = new DistanceMatrix(conf);
		distances->compute(points);
	}
	else {
		std::cout << "Wrong dataset value " << conf->DATASET << std::endl;
		exit(1);
	}
	//Init tour
	int* tour_array;
	if (conf->TOUR_ALGO == 0) {
		tour_array = random_tour(conf);
	}
	else if (conf->TOUR_ALGO == 1) {
		tour_array = improved_tour(conf, distances);
	}
	else if (conf->TOUR_ALGO == 2) {
		tour_array = improved_rand_tour(conf, distances);
	}
	else {
		std::cout << "Wrong TOUR_ALGO value " << conf->TOUR_ALGO << std::endl;
		exit(1);
	}
	//res->set_init_tour(points, tour_array);
	//Create tour
	Tour* tour = new Tour(tour_array, conf);
	//Run algorithm
	int num_pts = conf->NUM_POINTS;
	res->set_time_start();
	switch (conf->IMPR_CLASS)
	{
	case 0:
		run_BI(tour, distances, res, conf);
		break;
	case 1:
		run_FI(tour, distances, res, conf);
		break;
	case 2:
		run_BI_NL(tour, distances, res, conf);
		break;
	case 3:
		run_FI_NL(tour, distances, res, conf);
		break;
	default:
		std::cout << "Wrong IMPR_CLASS value " << conf->IMPR_CLASS << std::endl;
		exit(1);
		break;
	}
	res->set_final_length(tour->get_length(distances));
	res->set_time_end();
	//Save results
	save_to_hdf5(res, conf);
	delete tour;
	delete distances;
	if(conf->DATASET != 3)
		delete[] points;
	delete res;
}