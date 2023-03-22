#pragma once
#include "abstract.hpp"
#include "../data/get_data.hpp"
#include "../data/solution_container/clustering.hpp"
#include "../data/improvements/clustering.hpp"
#include "../algorithm/stats/metrics.hpp"
#include "../algorithm/factories.hpp"
#include "../utils.h"

using namespace std;

class ClusteringFactory : public AbstractFactory<ClusteringConfig> {
public:
	void run(ClusteringConfig& cf, bool dump_mapping = false, string root_data = "./") override
	{
		// setup
		const vector<double>* points_pos_ptr;
		switch (cf.DATASET.get())
		{
		case 0:
			/* Uniformly distrbuted points */
			points_pos_ptr = uniform_points(cf.NUM_POINTS.get(),
				cf.NUM_DIM.get(),
				cf.SEED_PROBLEM.get());
			break;
		case 1:
			points_pos_ptr = open_clustering(cf.SEED_PROBLEM.get(), root_data + "franti_benchmark.hdf5", &cf, true);
			break;
		case 2:
			points_pos_ptr = open_clustering(cf.SEED_PROBLEM.get(), root_data + "aloise_benchmark.hdf5", &cf, false);
			break;
		case 3:
			points_pos_ptr = normal_points(cf.NUM_POINTS.get(),
				cf.NUM_DIM.get(),
				cf.SEED_PROBLEM.get());
			break;
		default:
			throw invalid_argument("Invalid DATASET argument, Valid values are 0->2");
			break;
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
			kmeansPlusPlus(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get());
			break;
		case 2:
			/* KMeans++ super glutton */
			kmeansPlusPlusSuperGlutton(*centroids_ptr, *points_pos, *assignements_ptr, *npts_per_clust_ptr, cf.NUM_DIM.get(), cf.SEED_ASSIGN.get());
			break;
		default:
			throw invalid_argument("Invalid IMPR argument, Valid values are 0->2");
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
		if (dump_mapping)
		{
			save_mapping<ClusteringConfig, ClusteringSwap, ClusteringSolutionContainer<>>(&metrics, &cf, "mapping_clustering.json");
		}
		unique_ptr<typename clust_ls_t<>::ls_t> ls(getClusteringLocalSearch<>(obs));
		ls->run(co, cf);
		// writing the results
		/*create_dataset<ClusteringResult>(res);
		if (debug) {
			storage.save_json("actions.json");
		}*/
		// cleaning
		/*
		delete alg;
		delete metrics;
		delete res;
		*/
	}
};