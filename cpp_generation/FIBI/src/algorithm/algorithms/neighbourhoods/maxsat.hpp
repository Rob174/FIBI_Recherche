#pragma once

#include <cmath>
#include "abstract.hpp"
#include "../../../data/solution_container/maxsat.hpp"
#include "../../../data/flips/abstract.hpp"
#include "../../../data/constants/maxsat.hpp"

#include "../../../utils.h"

class MAXSATNeighbourhoodExplorer : public NeighbourhoodExplorer<MAXSATSolutionContainer, MAXSATConfig, FlipMAXSAT> 
{
private:
	AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>&o;
	bool FI;
public:
	MAXSATNeighbourhoodExplorer(AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>& o, bool FI = false) : o(o), FI(FI) {};
	/** @brief Explore all possible flips and choose the best one
	(first clustering with improving cost if @tparam FI = true, best solution otherwise).
	Test to change the assignement of each variable to its opposite value}
	* * Expecting number of moves with n variables to be n
	*
	*/
	bool explore_flips(MAXSATSolutionContainer& co, const MAXSATConfig& cf, FlipMAXSAT& f) const
	{
		double delta = 0;
		MAXSATSwap chosen_swap;
		for (var_id_t i_elem = 0; i_elem < cf.num_choices(); i_elem++)
		{
			MAXSATSwap tmp_swap(i_elem);
			double delta_ij = f.test_flip(tmp_swap, co);
			o.on_test_end(co, delta_ij, tmp_swap);
			if (delta_ij > EPSILON && delta_ij > delta)
			{
				delta = delta_ij;
				chosen_swap.var_id = i_elem;
				o.on_glob_iter_end(co, delta, tmp_swap);
				if (FI)
				{
					f.flip(chosen_swap, delta, co);
					o.on_iter_end(co, chosen_swap);
					return false;
				}
			}
		}
		if (abs(delta) < EPSILON)
		{
			// No improvement found, stopping the loop
			o.on_iter_end(co, chosen_swap);
			return true;
		}
		// Update 
		f.flip(chosen_swap, delta, co);
		o.on_iter_end(co, chosen_swap);
		return false;
	}
};