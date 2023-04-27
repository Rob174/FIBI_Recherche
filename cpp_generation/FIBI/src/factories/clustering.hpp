#pragma once
#include "abstract.hpp"
#include "../data/get_data.hpp"
#include "../data/solution_container/clustering.hpp"
#include "../data/improvements/clustering.hpp"
#include "../algorithm/stats/metrics.hpp"
#include "../algorithm/factories.hpp"
#include "../utils.h"
#include "../out/abstract.hpp"
#include <tuple>

using namespace std;

class ClusteringFactory : public AbstractFactory<ClusteringConfig> {
public:
	vector<pair<string, double>>* run(ClusteringConfig& cf, string root_data = "./") override
	{
		// setup
		const vector<double>* points_pos_ptr;
		string dataset_name;
		switch (cf.DATASET.get())
		{
		case 0:
			/* Uniformly distrbuted points */
			points_pos_ptr = uniform_points(cf.NUM_POINTS.get(),
				cf.NUM_DIM.get(),
				cf.SEED_PROBLEM.get());
			break;
		case 1:
#if HDF5save
			dataset_name = "franti_benchmark.hdf5";
#else
			dataset_name = "franti_benchmark/";
#endif
			points_pos_ptr = open_clustering(cf.SEED_PROBLEM.get(), root_data + dataset_name, &cf, true);
			break;
		case 2:
#if HDF5save
			dataset_name = "aloise_benchmark.hdf5";
#else
			dataset_name = "aloise_benchmark/";
#endif
			points_pos_ptr = open_clustering(cf.SEED_PROBLEM.get(), root_data + dataset_name, &cf, false);
			break;
		case 3:
			points_pos_ptr = get<0>(normal_points(cf.NUM_POINTS.get(),
				cf.NUM_DIM.get(),
				cf.NUM_CLUST.get(),
				cf.SEED_PROBLEM.get())
			);
			break;
		default:
			string error = "Invalid DATASET argument, Valid values are 0->3";
			cout << error << endl;
			throw invalid_argument(error);
			break;
		}
		if (cf.NUM_POINTS.get() <= cf.NUM_CLUST.get())
		{
			vector<pair<string, double>>*res = new vector<pair<string, double>>;
			for (pair<string, double> p : cf.get_json()) {
				res->push_back(p);
			}
			return res;
		}
		unique_ptr<const vector<double>> points_pos(points_pos_ptr);
		vector<int>* assignements_ptr = random_clust(cf.NUM_CLUST.get(), cf.NUM_POINTS.get(), cf.SEED_ASSIGN.get());
		auto [centroids_ptr, npts_per_clust_ptr] = cmpt_centroids_and_npts_per_clust(*points_pos, *assignements_ptr, cf.NUM_DIM.get(), cf.NUM_CLUST.get());
		switch (cf.IMPR.get())
		{
		case 0:
			/* Random assignement: already done*/
			break;
		case 1:
			/* Kmeans++ */
			init_clustering_greedy_randomized(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get());
			break;
		case 2:
			/* KMeans++ super glutton */
			init_clustering_greedy(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get());
			break;
		case 3:
			/* KMeans++ super glutton */
			init_clustering_greedy_topk(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get(),3);
			break;
		case 4:
			/* KMeans++ super glutton */
			init_clustering_greedy_topk(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get(), 4);
			break;
		case 5:
			/* KMeans++ super glutton */
			init_clustering_greedy_topk(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get(), 5);
			break;
		default:
			string error = "Invalid IMPR argument, Valid values are 0->5";
			cout << error << endl;
			throw invalid_argument(error);
			break;
		}
		unique_ptr<vector<int>> assignements(assignements_ptr);
		unique_ptr<vector<double>> centroids(centroids_ptr);
		unique_ptr<vector<int>> npts_per_clust(npts_per_clust_ptr);
		ClusteringSolutionContainer<> co(*points_pos, *assignements, cf);

		Metrics<ClusteringSwap, ClusteringSolutionContainer<>> metrics;
		// algorithms execution
		vector<clustering_obs_t* > obs;
		obs.push_back(&metrics);
		unique_ptr<typename clust_ls_t::ls_t> ls(getClusteringLocalSearch(obs, (bool)cf.FI_BI.get()));
		ls->run(co, cf);
		vector<pair<string, double>>* res = get_results<ClusteringSwap, ClusteringSolutionContainer<>>(&metrics, &cf);
		return res;
	}
};