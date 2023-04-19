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
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						i++;
					}
				}
			}
		}
	}
	// Dataset 1: Franti
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int seed_problem = 0; seed_problem < seeds_problem_franti.size(); seed_problem++) {
					i++;
				}
			}
		}
	}

	// Dataset 2: Aloise
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int seed_problem = 0; seed_problem < seeds_problem_aloise.size(); seed_problem++) {
					for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
						i++;
					}
				}
			}
		}
	}

	// Dataset 3: Normal dataset
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						i++;
					}
				}
			}
		}
	}
	const int num_poss = i;
	i = 0;
	Progress progress(num_poss, "\033[31m", "Queued");

	auto add_queue = [&pool, &progress, &i, &missing, &arguments](
		int seed_problem, int seed_assign, int FI_BI, int impr, int num_clust, int num_dim, int num_point, int dataset) {
			bool stop;
			stop = i > arguments.end_seed && arguments.end_seed != -1;
			stop = stop || (i < arguments.start_seed);
			stop = stop && missing->find(i) == missing->end();
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
			return;
		}
		if (i >= arguments.start_seed || (missing->find(i) != missing->end())) {
			pool.submit(clusteringfactory_run, args, arguments.root_data);
		}
		if (i < arguments.start_seed) progress.skip();
		if constexpr (seed_stop != -1 && seed_stop == i) {
			return;
		}
		progress.print(i);
		return;
	};
	// Dataset 0: Uniform
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					const int NUM_CLUST = num_clusters[NUM_CLUST_i];
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						const int NUM_POINTS = num_points[NUM_POINTS_i];
						add_queue(seed_assign, seed_assign, FI_BI, impr, NUM_CLUST, 2, NUM_POINTS, 0);
						i++;
					}
				}
			}
		}
	}
	
	// Dataset 1: Franti
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int seed_problem = 0; seed_problem < seeds_problem_franti.size(); seed_problem++) {
					add_queue(seed_problem, seed_assign, FI_BI, impr, -1, -1, -1, 1);
					i++;
				}
			}
		}
	}

	// Dataset 2: Aloise
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int seed_problem = 0; seed_problem < seeds_problem_aloise.size(); seed_problem++) {
					for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
						const int NUM_CLUST = num_clusters[NUM_CLUST_i];
						add_queue(seed_problem, seed_assign, FI_BI, impr, NUM_CLUST, -1, -1, 2);
						i++;
					}
				}
			}
		}
	}

	// Dataset 3: Normal
	for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 3; impr++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					const int NUM_CLUST = num_clusters[NUM_CLUST_i];
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						const int NUM_POINTS = num_points[NUM_POINTS_i];
						add_queue(seed_assign, seed_assign, FI_BI, impr, NUM_POINTS, 2, NUM_CLUST, 3);
						i++;
					}
				}
			}
		}
	}
	
	return {};
}