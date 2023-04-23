#include "tests_constants.hpp"
#include "ClusteringGenerator.h"
#include <random>
using namespace std;
void check_uniform_points() {
	// towns_pos = uniform_points(conf->NUM_TOWNS.get(),
	// 							   conf->NUM_DIM.get(),
	// 							   conf->SEED_PROBLEM.get());
	std::uniform_int_distribution<int> dist_num_towns(1, 100);
	std::uniform_int_distribution<int> dist_num_dims(1, 100);
	std::uniform_int_distribution<int> dist_seed(0, 1000000);
	std::uniform_real_distribution<double> dist_minV(0, 100);
	std::uniform_real_distribution<double> dist_maxV(0, 100);
	int num_tests = 100;
	std::mt19937 gen;

	for (int i = 0; i < num_tests; i++) {
		const unsigned int num_towns = dist_num_towns(gen);
		const unsigned int num_dims = dist_num_dims(gen);
		const unsigned int seed = dist_seed(gen);
		const double minV = dist_minV(gen);
		const double maxV = dist_maxV(gen);
		vector<double> towns_pos = uniform_points(num_towns, num_dims, seed, minV, maxV);
		// Check that the number of points is correct
		if (towns_pos.size() != num_towns * num_dims) {
			throw std::exception("Error in uniform_points: the number of towns is not correct");
		}
		// Check that the values are in the correct range
		for (int j = 0; j < towns_pos.size(); j++) {
			if (towns_pos[j] < minV || towns_pos[j] > maxV) {
				throw std::exception("Error in uniform_points: the values are not in the correct range");
			}
		}
	}
}

void check_tsplib() {
	// towns_pos = open_file("tsplib.hdf5", conf->SEED_PROBLEM.get(), conf);
	std::uniform_int_distribution<int> dist_seed(0, 1000000);
	std::uniform_int_distribution<int> dist_num_towns(1, 100);
	std::uniform_int_distribution<int> dist_num_dims(1, 100);
	int num_tests = 100;
	std::mt19937 gen;

	for (int i = 0; i < num_tests; i++) {
		const unsigned int seed = dist_seed(gen);
		const unsigned int num_towns = dist_num_towns(gen);
		const unsigned int num_dims = dist_num_dims(gen);
		vector<double> towns_pos = open_file("tsplib.hdf5", seed, num_towns, num_dims);
		// Check that the number of points is correct
		if (towns_pos.size() != num_towns * num_dims) {
			throw std::exception("Error in open_file: the number of towns is not correct");
		}
	}
}