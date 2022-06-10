#include "tests.h"
#include "utils.h"
#include <set>
#include <chrono>
#include <thread>
#include <iostream>
#include <array>
#include<sstream>
#include <string>
//0 : pass ; 1 : fail
void print(std::string name) {
	std::cout << "Test " << name<<": ";
}
void succ_result() {
	std::cout << "\x1B[32mPASS\033[0m" << std::endl;
}
int test_write_to_hdf5() {
	/*
	Config conf = { 1000,2,4,0,0,0,0 };
	Result res(&conf, 100, 1000, 256.2, 201.1, 214569874.3);
	Clustering *init_clust = new Clustering();
	init_clust->c_a = new int[conf.NUM_POINTS];
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		init_clust->c_a[i] = i;
	}
	const int num_poss = conf.NUM_POINTS * conf.NUM_DIM;
	init_clust->p_c = new double[num_poss];
	for (int i = 0; i < conf.NUM_POINTS*conf.NUM_DIM; i++) {
		init_clust->c_a[i] = num_poss - i;
	}

	Clustering* final_clust = new Clustering();
	final_clust->c_a = new int[conf.NUM_POINTS];
	for (int i = 0; i < conf.NUM_POINTS; i++) {
		final_clust->c_a[i] = i;
	}
	final_clust->p_c = new double[num_poss];
	for (int i = 0; i < conf.NUM_POINTS * conf.NUM_DIM; i++) {
		final_clust->c_a[i] = num_poss -i;
	}
	std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
	create_dataset(&res, init_clust, final_clust);
	return 1;*/
}
void run_tests() {
	std::vector<int (*)()> test_functions = { 
		//test_seed_diff,
		//test_algos,
		//test_DI,
		//test_DI_opti,
		//test_write_to_hdf5
	};
	for (auto& f : test_functions) {
		if (f() != 0)
			break;
	}
}