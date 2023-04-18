#pragma once

#include <cmath>
#include "abstract.hpp"
#include "../../../data/solution_container/tsp.hpp"
#include "../../../data/constants/tsp.hpp"

#include "../../../utils.h"

class TSPNeighbourhoodExplorer
{
private:
	AlgorithmObservable<TSPSwap, TSPSolutionContainer<>>* o;
	bool FI;
public:
	TSPNeighbourhoodExplorer(AlgorithmObservable<TSPSwap, TSPSolutionContainer<>>* o, bool FI = false) : o(o), FI(FI) {

	};

	~TSPNeighbourhoodExplorer()
	{
		delete o;
	}
	/** @brief {Explore all possible flips and choose the best one (first clustering with improving cost if @tparam FI = true, best solution otherwise).}
	* * Important note: all moves are not equal to all combinations of edges:
	* * Applying the 2-opt heuristic to two contiguous edges makes no sense, that is why these moves are excluded
	* * Expecting number of moves
			⎛n - 3            ⎞
			⎜_____            ⎟
			⎜╲       n - 1    ⎟   n - 2
			⎜ ╲       ___     ⎟    ___
			⎜  ╲      ╲       ⎟    ╲                            (n - 3) ⋅ (n - 2)             (n - 2) ⋅ (n - 3)                       ⎛    n - 2⎞
			⎜  ╱      ╱      1⎟ +  ╱    1 = (n - 2) ⋅ (n - 3) - ───────────────── + (n - 3) = ───────────────── + (n - 3) = (n - 3) ⋅ ⎜1 + ─────⎟
			⎜ ╱       ‾‾‾     ⎟    ‾‾‾                                  2                             2                               ⎝      2  ⎠
			⎜╱     j = i + 2  ⎟   j = 2
			⎜‾‾‾‾‾            ⎟
			⎝i = 1            ⎠
			---------------		--------
			Case i > 0			Case i = 0
	# done with https://arthursonzogni.com/Diagon/#Math
	*
	*/
	bool explore_flips(TSPSolutionContainer<>& co, const TSPConfig& cf) const
	{
		double delta = 0;
		TSPSwap chosen_swap;
		for (town_in_tour_id_t i = 0; i <= cf.num_choices() - 3; i++)
		{
			int n_prime = i == 0 ? cf.num_choices() - 2 : cf.num_choices() - 1;
			for (town_in_tour_id_t j = i + 2; j <= n_prime; j++)
			{
				TSPSwap tmp_swap(co.cycle_id(i), co.cycle_id(j));
				double delta_ij = co.test_flip(tmp_swap);
				o->on_test_end(co, delta_ij, tmp_swap);
				if (delta_ij < -EPSILON && delta_ij < delta)
				{
					delta = delta_ij;
					chosen_swap = tmp_swap;
					o->on_glob_iter_end(co, delta, tmp_swap);
					if (FI)
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
		// Update 
		co.flip(chosen_swap, delta);
		o->on_iter_end(co, chosen_swap);
		return false;
	}
};