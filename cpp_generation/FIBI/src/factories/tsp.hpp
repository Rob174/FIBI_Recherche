#pragma once
#include "abstract.hpp"
#include "../data/get_data.hpp"
#include "../data/solution_container/distance_matrix.hpp"
#include "../data/solution_container/tsp.hpp"
#include "../algorithm/stats/metrics.hpp"
#include "../algorithm/factories.hpp"
#include "../utils.h"
#include "../out/abstract.hpp"

using namespace std;

class TSPFactory : public AbstractFactory<TSPConfig> {
public:
	void run(TSPConfig& cf, bool dump_mapping = false, string root_data = "./", bool clean = false) override
	{
		// setup
		const vector<double>* towns_pos_ptr;
		string dataset_name;
		switch (cf.DATASET.get())
		{
		case 0:
			/* Uniformly distrbuted points */
			towns_pos_ptr = uniform_points(cf.NUM_TOWNS.get(),
				cf.NUM_DIM.get(),
				cf.SEED_PROBLEM.get());
			break;
		case 1:

#if HDF5save
			dataset_name = "tsplib.hdf5";
#else
			dataset_name = "tsplib/";
#endif
			towns_pos_ptr = open_tsplib(cf.SEED_PROBLEM.get(), root_data + dataset_name, &cf);
			break;
		case 2:
			towns_pos_ptr = normal_points(cf.NUM_TOWNS.get(),
				cf.NUM_DIM.get(),
				cf.SEED_PROBLEM.get());
			break;
		default:
			throw invalid_argument("Invalid DATASET argument, Valid values are 0->2");
			break;
		}
		unique_ptr<const vector<double>> towns_pos(towns_pos_ptr);
		DistanceMatrix m(cf.NUM_TOWNS.get(), cf.NUM_DIM.get(), *towns_pos);
		vector<int>* tour_ptr;
		switch (cf.IMPR.get())
		{
		case 0:
			/* Random tour */
			tour_ptr = random_tour(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get());
			break;
		case 1:
			/* Improved tour */
			tour_ptr = improved_tour(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m);
			break;
		case 2:
			/* Improved random tour TOP 3*/
			tour_ptr = improved_rand_tour(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m, 3);
			break;
		case 3:
			/* Improved random tour TOP 4*/
			tour_ptr = improved_rand_tour(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m, 4);
			break;
		case 4:
			/* Improved random tour TOP 5*/
			tour_ptr = improved_rand_tour(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m, 5);
			break;
		default:
			throw invalid_argument("Invalid IMPR argument, Valid values are 0->4");
			break;
		}
		unique_ptr<vector<int>> tour(tour_ptr);
		TSPSolutionContainer<> co(*tour, m);
		Metrics<TSPSwap, TSPSolutionContainer<>> metrics;
		// algorithms execution
		vector<tsp_obs_t* > obs;
		obs.push_back(&metrics);
		if (dump_mapping)
		{
			save_mapping<TSPConfig, TSPSwap, TSPSolutionContainer<>>(&metrics, &cf, "mapping_tsp.json");
		}
		unique_ptr<typename tsp_ls_t<>::ls_t> ls(getTSPLocalSearch<>(obs));
		ls->run(co, cf);
		// writing the results
		vector<pair<string, double>> res = get_results<TSPSwap, TSPSolutionContainer<>>(&metrics, &cf);
		if (clean) {
			clean_dataset("dataset_tsp.hdf5");
		}
		save_metadata<>(cf.SEED_GLOB.get(), res, "dataset_tsp.hdf5");
	}
};