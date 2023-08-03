#pragma once

#include <cmath>
#include "abstract.hpp"
#include "../../../data/solution_container/clustering.hpp"
#include "../../../data/flips/abstract.hpp"
#include "../../../data/constants/clustering.hpp"
#include "../../../utils.h"

class ClusteringNeighbourhoodExplorer : public NeighbourhoodExplorer<ClusteringSolutionContainer, ClusteringConfig, FlipClustering> 
{
private:
	AlgorithmObservable<ClusteringSwap, ClusteringSolutionContainer>& o;
	bool FI;
public:
	ClusteringNeighbourhoodExplorer(AlgorithmObservable<ClusteringSwap, ClusteringSolutionContainer>& o, bool FI = false) : o(o), FI(FI) {};
	/** @brief Explore all possible flips and choose the best one
	(first clustering with improving cost if @tparam FI = true, best solution otherwise).
	Explores for each point, each possibility of destination cluster}
	* * Note: the destination cluster must be different from the origin cluster (explains the k-1)
	* * Expecting number of moves with n point and k clusters
	*
		   n
		______
		╲
		 ╲     ⎛    k         ⎞
		  ╲    ⎜   ___        ⎟
		   ╲   ⎜   ╲          ⎟
		   ╱   ⎜   ╱     1 - 1⎟ = (k - 1) ⋅ n
		  ╱    ⎜   ‾‾‾        ⎟
		 ╱     ⎜c    = 1      ⎟
		╱      ⎝ dst          ⎠
		‾‾‾‾‾‾
		 p = 1

	*/
	bool explore_flips(ClusteringSolutionContainer& co, const ClusteringConfig& cf, FlipClustering& f) const
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
				double delta_ij = f.test_flip(tmp_swap, co);
				o.on_test_end(co, delta_ij, tmp_swap);
				if (delta_ij < -EPSILON && delta_ij < delta)
				{
					delta = delta_ij;
					chosen_swap = tmp_swap;
					o.on_glob_iter_end(co, delta, tmp_swap);
					if (FI)
					{
						f.flip(chosen_swap, delta, co);
						o.on_iter_end(co, chosen_swap);
						return false;
					}
				}
			}
		}
		if (abs(delta) < EPSILON)
		{
			o.on_iter_end(co, chosen_swap);
			return true;
		}
		// Update cluster assignements
		f.flip(chosen_swap, delta, co);
		o.on_iter_end(co, chosen_swap);
		return false;
	}
};