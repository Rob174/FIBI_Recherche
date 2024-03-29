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

vector<pair<string, double>> *tspfactory_run(map<string, long> *args, string root_data, int thread_id)
{

	TSPFactory f;
	TSPConfig cf(args);
	// Define start time for performanest measurement
	// chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
	vector<pair<string, double>> *success = f.run(cf, root_data);
	// Get end time for performance measurement
	// chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
	// Print performance in format Duration: x seconds y milliseconds z microseconds
	// int milliseconds = chrono::duration_cast<chrono::milliseconds>(end - start).count();

	// cout << "| " << cf.NUM_TOWNS.get() << " | " << milliseconds << " | " << endl;

	if (args->at("SEED_GLOB") % 100 == 0)
	{
		cout << "\x1B[32m \tOK ";
		cf.print();
		cout << "\033[0m " << endl;
	}
	return success;
}
template <int seed_stop = -1>
void run_tsp(Args arguments, const unique_ptr<set<int>> &missing)
{
	MergeMetadata merger(arguments.merge_size, arguments.fileout);
	ThreadPool pool(merger, arguments.num_threads);
	// Define possibilities
	vector<int> num_towns{20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
	int i = 0;
	vector<int> seeds_assign(arguments.num_rep, 0);
	iota(seeds_assign.begin(), seeds_assign.end(), 0);
	if (arguments.dataset == 0)
	{
		for (int seed_assign = 0; seed_assign < arguments.num_rep; seed_assign++)
		{
			for (int FI_BI = 0; FI_BI < 2; FI_BI++)
			{
				for (int NUM_TOWNS : num_towns)
				{
					if (arguments.town_max != -1 && NUM_TOWNS > arguments.town_max)
						continue;
					i++;
				}
			}
		}
	}
	else if (arguments.dataset == 1)
	{
		for (int seed_assign = 0; seed_assign < arguments.num_rep; seed_assign++)
		{
			for (int FI_BI = 0; FI_BI < 2; FI_BI++)
			{
				for (int seed_problem = 0; seed_problem < 49; seed_problem++)
				{
					i++;
				}
			}
		}
	}
	const int num_poss = i;
	cout << num_poss << " TODO" << endl;
	i = 0;
	if (arguments.start_seed == -1)
	{
		cout << "Out because start seed" << endl;
		return;
	}
	Progress progress(num_poss, "\033[31m", "Queued");
	auto add_queue = [&pool, &progress, &missing, &arguments](
						 long i,
						 int seed_problem, int seed_assign, int FI_BI, int impr, int num_dim, int num_towns, int dataset)
	{
		bool to_execute;
		const bool in_interval = (i <= arguments.end_seed || arguments.end_seed == -1) && (i >= arguments.start_seed);
		const bool not_in_interval = !in_interval;
		const bool is_missing = missing->find(i) != missing->end();
		if (arguments.do_missings == 0)
		{
			// do not do missing
			to_execute = in_interval;
		}
		else if (arguments.do_missings == 1)
		{
			// do missing no matter interval
			to_execute = is_missing;
		}
		else if (arguments.do_missings == 2)
		{
			// do missing in interval
			to_execute = (in_interval && is_missing);
		}
		else
		{
			cout << "Unknown do_missings option: " << arguments.do_missings << endl;
			throw runtime_error("Unknown do_missings option");
		}
		if (!to_execute)
		{
			progress.skip();
			return;
		}
		long seed_glob = i;
		map<string, long> *args = new map<string, long>{
			{"DATASET", dataset},
			{"SEED_GLOB", seed_glob},
			{"FI_BI", FI_BI},
			{"IMPR", impr},
			{"NUM_TOWNS", num_towns},
			{"NUM_DIM", num_dim},
			{"SEED_ASSIGN", seed_assign},
			{"SEED_PROBLEM", seed_problem},
			{"OPT", arguments.opt}};
		pool.submit(tspfactory_run, args, arguments.root_data);
		progress.print(i);
	};
	if (arguments.dataset == 0)
	{
		for (int seed_assign = 0; seed_assign < arguments.num_rep; seed_assign++)
		{
			for (int FI_BI = 0; FI_BI < 2; FI_BI++)
			{
				for (int NUM_TOWNS : num_towns)
				{
					if (arguments.town_max != -1 && NUM_TOWNS > arguments.town_max)
						continue;
					add_queue(i, seed_assign, seed_assign, FI_BI, arguments.impr, 2, NUM_TOWNS, arguments.dataset);
					i++;
				}
			}
		}
	}
	else if (arguments.dataset == 1)
	{
		for (int seed_assign = 0; seed_assign < arguments.num_rep; seed_assign++)
		{
			for (int FI_BI = 0; FI_BI < 2; FI_BI++)
			{
				for (int seed_problem = 0; seed_problem < 49; seed_problem++)
				{
					add_queue(i, seed_problem, seed_assign, FI_BI, arguments.impr, 2, -1, arguments.dataset);
					i++;
				}
			}
		}
	}
	else
	{
		cout << "Unknow dataset" << endl;
	}
};
void run_tsp_full(Args arguments, const unique_ptr<set<int>> &missing)
{
	if (arguments.dataset != -1 && arguments.impr == -1)
	{
		cout << "DATASET per DATASET" << endl;
		for (int impr = 0; impr < 6; impr++)
		{
			arguments.set_impr(impr);
			run_tsp<-1>(arguments, missing);
		}
	}
	else
	{
		cout << "Full possibility" << endl;
		for (int dataset = 0; dataset < 2; dataset++)
		{
			for (int impr = 0; impr < 6; impr++)
			{
				arguments.set_dataset(dataset);
				arguments.set_impr(impr);
				run_tsp<-1>(arguments, missing);
			}
		}
	}
}