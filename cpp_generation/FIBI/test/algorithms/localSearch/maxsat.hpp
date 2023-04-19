#pragma once

#pragma once 

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <optional>
#include "../../../src/algorithm/factories.hpp"
#include "../../../src/data/solution_container/maxsat.hpp"
#include "../../../src/utils.h"
#include "../../../src/data/get_data.hpp"


using namespace std;

class MAXSAT1NeighbourhoodExplorLogger : public maxsat_obs_t {
private:
	vector<MAXSATSwap> moves;
	MAXSATSwap chosen_move;
public:
	MAXSAT1NeighbourhoodExplorLogger() : maxsat_obs_t() {};

	void on_start(const MAXSATSolutionContainer& c, const optional<MAXSATSwap>& s) override {
		if (s.has_value()) {
			moves.push_back(s.value());
		}
	}
	void on_test_end(const MAXSATSolutionContainer& c, const optional<double>& delta, const optional<MAXSATSwap>& s) override {
		if (s.has_value()) {
			// push a copy of the solution
			moves.push_back(s.value());
		}
	}
	void on_glob_iter_end(const MAXSATSolutionContainer& c, const optional<double>& delta, const optional<MAXSATSwap>& s) override {
	}
	void on_iter_end(const MAXSATSolutionContainer& c, const optional<MAXSATSwap>& s) override {
		if (s.has_value()) {
			chosen_move = s.value();
		}
		throw out_of_range("Stop the algorithm");
	};
	void on_end(const MAXSATSolutionContainer& c, const optional<MAXSATSwap>& s) override {
	};
	const vector<MAXSATSwap> get_moves() const {
		return moves;
	}
	const MAXSATSwap get_chosen_move() const {
		return chosen_move;
	}
};
/** @brief Test that the correct number of moves are explored in the 1-neighbourhood}
* * Assuming that we have n variables, the number of moves explored is: n
* * See MAXSATNeighbourhoodExplorer::explore_flips doc for more details
* */
void test_LocalSearch_MAXSAT_flips_explored() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_variables = (uniform_int_distribution<> {1000, 5000})(gen);
	const int num_clauses = (uniform_int_distribution<> {1000, 5000})(gen);

	// Limit the tests to 2D for now (only usage in the codebase)
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<clause_t>> clause(random_clauses(num_variables, num_clauses, seed));
	unique_ptr<vector<bool>> assignements(random_assignements(num_variables, seed));
	vector<double> weights(num_clauses, 1.0);
	MAXSATConfig conf(map<string, int>{ {"NUM_VARIABLES", num_variables}, { "NUM_CLAUSES", num_clauses }});
	MAXSATSolutionContainer c(weights, *assignements, *clause);
	unique_ptr<MAXSAT1NeighbourhoodExplorLogger> logger(new MAXSAT1NeighbourhoodExplorLogger());
	vector<maxsat_obs_t*> obs{ logger.get() };
	unique_ptr<typename maxsat_ls_t::ls_t> ls(getMAXSATLocalSearch(obs));
	const unsigned int expected_num_swaps = num_variables;
	try {
		ls->run(c, conf);
	}
	catch (std::out_of_range) {
		// Check that the correct number of moves were explored
		if (logger->get_moves().size() != expected_num_swaps) {
			string moves;
			for (auto& m : logger->get_moves()) {
				moves += m.str() + ", ";
			}
			throw runtime_error(
				"The number of moves explored is not correct: expected "
				+ to_string(expected_num_swaps) + " but got " + to_string(logger->get_moves().size())
				+ " moves: " + moves

			);
		}
	}
}
/**
* Maximize: (v0 V NOT v1 V v2) AND (v1 V NOT v3 V v4) AND (NOT v0 V v3) AND (NOT v2 V NOT v3) AND (v0 V NOT v4)
* With weights constant to 1
* Subject to: v0, v1, v2, v3, v4 in {false, true}

* Initial: v0=true	v1=false v2=true	v3=true v4=false --> quality=3
* Swap v0: v0=false v1=false v2=true	v3=true v4=false --> quality=3
* Swap v1: v0=true	v1=true	 v2=true	v3=true v4=false --> quality=3
* Swap v2: v0=true	v1=false v2=false	v3=true v4=false --> quality=3
* Swap v3: v0=true	v1=false v2=true	v3=false v4=false --> quality=3
* Swap v4: v0=true	v1=false v2=true	v3=true v4=true --> quality=3
**/
void test_LocalSearch_MAXSAT_flips_explored_detail_BI() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_variables = 5;
	const int num_clauses = 5;
	const int seed = dis_ints(gen) - 1;
	const vector<clause_t> clauses{
		{{0,false}, {1,true}, {2,false}},
		{{1,false}, {3,true}, {4,false}},
		{{0,true}, {3,false}},
		{{2,true}, {3,true}},
		{{0,false}, {4,true}}
	};
	vector<bool> assignements{ true, false, true, true, false };
	vector<double> weights(num_clauses, 1.0);
	MAXSATConfig conf(map<string, int>{ {"NUM_VARIABLES", num_variables}, { "NUM_CLAUSES", num_clauses }});
	MAXSATSolutionContainer c(weights, assignements, clauses);
	unique_ptr<MAXSAT1NeighbourhoodExplorLogger> logger(new MAXSAT1NeighbourhoodExplorLogger());
	vector<maxsat_obs_t*> obs{ logger.get() };
	unique_ptr<typename maxsat_ls_t::ls_t> ls(getMAXSATLocalSearch(obs));
	try {
		ls->run(c, conf);
	}
	catch (std::out_of_range) {
		// Check the expected moves
		const vector<MAXSATSwap> moves = logger->get_moves();
		const vector<MAXSATSwap> exp_moves{
			MAXSATSwap(0),
			MAXSATSwap(1),
			MAXSATSwap(2),
			MAXSATSwap(3),
			MAXSATSwap(4)
		};
		for (int i = 0; i < exp_moves.size(); i++) {
			if (moves.at(i) != exp_moves.at(i)) {
				throw runtime_error("For move " + to_string(i) + " expecting " + exp_moves.at(i).str() + " but got " + moves.at(i).str());
			}
		}
		// And check the final choice
		const MAXSATSwap final_choice = logger->get_chosen_move();
		const MAXSATSwap exp_final_choice(-1);
		if (final_choice != exp_final_choice) {
			throw runtime_error("For final choice expecting " + exp_final_choice.str() + " but got " + final_choice.str());
		}
	}
}

