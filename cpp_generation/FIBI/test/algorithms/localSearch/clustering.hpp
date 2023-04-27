#pragma once

#pragma once 

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include "../../../src/algorithm/factories.hpp"
#include "../../../src/data/solution_container/tsp.hpp"
#include "../../../src/utils.h"
#include "../../../src/data/get_data.hpp"


using namespace std;

class Clust1NeighbourhoodExplorLogger : public clustering_obs_t {
private:
	vector<ClusteringSwap> moves;
	ClusteringSwap chosen_move;
public:
	Clust1NeighbourhoodExplorLogger() : clustering_obs_t() {};

	void on_start(const ClusteringSolutionContainer<>& c, const optional<ClusteringSwap>& s) override {
		if (s.has_value()) {
			moves.push_back(s.value());
		}
	}
	void on_test_end(const ClusteringSolutionContainer<>& c, const optional<double>& delta, const optional<ClusteringSwap>& s) override {
		if (s.has_value()) {
			moves.push_back(s.value());
		}
	}
	void on_glob_iter_end(const ClusteringSolutionContainer<>& c, const optional<double>& delta, const optional<ClusteringSwap>& s) override {
	}
	void on_iter_end(const ClusteringSolutionContainer<>& c, const optional<ClusteringSwap>& s) override {
		if (s.has_value()) {
			chosen_move = s.value();
		}
		throw out_of_range("Stop the algorithm");
	};
	void on_end(const ClusteringSolutionContainer<>& c, const optional<ClusteringSwap>& s) override {
	};
	const vector<ClusteringSwap> get_moves() const {
		return moves;
	}
	const ClusteringSwap get_chosen_move() const {
		return chosen_move;
	}
};
/** @brief Test that the correct number of moves are explored in the 1-neighbourhood}
* * Assuming that we have n towns, k clusters the number of moves explored is: (k-1)n
* * See ClusteringNeighbourhoodExplorer::explore_flips doc for more details
* */
void test_LocalSearch_Clust_flips_explored() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_points = (uniform_int_distribution<> {10, 1000})(gen);
	const int num_clust = (uniform_int_distribution<> {2, 256})(gen);
	const int num_dims = (uniform_int_distribution<> {2, 256})(gen);

	// Limit the tests to 2D for now (only usage in the codebase)
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> points(uniform_points(num_points, num_dims, seed));
	unique_ptr<vector<int>> assignements(random_clust(num_clust, num_points, seed));
	map<string,long>* args = new map<string,long>{ {"NUM_POINTS", num_points}, { "NUM_DIM", num_dims }, { "NUM_CLUST",num_clust } };
	ClusteringConfig conf(args);
	ClusteringSolutionContainer<> c(*points, *assignements, conf);
	unique_ptr<Clust1NeighbourhoodExplorLogger> logger(new Clust1NeighbourhoodExplorLogger());
	vector<clustering_obs_t*> obs{ logger.get() };
	unique_ptr<typename clust_ls_t::ls_t> ls(getClusteringLocalSearch(obs));
	const int n = num_points, k = num_clust;
	const unsigned int expected_num_swaps = (k - 1) * n;
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
				+ to_string(expected_num_swaps)
				+ " because "
				+ to_string(num_points)
				+ " points and "
				+ to_string(num_clust)
				+ " clusters, but got "
				+ to_string(logger->get_moves().size())
				+ " moves: "
				+ moves
			);
		}
	}
	delete args;
}

/** @brief Test that the correct moves are explored with BI for the following instance}
	  INIT                  FI STOP                                   BI CHOICE


   Cost=141.67             Cost=129            Cost=115.17           Cost=91.83           Cost=168            Cost=173.5
			   [2]                   [2]                  [2]                   [2]                 [2]                   [2]
			  B                     B                    B                     A                   B                     B
[4]                   [4]                  [4]                   [4]                 [4]                   [4]
   A                     A                    A                     A    @              A                     B

	 @  [0]                   [0]                  [0]                   [0]                 [0]                   [0]
		A                     B                    A   $                 A                   A$                    A
												@                                           @                      $
		 $                     $


	B                     B                    A                     B                   B                     B
 [1]      B            [1]      B           [1]      B            [1]   $  B          [1]      A            [1]      B
		   [3]                   [3]                  [3]                   [3]                 [3]                   [3]

					   Point 0, A->B         Point 1, B->A        Point 2, B->A       Point 3, B->A         Point 4, A->B


 (Cost rounded to 2 decimals, positions rounded to units for ascii canvas)


						(0,0) (-4,-5)   (6,5)    (2,-6)    (-5,3)
				 ┌─────┬───────────────────────────────────────────┐
				 │     │  0       1        2        3        4     │
				 ├─────┼──────┬───────┬────────┬─────────┬─────────┤
			(0,0)│  0  │  0   │  6.4  │   7.8  │   6.3   │  5.8    │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
		  (-4,-5)│  1  │ 6.4  │   0   │  14.1  │   6.1   │  8.1    │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
			(6,5)│  2  │ 7.8  │ 14.1  │    0   │   11.7  │  11.2   │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
		   (2,-6)│  3  │ 6.3  │  6.1  │  11.7  │    0    │  11.4   │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
		   (-5,3)│  4  │ 5.8  │  8.1  │  11.2  │   11.4  │   0     │
				 │     │      │       │        │         │         │
				 └─────┴──────┴───────┴────────┴─────────┴─────────┘

# done with https://asciiflow.com
* */
void test_LocalSearch_Clust_flips_explored_detail_BI() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_points = 5;
	const int num_clust = 2;
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	const vector<double> points{
		0,0,	// [0]
		-4,-5,	// [1]
		6,5,	// [2]
		2,-6,	// [3]
		-5,3	// [4]
	};
	vector<int> assignements{ 0,1,1,1,0 };
	map<string,long>* args = new map<string,long>{ {"NUM_POINTS", num_points}, { "NUM_DIM", num_dims }, { "NUM_CLUST",num_clust } };
	ClusteringConfig conf(args);
	ClusteringSolutionContainer<> c(points, assignements, conf);
	unique_ptr<Clust1NeighbourhoodExplorLogger> logger(new Clust1NeighbourhoodExplorLogger());
	vector<clustering_obs_t*> obs{ logger.get() };
	unique_ptr<typename clust_ls_t::ls_t> ls(getClusteringLocalSearch(obs, false));
	try {
		ls->run(c, conf);
	}
	catch (std::out_of_range) {
		// Check the expected moves
		const vector<ClusteringSwap> moves = logger->get_moves();
		const vector<ClusteringSwap> exp_moves{
			ClusteringSwap(0,0,1),
			ClusteringSwap(1,1,0),
			ClusteringSwap(2,1,0),
			ClusteringSwap(3,1,0),
			ClusteringSwap(4,0,1)
		};
		for (int i = 0; i < exp_moves.size(); i++) {
			if (moves.at(i) != exp_moves.at(i)) {
				throw runtime_error("For move " + to_string(i) + " expecting " + exp_moves.at(i).str() + " but got " + moves.at(i).str());
			}
		}
		// And check the final choice
		const ClusteringSwap final_choice = logger->get_chosen_move();
		const ClusteringSwap exp_final_choice(2, 1, 0);
		if (final_choice != exp_final_choice) {
			throw runtime_error("For final choice expecting " + exp_final_choice.str() + " but got " + final_choice.str());
		}
	}
	delete args;
}

