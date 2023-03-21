#pragma once

#include <cmath>
#include "abstract.hpp"
#include "../../../data/solution_container/clustering.hpp"
#include "../../../data/constants/clustering.hpp"

template <bool FI = false, const double EPSILON = 1e-5>
class ClusteringNeighbourhoodExplorer
{
private:
	AlgorithmObservable<ClusteringSwap, ClusteringSolutionContainer<>>* o;
public:
	ClusteringNeighbourhoodExplorer(AlgorithmObservable<ClusteringSwap, ClusteringSolutionContainer<>>* o) : o(o) {};
	bool explore_flips(ClusteringSolutionContainer<>& co, const ClusteringConfig& cf) const
	{
		double delta = 0;
		ClusteringSwap chosen_swap;
		for (point_id_t point_moving_id = 0; point_moving_id < cf.num_choices(); point_moving_id++)
		{
			cluster_id_t from_cluster_id = co.c_a[point_moving_id];
			for (int i = from_cluster_id + 1; i < from_cluster_id + cf.NUM_CLUST.get(); i++)
			{
				const cluster_id_t to_clust_id = i % cf.NUM_CLUST.get();
				ClusteringSwap tmp_swap(point_moving_id, from_cluster_id, to_clust_id);
				double delta_ij = co.test_flip(tmp_swap);
				if (delta_ij < -EPSILON && delta_ij < delta)
				{
					delta = delta_ij;
					chosen_swap = tmp_swap;
					o->on_glob_iter_end(co, delta, tmp_swap);
					if constexpr (FI)
					{
						co.flip(chosen_swap, delta);
						o->on_iter_end(co, chosen_swap);
						return false;
					}
				}
			}
		}
		if (abs(delta) < EPSILON)
		{
			o->on_iter_end(co, chosen_swap);
			return true;
		}
		// Update cluster assignements
		co.flip(chosen_swap, delta);
		o->on_iter_end(co, chosen_swap);
		return false;
	}
};