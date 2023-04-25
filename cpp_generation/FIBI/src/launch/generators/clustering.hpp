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

vector<pair<string, double>>* clusteringfactory_run(map<string,long>* args, string root_data, int thread_id) {
	
	ClusteringFactory f;
	ClusteringConfig cf(args);

	vector<pair<string, double>>* success = f.run(cf, root_data);
	if (args->at("SEED_GLOB") % 100 == 0) {
		cout << "\x1B[32m \tOK ";
		cf.print();
		cout << "\033[0m " << endl;
	}
	delete args;
	return success;
}
template <int seed_stop = -1>
void run_clustering(Args arguments, const unique_ptr < set<int>>& missing) {
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
	vector<int> l_seeds_assign(arguments.num_rep, 0);
	iota(l_seeds_assign.begin(), l_seeds_assign.end(), 0);

	vector<int> values;
	long i = 0;
	if (arguments.dataset == 0) {
		// Dataset 0: Uniform
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					const int NUM_CLUST = num_clusters.at(NUM_CLUST_i);
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						const int NUM_POINTS = num_points.at(NUM_POINTS_i);
						i++;
					}
				}
			}
		}
	}
	else if (arguments.dataset == 1) {
		// Dataset 1: Franti
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int seed_problem_i = 0; seed_problem_i < seeds_problem_franti.size(); seed_problem_i++) {
					const int seed_problem = seeds_problem_franti.at(seed_problem_i);
					i++;
				}
			}
		}
	}
	else if (arguments.dataset == 2) {
		// Dataset 2: Aloise
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int seed_problem_i = 0; seed_problem_i < seeds_problem_aloise.size(); seed_problem_i++) {
					const int seed_problem = seeds_problem_aloise.at(seed_problem_i);
					for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
						const int NUM_CLUST = num_clusters.at(NUM_CLUST_i);
						i++;
					}
				}
			}
		}
	}
	else if (arguments.dataset == 3) {
		// Dataset 3: Normal dataset
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					const int NUM_CLUST = num_clusters.at(NUM_CLUST_i);
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						const int NUM_POINTS = num_points.at(NUM_POINTS_i);
						i++;
					}
				}
			}
		}
	}
	const int num_poss = i;
	cout << num_poss << " TODO" << endl;
	Progress progress(num_poss, "\033[31m", "Queued");

	auto add_queue = [&pool, &progress, &missing, &arguments](
		long i,
		int seed_problem, int seed_assign, int FI_BI, int impr, int num_clust, int num_dim, int num_point, int dataset) {
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
		map<string,long>* args = new map<string,long>{
			{"DATASET",dataset},
			{"SEED_GLOB",seed_glob},
			{"SEED_PROBLEM",seed_problem},
			{"SEED_ASSIGN",seed_assign},
			{"FI_BI",FI_BI},
			{"IMPR",impr},
			{"NUM_POINTS",num_point},
			{"NUM_DIM",num_dim},
			{"NUM_CLUST",num_clust}
		};
		pool.submit(clusteringfactory_run, args, arguments.root_data);
		progress.print(i);
		return;
	};
	i = 0;
	if (arguments.start_seed == -1)
	{
		cout << "Out because start seed" << endl;
		return;
	}
	if (arguments.dataset == 0) {
		// Dataset 0: Uniform
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					const int NUM_CLUST = num_clusters.at(NUM_CLUST_i);
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						const int NUM_POINTS = num_points.at(NUM_POINTS_i);
						add_queue(i, seed_assign, seed_assign, FI_BI, arguments.impr, NUM_CLUST, 2, NUM_POINTS, arguments.dataset);
						i++;
					}
				}
			}
		}
	}
	else if (arguments.dataset == 1) {
		// Dataset 1: Franti
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int seed_problem_i = 0; seed_problem_i < seeds_problem_franti.size(); seed_problem_i++) {
					const int seed_problem = seeds_problem_franti.at(seed_problem_i);
					add_queue(i, seed_problem, seed_assign, FI_BI, arguments.impr, -1, -1, -1, arguments.dataset);
					i++;
				}
			}
		}
	}
	else if (arguments.dataset == 2) {
		// Dataset 2: Aloise
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int seed_problem_i = 0; seed_problem_i < seeds_problem_aloise.size(); seed_problem_i++) {
					const int seed_problem = seeds_problem_aloise.at(seed_problem_i);
					for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
						const int NUM_CLUST = num_clusters.at(NUM_CLUST_i);
						add_queue(i, seed_problem, seed_assign, FI_BI, arguments.impr, NUM_CLUST, -1, -1, arguments.dataset);
						i++;
					}
				}
			}
		}
	}
	else if (arguments.dataset == 3) {
		// Dataset 3: Normal dataset
		for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
			for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
				for (int NUM_CLUST_i = 0; NUM_CLUST_i < num_clusters.size(); NUM_CLUST_i++) {
					const int NUM_CLUST = num_clusters.at(NUM_CLUST_i);
					for (int NUM_POINTS_i = 0; NUM_POINTS_i < num_points.size(); NUM_POINTS_i++) {
						const int NUM_POINTS = num_points.at(NUM_POINTS_i);
						add_queue(i, seed_assign, seed_assign, FI_BI, arguments.impr, NUM_POINTS, 2, NUM_CLUST, arguments.dataset);
						i++;
					}
				}
			}
		}
	}
	else {
		cout << "Unknow dataset" << endl;
	}
	return;
}

void run_clustering_full(Args arguments, const unique_ptr < set<int>>& missing) {
	if (arguments.dataset != -1 && arguments.impr == -1) {
		cout << "DATASET per DATASET" << endl;
		for (int impr = 0; impr < 6; impr++) {
			arguments.set_impr(impr);
			run_clustering<-1>(arguments, missing);
		}
	}
	else {
		cout << "Full possibility" << endl;
		for (int dataset = 0; dataset < 4; dataset++) {
			for (int impr = 0; impr < 6; impr++) {
				arguments.set_dataset(dataset);
				arguments.set_impr(impr);
				run_clustering<-1>(arguments, missing);
			}
		}
	}
}