/** @brief Test that the correct moves are explored with FI for the following instance}
	  INIT                  FI STOP                                   BI CHOICE


   Cost=141.67             Cost=129            Cost=115.17           Cost=91.83           Cost=168            Cost=173.5
			   [2]                   [2]                  [2]                   [2]                 [2]                   [2]
			  B                     B                    B                     A                   B                     B
[4]                   [4]                  [4]                   [4]                 [4]                   [4]
   A                     A                    A                     A    @              A                     B

	 @  [0]                   [0]                  [0]                   [0]                 [0]                   [0]
		A                     B                    A   $                 A                   A$                    A
												@                                           @                      $
		 $                     $


	B                     B                    A                     B                   B                     B
 [1]      B            [1]      B           [1]      B            [1]   $  B          [1]      A            [1]      B
		   [3]                   [3]                  [3]                   [3]                 [3]                   [3]

					   Point 0, A->B         Point 1, B->A        Point 2, B->A       Point 3, B->A         Point 4, A->B


 (Cost rounded to 2 decimals, positions rounded to units for ascii canvas)


						(0,0) (-4,-5)   (6,5)    (2,-6)    (-5,3)
				 ┌─────┬───────────────────────────────────────────┐
				 │     │  0       1        2        3        4     │
				 ├─────┼──────┬───────┬────────┬─────────┬─────────┤
			(0,0)│  0  │  0   │  6.4  │   7.8  │   6.3   │  5.8    │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
		  (-4,-5)│  1  │ 6.4  │   0   │  14.1  │   6.1   │  8.1    │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
			(6,5)│  2  │ 7.8  │ 14.1  │    0   │   11.7  │  11.2   │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
		   (2,-6)│  3  │ 6.3  │  6.1  │  11.7  │    0    │  11.4   │
				 │     ├──────┼───────┼────────┼─────────┼─────────┤
				 │     │      │       │        │         │         │
		   (-5,3)│  4  │ 5.8  │  8.1  │  11.2  │   11.4  │   0     │
				 │     │      │       │        │         │         │
				 └─────┴──────┴───────┴────────┴─────────┴─────────┘

# done with https://asciiflow.com
* */
void test_LocalSearch_Clust_flips_explored_detail_FI() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_points = 5;
	const int num_clust = 2;
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	const vector<double> points{
		0,0,	// [0]
		-4,-5,	// [1]
		6,5,	// [2]
		2,-6,	// [3]
		-5,3	// [4]
	};
	vector<int> assignements{ 0,1,1,1,0 };
	map<string,long>* args = new map<string,long>{ {"NUM_POINTS", num_points}, { "NUM_DIM", num_dims }, { "NUM_CLUST",num_clust } };
	ClusteringConfig conf(args);
	ClusteringSolutionContainer<> c(points, assignements, conf);
	unique_ptr<Clust1NeighbourhoodExplorLogger> logger(new Clust1NeighbourhoodExplorLogger());
	vector<clustering_obs_t*> obs{ logger.get() };
	unique_ptr<typename clust_ls_t::ls_t> ls(getClusteringLocalSearch(obs, true));
	try {
		ls->run(c, conf);
	}
	catch (std::out_of_range) {
		// Check the expected moves
		const vector<ClusteringSwap> moves = logger->get_moves();
		const vector<ClusteringSwap> exp_moves{
			ClusteringSwap(0,0,1)
		};
		for (int i = 0; i < exp_moves.size(); i++) {
			if (moves.at(i) != exp_moves.at(i)) {
				throw runtime_error("For move " + to_string(i) + " expecting " + exp_moves.at(i).str() + " but got " + moves.at(i).str());
			}
		}
		// And check the final choice
		const ClusteringSwap final_choice = logger->get_chosen_move();
		const ClusteringSwap exp_final_choice(0, 0, 1);
		if (final_choice != exp_final_choice) {
			throw runtime_error("For final choice expecting " + exp_final_choice.str() + " but got " + final_choice.str());
		}
	}
	delete args;
}
