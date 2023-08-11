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

class TSPFactory : public AbstractFactory<TSPConfig>
{
public:
	vector<pair<string, double>> *run(TSPConfig &cf, string root_data = "./") override
	{
		// setup
		const vector<double> *towns_pos_ptr;
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
		default:
			cout << "Invalid IMPR argument, Valid values are 0->1" << endl;
			throw invalid_argument("Invalid DATASET argument, Valid values are 0->1");
			break;
		}
		unique_ptr<const vector<double>> towns_pos(towns_pos_ptr);
		DistanceMatrix m(cf.NUM_TOWNS.get(), cf.NUM_DIM.get(), *towns_pos);
		vector<int> *tour_ptr;
		switch (cf.IMPR.get())
		{
		case 0:
			/* Random tour */
			tour_ptr = random_tour(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get());
			break;
		case 1:
			/* Improved random tour GREEDY RANDOMIZED*/
			tour_ptr = init_tsp_greedy_randomized(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m);
			break;
		case 2:
			/* Improved tour */
			tour_ptr = init_tsp_greedy(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m);
			break;
		case 3:
			/* Improved random tour TOP 3*/
			tour_ptr = init_tsp_greedy_topk(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m, 3);
			break;
		case 4:
			/* Improved random tour TOP 4*/
			tour_ptr = init_tsp_greedy_topk(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m, 4);
			break;
		case 5:
			/* Improved random tour TOP 5*/
			tour_ptr = init_tsp_greedy_topk(cf.NUM_TOWNS.get(), cf.SEED_ASSIGN.get(), m, 5);
			break;
		default:
			cout << "Invalid IMPR argument, Valid values are 0->5" << endl;
			throw invalid_argument("Invalid IMPR argument, Valid values are 0->5");
			break;
		}
		unique_ptr<vector<int>> tour(tour_ptr);
		TSPSolutionContainer co(*tour, m);
		Metrics<TSPSolutionContainer> metrics;
		vector<AlgorithmObserver<TSPSolutionContainer> *> obs;
		obs.push_back(&metrics);
		if (cf.OPT.get() == 1) {
			AlgorithmObservable<TSPInsertionSwap, TSPSolutionContainer>o(obs);
			InsertionNeighbourhood n(o, (bool)cf.FI_BI.get()); 
			FlipTSPInsertion f;
			LocalSearch<TSPInsertionSwap,TSPSolutionContainer,InsertionNeighbourhood,TSPConfig,FlipTSPInsertion> ls(n, o);
			ls.run(co,cf,f);
		}
		else if (cf.OPT.get() == 2)
		{
			AlgorithmObservable<TSPTwoOptSwap, TSPSolutionContainer>o(obs);
			TwoOptNeighbourhood n(o, (bool)cf.FI_BI.get()); 
			FlipTSPTwoOpt f;
			LocalSearch<TSPTwoOptSwap,TSPSolutionContainer,TwoOptNeighbourhood,TSPConfig,FlipTSPTwoOpt> ls(n, o);
			ls.run(co,cf,f);
		}
		else if (cf.OPT.get() == 3)
		{
			AlgorithmObservable<TSPThreeOptSwap, TSPSolutionContainer>o(obs);
			ThreeOptNeighbourhood n(o, (bool)cf.FI_BI.get()); 
			FlipTSPThreeOpt<> f;
			LocalSearch<TSPThreeOptSwap,TSPSolutionContainer,ThreeOptNeighbourhood,TSPConfig,FlipTSPThreeOpt<>> ls(n, o);
			ls.run(co,cf,f);
		}
		// writing the results
		vector<pair<string, double>> *res = get_results<TSPSolutionContainer>(&metrics, &cf);
		return res;
	}
};