/**
* Maximize: (v0 V NOT v1 V v2) AND (v1 V NOT v3 V v4) AND (NOT v0 V v3) AND (NOT v2 V NOT v3) AND (v0 V NOT v4)
* With weights constant to 1
* Subject to: v0, v1, v2, v3, v4 in {false, true}

* Initial: v0=true	v1=false v2=true	v3=true v4=false --> quality=3
* Swap v0: v0=false v1=false v2=true	v3=true v4=false --> quality=3
* Swap v1: v0=true	v1=true	 v2=true	v3=true v4=false --> quality=3
* Swap v2: v0=true	v1=false v2=false	v3=true v4=false --> quality=3
* Swap v3: v0=true	v1=false v2=true	v3=false v4=false --> quality=3
* Swap v4: v0=true	v1=false v2=true	v3=true v4=true --> quality=3
**/
void test_LocalSearch_MAXSAT_flips_explored_detail_FI() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_variables = 5;
	const int num_clauses = 5;
	const int seed = dis_ints(gen) - 1;
	const vector<clause_t> clauses{
		{{0,false}, {1,true}, {2,false}},
		{{1,false}, {3,true}, {4,false}},
		{{0,true}, {3,false}},
		{{2,true}, {3,true}},
		{{0,false}, {4,true}}
	};
	vector<bool> assignements{ true, false, true, true, false };
	vector<double> weights(num_clauses, 1.0);
	MAXSATConfig conf(map<string, int>{ {"NUM_VARIABLES", num_variables}, { "NUM_CLAUSES", num_clauses }});
	MAXSATSolutionContainer c(weights, assignements, clauses);
	unique_ptr<MAXSAT1NeighbourhoodExplorLogger> logger(new MAXSAT1NeighbourhoodExplorLogger());
	vector<maxsat_obs_t*> obs{ logger.get() };
	unique_ptr<typename maxsat_ls_t::ls_t> ls(getMAXSATLocalSearch(obs, true));
	try {
		ls->run(c, conf);
	}
	catch (std::out_of_range) {
		// Check the expected moves
		const vector<MAXSATSwap> moves = logger->get_moves();
		const vector<MAXSATSwap> exp_moves{
			MAXSATSwap(0),
			MAXSATSwap(1),
			MAXSATSwap(2),
			MAXSATSwap(3),
			MAXSATSwap(4)
		};
		for (int i = 0; i < exp_moves.size(); i++) {
			if (moves.at(i) != exp_moves.at(i)) {
				throw runtime_error("For move " + to_string(i) + " expecting " + exp_moves.at(i).str() + " but got " + moves.at(i).str());
			}
		}
		// And check the final choice
		const MAXSATSwap final_choice = logger->get_chosen_move();
		const MAXSATSwap exp_final_choice(-1);
		if (final_choice != exp_final_choice) {
			throw runtime_error("For final choice expecting " + exp_final_choice.str() + " but got " + final_choice.str());
		}
	}
}