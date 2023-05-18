#pragma once
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>

#include "./data/get_data.hpp"
#include "./data/hdf5_files/tsp.hpp"
#include "./data/hdf5_files/clustering.hpp"
#include "./data/hdf5_files/maxsat.hpp"
#include "./data/improvements/tsp.hpp"
#include "./data/improvements/clustering.hpp"
#include "./data/improvements/maxsat.hpp"
#include "./data/solution_container/distance_matrix.hpp"
#include "./data/solution_container/tsp.hpp"
#include "./data/solution_container/clustering.hpp"
#include "./data/solution_container/maxsat.hpp"
#include "./algorithms/localSearch/tsp.hpp"
#include "./algorithms/localSearch/clustering.hpp"
#include "./algorithms/localSearch/maxsat.hpp"
#include "./factories/tsp.hpp"
#include "./factories/clustering.hpp"
#include "./factories/maxsat.hpp"
#include "./launch/threadpool.hpp"
#include "./launch/generators.hpp"
#include "../src/parser/Parser.hpp"

using namespace std;
#define TEST_MACRO(name) \
    void name()          \
    {
#define REGISTER(fn)                 \
    functions_to_run.push_back(&fn); \
    functions_names.push_back(#fn);
void run_tests(const int argc, char** argv)
{
	vector<void (*)(const int argc, char** argv)> functions_to_run;
	vector<string> functions_names;
	/*
	REGISTER(test_uniform_points);
	REGISTER(test_uniform_points_repetability);
	*/
	//REGISTER(test_normal_points);
	//REGISTER(test_normal_points_repetability);
	/*
	REGISTER(test_random_clust);
	REGISTER(test_random_clust_repeatability);
	REGISTER(test_tsplib);
	REGISTER(test_clustering_aloise);
	REGISTER(test_clustering_franti);
	REGISTER(test_maxsat_benchmark);
	REGISTER(test_random_tour);
	REGISTER(test_improved_tour_1);
	REGISTER(test_improved_tour_2);
	REGISTER(test_improved_rand_tour_1);
	REGISTER(test_improved_rand_tour_2);
	REGISTER(test_kmeansPlusPlus);
	REGISTER(test_kmeansPlusPlusSuperGlutton);
	REGISTER(test_kmeansPlusPlusSuperGlutton);
	REGISTER(test_improve_maxsat);

	REGISTER(test_DistanceMatrix);
	REGISTER(test_TSPSolutionContainer_compute_quality_metric_1);
	REGISTER(test_TSPSolutionContainer_compute_quality_metric_2);
	REGISTER(test_TSPSolutionContainer_test_flip);
	REGISTER(test_TSPSolutionContainer_flip);
	REGISTER(test_ClusteringSolutionContainer_compute_quality_metric_1);
	REGISTER(test_ClusteringSolutionContainer_test_flip);
	REGISTER(test_ClusteringSolutionContainer_compute_quality_metric_2);
	REGISTER(test_MAXSATSolutionContainer_compute_quality_metric);
	REGISTER(test_MAXSATSolutionContainer_test_flip);

	REGISTER(test_LocalSearch_TSP_flips_explored);
	REGISTER(test_LocalSearch_TSP_flips_explored_detail_BI);
	REGISTER(test_LocalSearch_TSP_flips_explored_detail_FI);
	REGISTER(test_LocalSearch_Clust_flips_explored);
	REGISTER(test_LocalSearch_Clust_flips_explored_detail_BI);
	REGISTER(test_LocalSearch_Clust_flips_explored_detail_FI);

	REGISTER(test_LocalSearch_MAXSAT_flips_explored);
	REGISTER(test_LocalSearch_MAXSAT_flips_explored_detail_BI);
	REGISTER(test_LocalSearch_MAXSAT_flips_explored_detail_FI);

	REGISTER(test_TSPFactory);
	REGISTER(test_ClusteringFactory);
	REGISTER(test_MAXSATFactory);

	REGISTER(test_thread_pool);
	REGISTER(test_thread_pool_hdf5_read);
	REGISTER(test_thread_pool_hdf5_write);
	REGISTER(test_thread_pool_TSPFactory);
	*/
	REGISTER(main_write_data);
	// REGISTER(test_gather_metadata);
#define DEBUG
	for (int i = 0; i < functions_names.size(); i++)
	{
		try
		{
			functions_to_run.at(i)(argc,argv);
			cout << "\x1B[32m " << functions_names.at(i) << ":OK \033[0m " << endl;
		}
		// catch all runtime errors
		catch (const runtime_error& e)
		{
			cout << "\x1B[31m " << functions_names.at(i) << ":FAIL \033[0m " << endl;
			cout << e.what() << endl;
		}
	}
}