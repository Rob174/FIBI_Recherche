#pragma once

#include <cmath>
#include "abstract.hpp"
#include "../../../data/solution_container/tsp.hpp"
#include "../../../data/constants/maxsat.hpp"

template <bool FI = false, const double EPSILON = 1e-5>
class MAXSATNeighbourhoodExplorer
{
private:
	AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>* o;
public:
	MAXSATNeighbourhoodExplorer(AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>* o) : o(o) {};
	bool explore_flips(MAXSATSolutionContainer& co, const MAXSATConfig& cf) const
	{
		double delta = 0;
		MAXSATSwap chosen_swap;
		for (var_id_t i_elem = 0; i_elem < cf.num_choices(); i_elem++)
		{
			MAXSATSwap tmp_swap(i_elem);
			double delta_ij = co.test_flip(tmp_swap);
			if (delta_ij > EPSILON && delta_ij > delta)
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