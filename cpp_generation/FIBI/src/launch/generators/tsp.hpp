#pragma once
#include <vector>
#include <map>
#include <chrono>
#include "../threapool.hpp"
#include "../../../src/launch/progress/progress.hpp"
#include "../../../src/factories/tsp.hpp"
#include "../../../src/launch/parseArgs.hpp"
#include "abstract.hpp"


using namespace std;

vector<pair<string, double>>* tspfactory_run(map<string,long>* args, string root_data, int thread_id) {

	TSPFactory f;
	TSPConfig cf(args);

	vector<pair<string, double>>* success = f.run(cf, root_data);
	if (args->at("SEED_GLOB") % 100 == 0) {
		cout << "\x1B[32m \tOK ";
		cf.print();
		cout << "\033[0m " << endl;
	}
	return success;
}
template <int seed_stop = -1>
bool run_tsp(Args arguments, const unique_ptr <set<int>>& missing) {
	MergeMetadata merger(arguments.merge_size, arguments.fileout);
	ThreadPool pool(merger, arguments.num_threads);
	// Define possibilities
	vector<int> range_0_26(26);
	iota(range_0_26.begin(), range_0_26.end(), 0);
	vector<int> range_0_49(49);
	iota(range_0_49.begin(), range_0_49.end(), 0);
	map<int, vector<int>> seeds_problem{
		{0,range_0_26},
		{1,range_0_49},
		{2,range_0_26}
	};
	// map num_towns map for each dataset a function that takes the seed_problem and returns the number of towns
	vector <int> num_towns{ 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 };
	map<int, function<int(int)>> num_towns_map{
		{0, [&num_towns](int seed_problem) {return num_towns.at(seed_problem); }},
		{1, [](int seed_problem) {return -1; }},
		{2, [&num_towns](int seed_problem) {return num_towns.at(seed_problem); }}
	};
	int i = 0;
	vector<int>datasets{ 0,1 };
	vector<int>FI_BI{ 0, 1};
	vector<int>impr{ 0, 1, 2, 3, 4};
	vector<int> seeds_assign(arguments.num_rep, 0);
	iota(seeds_assign.begin(), seeds_assign.end(), 0);
	
	ProductIterator it1({ datasets, FI_BI, impr, seeds_assign });
	vector<int> values;
	while ((values = it1.next()).size() > 0) {
		int dataset = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_assign = values[3];
		for (int seed_problem : seeds_problem.at(dataset)) {
			if (dataset == 1 && impr == 1) continue; //tsplib with improvement will give always the same result
			i++;
		}
	}
	const int num_poss = i;
	i = 0;
	Progress progress(num_poss, "\033[31m","Queued");
	it1.restart();
	while ((values = it1.next()).size() > 0) {
		int dataset = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_assign = values[3];
		for (int seed_problem : seeds_problem.at(dataset)) {
			if (dataset == 1 && impr == 1) continue; //tsplib with improvement will give always the same result
			map<string,long> *args = new map<string,long>{
				{"DATASET",dataset},
				{"SEED_GLOB",i},
				{"FI_BI",FI_BI},
				{"IMPR",impr},
				{"NUM_TOWNS",num_towns_map.at(dataset)(seed_problem)},
				{"NUM_DIM",2}
			};
			if (dataset == 1) {// TSPLIB has different 
				(*args)["SEED_PROBLEM"] = seed_problem;
				(*args)["SEED_ASSIGN"] = seed_assign;
			} else {
				(*args)["SEED_PROBLEM"] = seed_assign;
				(*args)["SEED_ASSIGN"] = seed_assign;
			}
			if (i > arguments.end_seed && arguments.end_seed != -1) return false;
			if (i >= arguments.start_seed || (missing->find(i) != missing->end())) {
				if (pool.submit(tspfactory_run, args, arguments.root_data)) {
					cout << "Restarting..." << endl;
					return true;
				}
			}
			if constexpr (seed_stop != -1) {
				if (seed_stop == i) return false;
			}
			progress.print(i);

			i++;
		}
	}
	return false;
}