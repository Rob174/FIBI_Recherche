#pragma once

#include <vector>
#include "./abstract.hpp"
#include "distance_matrix.hpp"
#include "../../algorithm/swaps/tsp.hpp"

using namespace std;

template <bool detailDelta = false, bool debugCost = false>
class TSPSolutionContainer : public AbstractSolutionContainer<TSPSwap>
{
public:
	vector<town_in_tour_id_t>& tour;
	const DistanceMatrix& dist;

public:
	TSPSolutionContainer(vector<town_in_tour_id_t >& tour, const DistanceMatrix& m) : dist(m), AbstractSolutionContainer<TSPSwap>(), tour(tour) {
	};
	TSPSolutionContainer(const TSPSolutionContainer& other) {
		tour = other.tour;
		dist = other.dist;
	}
	// Flips
	quality_delta_t test_flip(const TSPSwap& test_swap) const {
		const town_id_t pi = tour.at(test_swap.i),
			pj = tour.at(test_swap.j),
			pi_1 = tour.at(cycle_id(test_swap.i + 1)),
			pj_1 = tour.at(cycle_id(test_swap.j + 1));
		if constexpr (detailDelta) {
			const double new1 = dist.get(pi, pj),
				new2 = dist.get(pi_1, pj_1),
				past1 = dist.get(pi, pi_1),
				past2 = dist.get(pj, pj_1);
			return new1 + new2 - past1 - past2;
		}
		else {
			return dist.get(pi, pj) + dist.get(pi_1, pj_1) - dist.get(pi, pi_1) - dist.get(pj, pj_1);
		}
	};
	void flip(const TSPSwap& swap, const quality_delta_t delta) {
		if constexpr (debugCost) {
			const double delta_cmpt = test_flip(swap);
			const double past_cost = compute_quality_metric();
		}
		const int num_towns = tour.size();
		int buffer;
		if (swap.i >= tour.size() || swap.i < 0)
		{
			cout << "Wrong i with :" << swap.i << endl;
			exit(1);
		}
		if (swap.j >= tour.size() || swap.j < 0)
		{
			cout << "Wrong j with :" << swap.j << endl;
			exit(1);
		}
		if (swap.i == swap.j)
		{
			cout << "Wrong ids with :i:" << swap.i << ";j:" << swap.j << endl;
			exit(1);
		}
		const int length = swap.j > swap.i ? abs(swap.j - swap.i) : tour.size() - swap.i + swap.j;
		for (int l = 1; l <= length / 2; l++)
		{
			buffer = tour.at(cycle_id((swap.i + l)));
			tour[cycle_id((swap.i + l))] = tour.at(cycle_id(swap.j - l + 1));
			tour[cycle_id(swap.j - l + 1)] = buffer;
		}
		if constexpr (debugCost) {
			const double real_cost = compute_quality_metric();
			update_with_delta_cost(delta);
			if (abs(real_cost - get_quality_metric()) > EPSILON)
			{
				throw exception("Different costs");
			}
		}
		else {
			update_with_delta_cost(delta);
		}
		last_choice = swap;
	}
	// Objective
	double compute_quality_metric() {
		double cost = 0;
		for (town_in_tour_id_t i = 0; i < tour.size(); i++)
		{
			const town_id_t p = cycle_id(i), pnext = cycle_id(i + 1);
			cost += dist.get(tour.at(p), tour.at(pnext));
		}
		return cost;
	}
	// Getters
	[[nodiscard]] const int* get_tour() { return tour.data(); }
	const std::vector<double>& get_towns_pos() { return dist.get_town_pos(); }
	// Utility
	town_in_tour_id_t cycle_id(const town_in_tour_id_t i) const
	{
		const int num_towns = tour.size();
		town_in_tour_id_t a = i < 0 ? num_towns + i : i;
		return a % num_towns;
	}
	void print() {

		cout << "----------------------------------------" << endl;
		cout << "\tTour: " << endl;
		for (int i = 0; i < tour.size(); i++)
		{
			cout << tour[i] << ",";
		}
		cout << endl;
		cout << "\tCost: " << get_quality_metric() << endl;
		cout << "----------------------------------------" << endl;
	}
};