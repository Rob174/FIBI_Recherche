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

vector<pair<string, double>>* MAXSATfactory_run(map<string,long>* args, string root_data, int thread_id) {
	MAXSATFactory f;
	MAXSATConfig cf(args);

	vector<pair<string, double>>* success = f.run(cf, root_data);
	if (args->at("SEED_GLOB") % 100 == 0) {
		cout << "\x1B[32m \tOK ";
		cf.print();
		cout << "\033[0m " << endl;
	}
	return success;

}
template <int seed_stop = -1>
void run_maxsat(Args arguments, const unique_ptr < set<int>>& missing) {
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

	vector<int> values;
	// Dataset 1: MAXSAT benchmark
	if(arguments.dataset == 1) {
		// Dataset 1: Maxsat evaluation benchmark 2021
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int seed_problem_i = 0; seed_problem_i < seeds_problem.size(); seed_problem_i++) {
					const int seed_problem = seeds_problem.at(seed_problem_i);
					const int impr = arguments.impr;
					i++;
				}
			}
		}
	}
	else {
		cout << "Unknow dataset" << endl;
	}
	const int num_poss = i;
	Progress progress(num_poss, "\033[31m", "Queued");
	auto add_queue = [&pool, &progress, &missing, &arguments](int i, int seed_problem, int seed_assign, int FI_BI, int impr, int num_variables, int num_clauses, int dataset) {
		bool stop;
		stop = i > arguments.end_seed && arguments.end_seed != -1;
		stop = i > seed_stop && seed_stop != -1;
		stop = stop || (i < arguments.start_seed);
		stop = stop && missing->find(i) == missing->end();
		if (stop) {
			progress.skip();
			return;
		}
		long seed_glob = i;
		map<string,long>* args=new map<string,long>{
			{"DATASET",dataset},
			{"SEED_GLOB",seed_glob},
			{"SEED_PROBLEM",seed_problem},
			{"SEED_ASSIGN",seed_assign},
			{"FI_BI",FI_BI},
			{"IMPR",impr},
			{"NUM_VARIABLES",num_variables},
			{"NUM_CLAUSES",num_clauses}
		};
		MAXSATfactory_run(args, arguments.root_data,0);
		progress.print(i);
		return;
	};
	// Dataset 1: Uniform
	i = 0;
	if(arguments.dataset == 1) {
		// Dataset 1: Maxsat evaluation benchmark 2021
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int seed_problem_i = 0; seed_problem_i < seeds_problem.size(); seed_problem_i++) {
					const int seed_problem = seeds_problem.at(seed_problem_i);
					add_queue(i,seed_problem,seed_assign,FI_BI,arguments.impr,-1,-1,arguments.dataset);
					i++;
				}
			}
		}
	}
	else {
		cout << "Unknow dataset" << endl;
	}
	return;
}

void run_maxsat_full(Args arguments, const unique_ptr < set<int>>& missing) {
	if (arguments.dataset != -1 && arguments.impr == -1) {
		cout << "DATASET per DATASET" << endl;
		for (int impr = 0; impr < 2; impr++) {
			arguments.set_impr(impr);
			run_maxsat<-1>(arguments, missing);
		}
	}
	else {
		cout << "Full possibility" << endl;
		for (int dataset = 1; dataset < 2; dataset++) {
			for (int impr = 0; impr < 2; impr++) {
				arguments.set_dataset(dataset);
				arguments.set_impr(impr);
				run_maxsat<-1>(arguments, missing);
			}
		}
	}
}