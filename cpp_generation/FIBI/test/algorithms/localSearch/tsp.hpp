#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include "../../../src/algorithm/factories.hpp"
#include "../../../src/data/solution_container/tsp.hpp"
#include "../../../src/utils.h"

using namespace std;

// class TSP1NeighbourhoodExplorLogger : public tsp_obs_t
// {
// private:
// 	vector<TSPSwap> moves;
// 	TSPSwap chosen_move;

// public:
// 	TSP1NeighbourhoodExplorLogger() : tsp_obs_t(){};

// 	void on_start(const TSPSolutionContainer &c, const optional<TSPSwap> &s) override
// 	{
// 		if (s.has_value())
// 		{
// 			moves.push_back(s.value());
// 		}
// 	}
// 	void on_test_end(const TSPSolutionContainer &c, const optional<double> &delta, const optional<TSPSwap> &s) override
// 	{
// 		if (s.has_value())
// 		{
// 			moves.push_back(s.value());
// 		}
// 	}
// 	void on_glob_iter_end(const TSPSolutionContainer &c, const optional<double> &delta, const optional<TSPSwap> &s) override
// 	{
// 	}
// 	void on_iter_end(const TSPSolutionContainer &c, const optional<TSPSwap> &s) override
// 	{
// 		if (s.has_value())
// 		{
// 			chosen_move = s.value();
// 		}
// 		throw out_of_range("Stop the algorithm");
// 	};
// 	void on_end(const TSPSolutionContainer &c, const optional<TSPSwap> &s) override{};
// 	const vector<TSPSwap> get_moves() const
// 	{
// 		return moves;
// 	}
// 	const TSPSwap get_chosen_move() const
// 	{
// 		return chosen_move;
// 	}
// };
// /** @brief Test that the correct number of moves are explored in the 1-neighbourhood}
// * * Assuming that we have n towns, there are n-1 edges
// * * As swaps are build by pairs of edges the number of unique swaps is given by (n-1)C2. Contiguous edges are excluded.
// * * See documentation of TSPNeighbourhoodExplorer::explore_flips to see the explantion of the formula
// * *
// 					⎛    n - 2⎞
// 		  (n - 3) ⋅ ⎜1 + ─────⎟
// 					⎝      2  ⎠
// * */
// void test_LocalSearch_TSP_flips_explored()
// {
// 	random_device rd;
// 	mt19937 gen(rd());
// 	uniform_int_distribution<> dis_ints(1, max_int);
// 	const int num_towns = 5;

// 	// Limit the tests to 2D for now (only usage in the codebase)
// 	const int num_dims = 2;
// 	const int seed = dis_ints(gen) - 1;
// 	unique_ptr<const vector<double>> towns(uniform_points(num_towns, num_dims, seed));
// 	DistanceMatrix m(num_towns, num_dims, *towns);
// 	vector<int> tour;
// 	for (int i = 0; i < num_towns; i++)
// 	{
// 		tour.push_back(i);
// 	}
// 	TSPSolutionContainer c(tour, m, TwoOpt(m));
// 	unique_ptr<TSP1NeighbourhoodExplorLogger> logger(new TSP1NeighbourhoodExplorLogger());
// 	vector<tsp_obs_t *> obs{logger.get()};
// 	unique_ptr<typename tsp_ls_t::ls_t> ls(getTSPLocalSearch(obs));
// 	map<string, long> *args = new map<string, long>{{"NUM_TOWNS", num_towns}, {"NUM_DIM", num_dims}};
// 	TSPConfig conf(args);
// 	// Compute 2 in n-1
// 	const int n = num_towns;
// 	const unsigned int expected_num_swaps = (unsigned int)((n - 3) * (1 + (double)(n - 2) / 2));
// 	try
// 	{
// 		ls->run(c, conf);
// 	}
// 	catch (std::out_of_range)
// 	{
// 		// Check that the correct number of moves were explored
// 		if (logger->get_moves().size() != expected_num_swaps)
// 		{
// 			string moves;
// 			for (auto &m : logger->get_moves())
// 			{
// 				moves += "(i=" + to_string(m.i) + ",j=" + to_string(m.j) + "), ";
// 			}
// 			throw runtime_error(
// 				"The number of moves explored is not correct: expected " + to_string(expected_num_swaps) + " because " + to_string(num_towns) + " towns but got " + to_string(logger->get_moves().size()) + " moves: \n" + moves);
// 		}
// 	}
// 	delete args;
// }

// /** @brief Test that the correct moves are explored with BI for the following instance}
// 	  INIT					 FI STOP                                  BI CHOICE

