#pragma once
#include "./tsp.hpp"
#include "./clustering.hpp"
#include "./maxsat.hpp"
#include "../solution_container/abstract.hpp"
#include "../solution_container/tsp.hpp"
#include "../solution_container/clustering.hpp"
#include "../solution_container/maxsat.hpp"
#include "../../algorithm/swaps/tsp.hpp"
#include "../../algorithm/swaps/clustering.hpp"
#include "../../algorithm/swaps/maxsat.hpp"

template <typename Swap_T, typename Container_T>
class Flip
{
public:
	Swap_T last_choice;
	virtual quality_delta_t test_flip(Swap_T &test_swap, const Container_T &c) const = 0;
	virtual void flip(const Swap_T &swap, const quality_delta_t delta, Container_T &c) = 0;
};

class FlipTSPTwoOpt : public Flip<TSPTwoOptSwap, TSPSolutionContainer>
{
	public:
	quality_delta_t test_flip(TSPTwoOptSwap &test_swap, const TSPSolutionContainer &c) const override
	{
		quality_delta_t delta = fn_test_flip1(
			test_swap,
			c.tour,
			c.dist);
		return delta;
	}
	void flip(const TSPTwoOptSwap &swap, const quality_delta_t delta, TSPSolutionContainer &c) override
	{
		fn_flip(swap, c.tour);
		c.update_with_delta_cost(delta);
		c.last_choice = swap;
	}
};

class FlipTSPThreeOpt : public Flip<TSPThreeOptSwap, TSPSolutionContainer>
{public:
	quality_delta_t test_flip(TSPThreeOptSwap &test_swap, const TSPSolutionContainer &c) const override
	{
		quality_delta_t delta = fn_test_flip(
			test_swap,
			c.tour,
			c.dist);
		return delta;
	}
	void flip(const TSPThreeOptSwap &swap, const quality_delta_t delta, TSPSolutionContainer &c) override
	{
		fn_flip(swap, c.tour);
		c.update_with_delta_cost(delta);
		c.last_choice = swap;
	}
};

class FlipTSPInsertion : public Flip<TSPInsertionSwap, TSPSolutionContainer>
{public:
	quality_delta_t test_flip(TSPInsertionSwap &test_swap, const TSPSolutionContainer &c) const override
	{
		quality_delta_t delta = fn_test_flip(
			test_swap,
			c.tour,
			c.dist);
		return delta;
	}
	void flip(const TSPInsertionSwap &swap, const quality_delta_t delta, TSPSolutionContainer &c) override
	{
		fn_flip(swap, c.tour);
		c.update_with_delta_cost(delta);
		last_choice = swap;
	}
};

class FlipClustering :  public Flip<ClusteringSwap, ClusteringSolutionContainer>
{public:
	quality_delta_t test_flip(ClusteringSwap &test_swap, const ClusteringSolutionContainer &c) const override
	{
		quality_delta_t delta = fn_test_flip(test_swap, c.n_p_p_c, c.p_c, c.c_c, c.conf.NUM_DIM.get());
		return delta;
	}
	void  flip(const ClusteringSwap &swap, const quality_delta_t delta, ClusteringSolutionContainer &c) override
	{
		fn_flip(swap, c.n_p_p_c, c.p_c, c.c_a, c.c_c, c.conf.NUM_DIM.get());
		c.update_with_delta_cost(delta);
		c.last_choice = swap;
	}
};

class FlipMAXSAT : public Flip<MAXSATSwap, MAXSATSolutionContainer>
{public:
	quality_delta_t test_flip(MAXSATSwap &test_swap, const MAXSATSolutionContainer &c) const override
	{
		quality_delta_t delta = fn_test_flip(test_swap, c.var_in_clauses, c.clauses, c.variables_val, c.weights, c.sat_clauses);
		return delta;
	}
	void  flip(const MAXSATSwap &swap, const quality_delta_t delta, MAXSATSolutionContainer &c) override
	{
		fn_flip(swap, c.var_in_clauses, c.clauses, c.variables_val, c.weights, c.sat_clauses);
		c.update_with_delta_cost(delta);
		c.last_choice = swap;
	}
};
