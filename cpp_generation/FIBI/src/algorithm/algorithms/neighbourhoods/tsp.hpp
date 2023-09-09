#pragma once

#include <cmath>
#include "abstract.hpp"
#include "../../../data/solution_container/tsp.hpp"
#include "../../../data/flips/tsp.hpp"
#include "../../../data/constants/tsp.hpp"

#include "../../../utils.h"
#include "../../../data/flips/abstract.hpp"

class TwoOptNeighbourhood : public NeighbourhoodExplorer<TSPSolutionContainer, TSPConfig, FlipTSPTwoOpt> 
{
protected:
	AlgorithmObservable<TSPTwoOptSwap, TSPSolutionContainer> &o;
	bool FI;

public:
	TwoOptNeighbourhood(AlgorithmObservable<TSPTwoOptSwap, TSPSolutionContainer> &o, bool FI = false) : o(o), FI(FI){};

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
	bool explore_flips(TSPSolutionContainer &co, const TSPConfig &cf, FlipTSPTwoOpt& f) const
	{
		double delta = 0;
		TSPTwoOptSwap chosen_swap;
		for (town_in_tour_id_t i = 0; i <= cf.num_choices() - 3; i++)
		{
			int n_prime = i == 0 ? cf.num_choices() - 2 : cf.num_choices() - 1;
			for (town_in_tour_id_t j = i + 2; j <= n_prime; j++)
			{
				TSPTwoOptSwap tmp_swap(cycle_id(i, co.tour), cycle_id(j, co.tour));
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
		// Update
		f.flip(chosen_swap, delta,co);
		o.on_iter_end(co, chosen_swap);
		return false;
	}
};

class ThreeOptNeighbourhood : public NeighbourhoodExplorer<TSPSolutionContainer, TSPConfig, FlipTSPThreeOpt<>>
{
private:
	AlgorithmObservable<TSPThreeOptSwap, TSPSolutionContainer> &o;
	bool FI;

public:
	ThreeOptNeighbourhood(AlgorithmObservable<TSPThreeOptSwap, TSPSolutionContainer> &o, bool FI = false) : FI(FI), o(o) {};

	/** @brief {Explore all possible flips and choose the best one (first clustering with improving cost if @tparam FI = true, best solution otherwise).}
	 * * Important note: all moves are not equal to all combinations of edges:
	 * * Applying the 3-opt heuristic
	 */
	bool explore_flips(TSPSolutionContainer &co, const TSPConfig &cf, FlipTSPThreeOpt<>& f) const
	{
		double delta = 0;
		TSPThreeOptSwap chosen_swap;

		for (town_in_tour_id_t i = 0; i < cf.num_choices() - 4; i++)
		{
			for (town_in_tour_id_t j = i + 2; j < cf.num_choices() - 4; j++)
			{
				for (town_in_tour_id_t k = j + 2; k < cf.num_choices() - 1; k++)
				{
					// Perform the 3-opt move on the tour
					TSPThreeOptSwap tmp_swap(cycle_id(i, co.tour), cycle_id(j, co.tour), cycle_id(k, co.tour));
					double delta_ijk = f.test_flip(tmp_swap,co);

					// If the move improves the solution and is the best so far, choose it
					if (delta_ijk < -EPSILON && delta_ijk < delta)
					{
						delta = delta_ijk;
						chosen_swap = tmp_swap;
						o.on_glob_iter_end(co, delta, chosen_swap);

						if (FI)
						{
							f.flip(chosen_swap, delta, co);
							o.on_iter_end(co, chosen_swap);
							return false;
						}
					}
				}
			}
		}

		if (abs(delta) < EPSILON)
		{
			o.on_iter_end(co, chosen_swap);
			return true;
		}

		// Update tour with the chosen swap and apply the delta improvement
		f.flip(chosen_swap, delta, co);
		o.on_iter_end(co, chosen_swap);
		return false;
	}
};

class InsertionNeighbourhood : public NeighbourhoodExplorer<TSPSolutionContainer, TSPConfig, FlipTSPInsertion>
{
private:
	AlgorithmObservable<TSPInsertionSwap, TSPSolutionContainer> &o;
	bool FI;

public:
	InsertionNeighbourhood(AlgorithmObservable<TSPInsertionSwap, TSPSolutionContainer> &o, bool FI = false) : o(o), FI(FI) {};

	/** @brief {Explore all possible flips and choose the best one (first clustering with improving cost if @tparam FI = true, best solution otherwise).}
	 * * Important note: all moves are not equal to all combinations of edges:
	 * * Applying the 3-opt heuristic
	 */
	bool explore_flips(TSPSolutionContainer &co, const TSPConfig &cf, FlipTSPInsertion& f) const
	{
		double delta = 0;
		TSPInsertionSwap chosen_swap;

		for (town_in_tour_id_t i = 0; i < cf.num_choices(); i++)
		{
			for (int j = 0; i < cf.num_choices() - 1; i++)
			{
				const int dest_pos = (i + j) % cf.num_choices();
				// Perform the 3-opt move on the tour
				TSPInsertionSwap tmp_swap(i, dest_pos);
				double delta_ijk = f.test_flip(tmp_swap, co);

				// If the move improves the solution and is the best so far, choose it
				if (delta_ijk < -EPSILON && delta_ijk < delta)
				{
					delta = delta_ijk;
					chosen_swap = tmp_swap;
					o.on_glob_iter_end(co, delta, chosen_swap);

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

		// Update tour with the chosen swap and apply the delta improvement
		f.flip(chosen_swap, delta, co);
		o.on_iter_end(co, chosen_swap);
		return false;
	}
};