//    Cost=43.6            Cost=39.4               Cost=40.2             Cost=35.4         Cost=39.5            Cost=43.6
// 			   [2]                     [2]                 [2]                  [2]                  [2]                 [2]
// 			 *O                      *O                  *O         @@@@@@@@@@@O                 @@@O        @@@@@@@@@@@O
// [4]         ** *        [4]         **@     [4]         ** *     [4]@           *     [4]        @   *    [4]@        **
// ***O        *  *        ***O        * @        O        *  *        O           *     ***O       @   *    ***O        *
// *   **      *  *        *   **      * @        @**      *  *         **         *     *  @       @   *    *           *
// *    ***[0] *  *        *    ***[0] * @        @ ***[0] *  *          ***[0]    *     *  @    [0]@   *    *       [0] *
// *       O  * **         *       O@@*@@@        @    O  * **              O    **      *  @    O@@@ **     *       O@@@*
// *      **  * *          *          *           @    @  * *              **    *       *  @   **    *      *      **  @
// *     **  *  *          *        **            @    @ *  *             **     *       *  @  **     *      *     **  *@
// *    **  *  **          *      **              @    @*  **            **     **       *  @ **     **      *    **  * @
// *    *  *  **           *    **                @    @  **             *     **        *  @ *     **       *    *  *  @
// *   O***  **            *   O@@@@@@            @O***@ **             O     **         *  @@     **        *   O***   @
// *[1]      O             *[1]      O          [1]    @@O           [1] @@@@@O          *[1]      O         *[1]      O@
// ***********[3]          ***********[3]                 [3]                  [3]       ***********[3]      ***********[3]
// 							 0,2                  0,3                  1,3                 1,4                2,4


// 						(0,0) (-4,-5)   (6,5)    (2,-6)    (-5,3)
// 				 ┌─────┬───────────────────────────────────────────┐
// 				 │     │  0       1        2        3        4     │
// 				 ├─────┼──────┬───────┬────────┬─────────┬─────────┤
// 			(0,0)│  0  │  0   │  6.4  │   7.8  │   6.3   │  5.8    │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 		  (-4,-5)│  1  │ 6.4  │   0   │  14.1  │   6.1   │  8.1    │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 			(6,5)│  2  │ 7.8  │ 14.1  │    0   │   11.7  │  11.2   │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 		   (2,-6)│  3  │ 6.3  │  6.1  │  11.7  │    0    │  11.4   │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 		   (-5,3)│  4  │ 5.8  │  8.1  │  11.2  │   11.4  │   0     │
// 				 │     │      │       │        │         │         │
// 				 └─────┴──────┴───────┴────────┴─────────┴─────────┘

// # done with https://asciiflow.com
// * */
// void test_LocalSearch_TSP_flips_explored_detail_BI()
// {
// 	random_device rd;
// 	mt19937 gen(rd());
// 	uniform_int_distribution<> dis_ints(1, max_int);
// 	const int num_towns = 5;

// 	// Limit the tests to 2D for now (only usage in the codebase)
// 	const int num_dims = 2;
// 	const int seed = dis_ints(gen) - 1;
// 	const vector<double> towns{
// 		0, 0,	// [0]
// 		-4, -5, // [1]
// 		6, 5,	// [2]
// 		2, -6,	// [3]
// 		-5, 3	// [4]
// 	};
// 	DistanceMatrix m(num_towns, num_dims, towns);
// 	vector<int> tour;
// 	for (int i = 0; i < num_towns; i++)
// 	{
// 		tour.push_back(i);
// 	}
// 	TSPSolutionContainer c(tour, m, TwoOpt(m));
// 	unique_ptr<TSP1NeighbourhoodExplorLogger> logger(new TSP1NeighbourhoodExplorLogger());
// 	vector<tsp_obs_t *> obs{logger.get()};
// 	unique_ptr<typename tsp_ls_t::ls_t> ls(getTSPLocalSearch(obs));
// 	map<string, long> *args = new map<string, long>{{"NUM_TOWNS", num_towns}, {"NUM_DIM", num_dims}};
// 	TSPConfig conf(args);
// 	try
// 	{
// 		ls->run(c, conf);
// 	}
// 	catch (std::out_of_range)
// 	{
// 		// Check the expected moves
// 		const vector<TSPSwap> moves = logger->get_moves();
// 		const vector<TSPSwap> exp_moves{
// 			TSPSwap(0, 2),
// 			TSPSwap(0, 3),
// 			TSPSwap(1, 3),
// 			TSPSwap(1, 4),
// 			TSPSwap(2, 4)};
// 		for (int i = 0; i < exp_moves.size(); i++)
// 		{
// 			if (moves.at(i) != exp_moves.at(i))
// 			{
// 				throw runtime_error("For move " + to_string(i) + " expecting " + exp_moves.at(i).str() + " but got " + moves.at(i).str());
// 			}
// 		}
// 		// And check the final choice
// 		const TSPSwap final_choice = logger->get_chosen_move();
// 		const TSPSwap exp_final_choice(1, 3);
// 		if (final_choice != exp_final_choice)
// 		{
// 			throw runtime_error("For final choice expecting " + exp_final_choice.str() + " but got " + final_choice.str());
// 		}
// 	}
// 	delete args;
// }

// /** @brief Test that the correct moves are explored with FI for the following instance}
// 	  INIT					 FI STOP                                  BI CHOICE

