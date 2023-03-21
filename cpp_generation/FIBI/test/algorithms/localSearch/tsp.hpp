#pragma once 

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include "../../../src/algorithm/factories.hpp"
#include "../../../src/data/solution_container/tsp.hpp"
#include "../../../src/utils.h"


using namespace std;

class TSP1NeighbourhoodExplorLogger : public tsp_obs_t {
private:
	vector<TSPSwap> moves;
public:
	TSP1NeighbourhoodExplorLogger() : tsp_obs_t() {};

	void on_start(const TSPSolutionContainer<>& c, const optional<TSPSwap>& s) override {
		if (s.has_value()) {
			moves.push_back(s.value());
		}
	}
	void on_test_end(const TSPSolutionContainer<>& c, const optional<double>& delta, const optional<TSPSwap>& s) override {
		if (s.has_value()) {
			moves.push_back(s.value());
		}
	}
	void on_glob_iter_end(const TSPSolutionContainer<>& c, const optional<double>& delta, const optional<TSPSwap>& s) override {
	}
	void on_iter_end(const TSPSolutionContainer<>& c, const optional<TSPSwap>& s) override {
		// throw exception stop the algorithm
		throw out_of_range("Stop the algorithm");
	};
	void on_end(const TSPSolutionContainer<>& c, const optional<TSPSwap>& s) override {
	};
	const vector<TSPSwap> get_moves() {
		return moves;
	}
};
/** Test that the correct number of moves are explored in the 1-neighbourhood
* * Assuming that we have n towns, there are n-1 edges
* * As swaps are build by pairs of edges the number of unique swaps is given by (n-1)C2. Contiguous edges are excluded.
* * See documentation of TSPNeighbourhoodExplorer::explore_flips to see the explantion of the formula
* *
					⎛    n - 2⎞
		  (n - 3) ⋅ ⎜1 + ─────⎟
					⎝      2  ⎠
* */
void test_LocalSearch_flips_explored() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_towns = dis_ints(gen);

	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> towns(uniform_points(num_towns, num_dims, seed));
	DistanceMatrix m(num_towns, num_dims, *towns);
	vector<int> tour;
	for (int i = 0; i < num_towns; i++) {
		tour.push_back(i);
	}
	TSPSolutionContainer<> c(tour, m);
	unique_ptr<TSP1NeighbourhoodExplorLogger> logger(new TSP1NeighbourhoodExplorLogger());
	vector<tsp_obs_t*> obs{ logger.get() };
	unique_ptr<tsp_ls_t> ls(getTSPLocalSearch(obs));
	TSPConfig conf(map<string, int>{ {"NUM_TOWNS", num_towns}, { "NUM_DIM", num_dims }});
	// Compute 2 in n-1
	const int n = num_towns;
	const unsigned int expected_num_swaps = (unsigned int)((n - 3) * (1 + (double)(n - 2) / 2));
	try {
		ls->run(c, conf);
	}
	catch (std::out_of_range) {
		// Check that the correct number of moves were explored
		if (logger->get_moves().size() != expected_num_swaps) {
			string moves;
			for (auto& m : logger->get_moves()) {
				moves += "(i=" + to_string(m.i) + ",j=" + to_string(m.j) + "), ";
			}
			throw runtime_error(
				"The number of moves explored is not correct: expected "
				+ to_string(expected_num_swaps)
				+ " because "
				+ to_string(num_towns)
				+ " towns but got "
				+ to_string(logger->get_moves().size())
				+ " moves: \n"
				+ moves
			);
		}
	}
}