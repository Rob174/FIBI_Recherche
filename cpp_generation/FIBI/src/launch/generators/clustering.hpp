#pragma once
#include <vector>
#include <map>
#include <chrono>
#include "../threapool.hpp"
#include "../../../src/launch/progress/progress.hpp"
#include "../../../src/factories/clustering.hpp"
#include "../../data/get_data.hpp"
#include "../../../src/launch/parseArgs.hpp"
#include "abstract.hpp"


using namespace std;

vector<pair<string, double>> clusteringfactory_run(const map<string, int>& args, string root_data, int thread_id) {
	
	ClusteringFactory f;
	ClusteringConfig cf(args);

	vector<pair<string, double>> success = f.run(cf, root_data);
	if (args.at("SEED_GLOB") % 100 == 0) {
		cout << "\x1B[32m \tOK ";
		cf.print();
		cout << "\033[0m " << endl;
	}
	return success;
}
template <int seed_stop = -1>
vector<pair<string, double>> run_clustering(Args arguments, const unique_ptr < set<int>>& missing) {
	MergeMetadata merger(arguments.merge_size, arguments.fileout);
	ThreadPool pool(merger, arguments.num_threads);
	// Define possibilities
	vector<int> num_clusters = { 2,4,8,16,32,64,128,256 };
	vector <int> num_points{ 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 };
	vector <int> seeds_problem_aloise;
	for (int i = 0; i < 25; i++) {
		seeds_problem_aloise.push_back(i);
	}
	vector<int> seeds_problem_franti{ 0,1,2,8,9,10,11,12,13,124,125,126,127,128 };
	vector<int>l_FI_BI{ 0, 1 };
	vector<int>l_impr{ 0, 1, 2 };
	vector<int> l_seeds_assign(arguments.num_rep, 0);
	iota(l_seeds_assign.begin(), l_seeds_assign.end(), 0);

	vector<int> values;
	int i = 0;
	// Dataset 0: Uniform
	ProductIterator it0({ l_seeds_assign, l_FI_BI, l_impr, num_clusters, num_points });
	while ((values = it0.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int NUM_CLUST = values[3];
		int NUM_POINTS = values[4];
		i++;
	}
	
	// Dataset 1: Franti
	ProductIterator it1({ l_seeds_assign, l_FI_BI, l_impr, seeds_problem_franti });
	while ((values = it1.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_problem = values[3];
		i++;
	}

	// Dataset 2: Aloise
	ProductIterator it2({ l_seeds_assign, l_FI_BI, l_impr, seeds_problem_aloise, num_clusters });
	while ((values = it2.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_problem = values[3];
		int num_clust = values[4];
		i++;
	}

	// Dataset 3: Normal
	ProductIterator it3({ l_seeds_assign, l_FI_BI, l_impr, num_clusters, num_points });
	while ((values = it3.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int NUM_CLUST = values[3];
		int NUM_POINTS = values[4];
		i++;
	}
	const int num_poss = i;
	i = 0;
	Progress progress(num_poss, "\033[31m", "Queued");

	auto add_queue = [&pool, &progress, &i, &missing, &arguments](
		int seed_problem, int seed_assign, int FI_BI, int impr, int num_clust, int num_dim, int num_point, int dataset) {
		map<string, int> args{
			{"DATASET",dataset},
			{"SEED_GLOB",i},
			{"SEED_PROBLEM",seed_problem},
			{"SEED_ASSIGN",seed_assign},
			{"FI_BI",FI_BI},
			{"IMPR",impr},
			{"NUM_POINTS",num_point},
			{"NUM_DIM",num_dim},
			{"NUM_CLUST",num_clust}
		};
		if (i > arguments.end_seed && arguments.end_seed != -1) { 
			i++;
			return;
		}
		if (i >= arguments.start_seed || (missing->find(i) != missing->end())) {
			pool.submit(clusteringfactory_run, args, arguments.root_data);
		}
		if (i < arguments.start_seed) progress.skip();
		if constexpr (seed_stop != -1 && seed_stop == i) {
			i++;
			return;
		}
		progress.print(i);
		i++;
		return;
	};
	// Dataset 0: Uniform
	it0.restart();
	while ((values = it0.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int NUM_CLUST = values[3];
		int NUM_POINTS = values[4];
		add_queue(seed_assign, seed_assign, FI_BI, impr, NUM_CLUST, 2, NUM_POINTS, 0);
	}
	
	// Dataset 1: Franti
	it1.restart();
	while ((values = it1.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_problem = values[3];
		add_queue(seed_problem, seed_assign, FI_BI, impr, -1, -1, -1, 1);
	}

	// Dataset 2: Aloise
	it2.restart();
	while ((values = it2.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int seed_problem = values[3];
		int num_clust = values[4];
		add_queue(seed_problem, seed_assign, FI_BI, impr, num_clust, -1, -1, 2);
	}

	// Dataset 3: Normal
	it3.restart();
	while ((values = it3.next()).size() > 0) {
		int seed_assign = values[0];
		int FI_BI = values[1];
		int impr = values[2];
		int NUM_CLUST = values[3];
		int NUM_POINTS = values[4];
		add_queue(seed_assign, seed_assign, FI_BI, impr, NUM_POINTS, 2, NUM_CLUST, 3);
	}
	return {};
}