//    Cost=43.6            Cost=39.4               Cost=40.2             Cost=35.4         Cost=39.5            Cost=43.6
// 			   [2]                     [2]                 [2]                  [2]                  [2]                 [2]
// 			 *O                      *O                  *O         @@@@@@@@@@@O                 @@@O        @@@@@@@@@@@O
// [4]         ** *        [4]         **@     [4]         ** *     [4]@           *     [4]        @   *    [4]@        **
// ***O        *  *        ***O        * @        O        *  *        O           *     ***O       @   *    ***O        *
// *   **      *  *        *   **      * @        @**      *  *         **         *     *  @       @   *    *           *
// *    ***[0] *  *        *    ***[0] * @        @ ***[0] *  *          ***[0]    *     *  @    [0]@   *    *       [0] *
// *       O  * **         *       O@@*@@@        @    O  * **              O    **      *  @    O@@@ **     *       O@@@*
// *      **  * *          *          *           @    @  * *              **    *       *  @   **    *      *      **  @
// *     **  *  *          *        **            @    @ *  *             **     *       *  @  **     *      *     **  *@
// *    **  *  **          *      **              @    @*  **            **     **       *  @ **     **      *    **  * @
// *    *  *  **           *    **                @    @  **             *     **        *  @ *     **       *    *  *  @
// *   O***  **            *   O@@@@@@            @O***@ **             O     **         *  @@     **        *   O***   @
// *[1]      O             *[1]      O          [1]    @@O           [1] @@@@@O          *[1]      O         *[1]      O@
// ***********[3]          ***********[3]                 [3]                  [3]       ***********[3]      ***********[3]
// 							 0,2                  0,3                  1,3                 1,4                2,4

// 						(0,0) (-4,-5)   (6,5)    (2,-6)    (-5,3)
// 				 ┌─────┬───────────────────────────────────────────┐
// 				 │     │  0       1        2        3        4     │
// 				 ├─────┼──────┬───────┬────────┬─────────┬─────────┤
// 			(0,0)│  0  │  0   │  6.4  │   7.8  │   6.3   │  5.8    │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 		  (-4,-5)│  1  │ 6.4  │   0   │  14.1  │   6.1   │  8.1    │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 			(6,5)│  2  │ 7.8  │ 14.1  │    0   │   11.7  │  11.2   │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 		   (2,-6)│  3  │ 6.3  │  6.1  │  11.7  │    0    │  11.4   │
// 				 │     ├──────┼───────┼────────┼─────────┼─────────┤
// 				 │     │      │       │        │         │         │
// 		   (-5,3)│  4  │ 5.8  │  8.1  │  11.2  │   11.4  │   0     │
// 				 │     │      │       │        │         │         │
// 				 └─────┴──────┴───────┴────────┴─────────┴─────────┘

// # done with https://asciiflow.com
// * */
// void test_LocalSearch_TSP_flips_explored_detail_FI()
// {
// 	random_device rd;
// 	mt19937 gen(rd());
// 	uniform_int_distribution<> dis_ints(1, max_int);
// 	const int num_towns = 5;

// 	// Limit the tests to 2D for now (only usage in the codebase)
// 	const int num_dims = 2;
// 	const int seed = dis_ints(gen) - 1;
// 	const vector<double> towns{
// 		0, 0,	// [0]
// 		-4, -5, // [1]
// 		6, 5,	// [2]
// 		2, -6,	// [3]
// 		-5, 3	// [4]
// 	};
// 	DistanceMatrix m(num_towns, num_dims, towns);
// 	vector<int> tour;
// 	for (int i = 0; i < num_towns; i++)
// 	{
// 		tour.push_back(i);
// 	}
// 	TSPSolutionContainer c(tour, m, TwoOpt(m));
// 	unique_ptr<TSP1NeighbourhoodExplorLogger> logger(new TSP1NeighbourhoodExplorLogger());
// 	vector<tsp_obs_t *> obs{logger.get()};
// 	unique_ptr<typename tsp_ls_t::ls_t> ls(getTSPLocalSearch(obs, true));
// 	map<string, long> *args = new map<string, long>{{"NUM_TOWNS", num_towns}, {"NUM_DIM", num_dims}};
// 	TSPConfig conf(args);
// 	try
// 	{
// 		ls->run(c, conf);
// 	}
// 	catch (std::out_of_range)
// 	{
// 		// Check the expected moves
// 		const vector<TSPSwap> moves = logger->get_moves();
// 		const vector<TSPSwap> exp_moves{
// 			TSPSwap(0, 2)};
// 		for (int i = 0; i < exp_moves.size(); i++)
// 		{
// 			if (moves.at(i) != exp_moves.at(i))
// 			{
// 				throw runtime_error("For move " + to_string(i) + " expecting " + exp_moves.at(i).str() + " but got " + moves.at(i).str());
// 			}
// 		}
// 		// And check the final choice
// 		const TSPSwap final_choice = logger->get_chosen_move();
// 		const TSPSwap exp_final_choice(0, 2);
// 		if (final_choice != exp_final_choice)
// 		{
// 			throw runtime_error("For final choice expecting " + exp_final_choice.str() + " but got " + final_choice.str());
// 		}
// 	}
// 	delete args;
// }