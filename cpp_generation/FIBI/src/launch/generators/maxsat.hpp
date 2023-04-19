#pragma once
#include <vector>
#include <map>
#include <chrono>
#include "../threapool.hpp"
#include "../../../src/launch/progress/progress.hpp"
#include "../../../src/factories/maxsat.hpp"
#include "../../../src/launch/parseArgs.hpp"
#include "../../data/get_data.hpp"
#include "abstract.hpp"


using namespace std;

vector<pair<string, double>> MAXSATfactory_run(const map<string, int>& args, string root_data, int thread_id) {
	MAXSATFactory f;
	MAXSATConfig cf(args);

	vector<pair<string, double>> success = f.run(cf, root_data);
	if (args.at("SEED_GLOB") % 100 == 0) {
		cout << "\x1B[32m \tOK ";
		cf.print();
		cout << "\033[0m " << endl;
	}
	return success;

}
template <int seed_stop = -1>
vector<pair<string, double>> run_maxsat(Args arguments, const unique_ptr < set<int>>& missing) {
	MergeMetadata merger(arguments.merge_size, arguments.fileout);
	ThreadPool pool(merger, arguments.num_threads);
	// Define possibilities
	int i = 0;
	vector<int> seeds_problem;
	for (int i = 0; i < 29; i++) {
		seeds_problem.push_back(i);
	}
	vector<int>l_FI_BI{ 0, 1 };
	vector<int>l_impr{ 0, 1 };
	vector<int> l_seeds_assign(arguments.num_rep, 0);
	iota(l_seeds_assign.begin(), l_seeds_assign.end(), 0);

	vector<int> values;
	// Dataset 1: MAXSAT benchmark
	ProductIterator it0({ l_seeds_assign, l_FI_BI, l_impr, seeds_problem });
	while ((values = it0.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_problem = values[3];
		i++;
	}
	const int num_poss = i;
	i = 0;
	Progress progress(num_poss, "\033[31m", "Queued");

	auto add_queue = [&pool, &progress, &i, &missing, &arguments](int seed_problem, int seed_assign, int FI_BI, int impr, int num_variables, int num_clauses, int dataset) {
		map<string, int> args{
			{"DATASET",dataset},
			{"SEED_GLOB",i},
			{"SEED_PROBLEM",seed_problem},
			{"SEED_ASSIGN",seed_assign},
			{"FI_BI",FI_BI},
			{"IMPR",impr},
			{"NUM_VARIABLES",num_variables},
			{"NUM_CLAUSES",num_clauses}
		};
		if (i > arguments.end_seed && arguments.end_seed != -1) return vector<pair<string, double>>{};
		if (i >= arguments.start_seed || (missing->find(i) != missing->end())) {
			if (pool.submit(MAXSATfactory_run, args, arguments.root_data)) {
				cout << "Restarting..." << endl;
				return vector<pair<string, double>>{};
			}
		}
		if (i < arguments.start_seed)progress.skip();
		if constexpr (seed_stop != -1) {
			if (seed_stop == i) return vector<pair<string, double>>{};
		}
		progress.print(i);
		i++;
		return vector<pair<string, double>>{};
	};
	// Dataset 0: Uniform
	it0.restart();
	while ((values = it0.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_problem = values[3];
		add_queue(seed_problem, seed_assign, FI_BI, impr, -1, -1, 1);
	}
	return {};
}