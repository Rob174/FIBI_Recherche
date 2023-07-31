#pragma once

#include <vector>
#include "./abstract.hpp"
#include "distance_matrix.hpp"
#include "../../algorithm/swaps/tsp.hpp"

using namespace std;

town_in_tour_id_t cycle_id(const town_in_tour_id_t i, vector<town_in_tour_id_t> &tour)
{
	const int num_towns = tour.size();
	town_in_tour_id_t a = i < 0 ? num_towns + i : i;
	return a % num_towns;
}

double tsp_compute_quality_metric(vector<town_in_tour_id_t> &tour, const DistanceMatrix &dist)
{
	double cost = 0;
	for (town_in_tour_id_t i = 0; i < tour.size(); i++)
	{
		const town_id_t p = cycle_id(i, tour), pnext = cycle_id(i + 1, tour);
		cost += dist.get(tour.at(p), tour.at(pnext));
	}
	return cost;
}

class OptFlip
{
public:
	DistanceMatrix &dist;
	OptFlip(DistanceMatrix &dist) : dist(dist){};
	virtual quality_delta_t test_flip(const TSPSwap &test_swap, vector<town_in_tour_id_t> &tour) const = 0;
	virtual void flip(const TSPSwap &swap, const quality_delta_t delta, vector<town_in_tour_id_t> &tour) const = 0;
	double compute_quality_metric(vector<town_in_tour_id_t> &tour) const
	{
		return tsp_compute_quality_metric(tour, dist);
	}
};

template <bool detailDelta = false, bool debugCost = false>
class TwoOptFlip : public OptFlip
{
public:
	TwoOptFlip(DistanceMatrix &dist) : OptFlip(dist){};
	quality_delta_t test_flip(const TSPSwap &test_swap, vector<town_in_tour_id_t> &tour) const
	{
		const town_id_t pi = tour.at(test_swap.i),
						pj = tour.at(test_swap.j),
						pi_1 = tour.at(cycle_id(test_swap.i + 1, tour)),
						pj_1 = tour.at(cycle_id(test_swap.j + 1, tour));
		if constexpr (detailDelta)
		{
			const double new1 = dist.get(pi, pj),
						 new2 = dist.get(pi_1, pj_1),
						 past1 = dist.get(pi, pi_1),
						 past2 = dist.get(pj, pj_1);
			return new1 + new2 - past1 - past2;
		}
		else
		{
			return dist.get(pi, pj) + dist.get(pi_1, pj_1) - dist.get(pi, pi_1) - dist.get(pj, pj_1);
		}
	};
	void flip(const TSPSwap &swap, const quality_delta_t delta, vector<town_in_tour_id_t> &tour) const
	{
		if constexpr (debugCost)
		{
			const double delta_cmpt = test_flip(swap, tour);
			const double past_cost = compute_quality_metric(tour);
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
			buffer = tour.at(cycle_id(swap.i + l, tour));
			tour[cycle_id(swap.i + l, tour)] = tour.at(cycle_id(swap.j - l + 1, tour));
			tour[cycle_id(swap.j - l + 1, tour)] = buffer;
		}
		return;
	}
};
template <bool detailDelta = false, bool debugCost = false>
class ThreeOptFlip : public OptFlip
{
public:
	ThreeOptFlip(DistanceMatrix &dist) : OptFlip(dist){};

	quality_delta_t test_flip(const TSPSwap &test_swap, vector<town_in_tour_id_t> &tour) const
	{
		const town_id_t pi = tour.at(test_swap.i),
						pj = tour.at(test_swap.j),
						pk = tour.at(test_swap.k),
						pi_1 = tour.at(cycle_id(test_swap.i + 1, tour)),
						pj_1 = tour.at(cycle_id(test_swap.j + 1, tour)),
						pk_1 = tour.at(cycle_id(test_swap.k + 1, tour));

		if constexpr (detailDelta)
		{
			const double new1 = dist.get(pi, pj),
						 new2 = dist.get(pj, pk),
						 new3 = dist.get(pk_1, pi_1),
						 past1 = dist.get(pi, pi_1),
						 past2 = dist.get(pj, pj_1),
						 past3 = dist.get(pk, pk_1);
			return new1 + new2 + new3 - past1 - past2 - past3;
		}
		else
		{
			return dist.get(pi, pj) + dist.get(pj, pk) + dist.get(pk_1, pi_1) - dist.get(pi, pi_1) - dist.get(pj, pj_1) - dist.get(pk, pk_1);
		}
	}

	void flip(const TSPSwap &swap, const quality_delta_t delta, vector<town_in_tour_id_t> &tour) const
	{
		if constexpr (debugCost)
		{
			const double delta_cmpt = test_flip(swap, tour);
			const double past_cost = compute_quality_metric(tour);
		}

		const int num_towns = tour.size();
		int buffer;

		if (swap.i >= tour.size() || swap.i < 0 || swap.j >= tour.size() || swap.j < 0 || swap.k >= tour.size() || swap.k < 0)
		{
			cout << "Wrong swap indices: i=" << swap.i << ", j=" << swap.j << ", k=" << swap.k << endl;
			exit(1);
		}

		if (swap.i == swap.j || swap.i == swap.k || swap.j == swap.k)
		{
			cout << "Duplicate swap indices: i=" << swap.i << ", j=" << swap.j << ", k=" << swap.k << endl;
			exit(1);
		}

		const int i = swap.i;
		const int j = swap.j;
		const int k = swap.k;

		// Perform the 3-opt flip
		// Reverse the sub-tour between indices i and j
		reverse(tour.begin() + cycle_id(i + 1, tour), tour.begin() + cycle_id(j + 1, tour));

		// Reverse the sub-tour between indices j and k
		reverse(tour.begin() + cycle_id(j + 1, tour), tour.begin() + cycle_id(k + 1, tour));

		// Reverse the sub-tour between indices k and i
		reverse(tour.begin() + cycle_id(k + 1, tour), tour.begin() + cycle_id(i + num_towns, tour));
	}
};
class TSPSolutionContainer : public AbstractSolutionContainer<TSPSwap>
{
public:
	vector<town_in_tour_id_t> &tour;
	const DistanceMatrix &dist;
	const OptFlip &flip_obj;

public:
	TSPSolutionContainer(vector<town_in_tour_id_t> &tour, const DistanceMatrix &m, const OptFlip &flip) : dist(m), AbstractSolutionContainer<TSPSwap>(), tour(tour), flip_obj(flip){};
	TSPSolutionContainer(const TSPSolutionContainer &other) : tour(other.tour), dist(other.dist), flip_obj(other.flip_obj)
	{
	}
	// Flips
	[[nodiscard]] quality_delta_t test_flip(const TSPSwap &test_swap) const 
	{
		return flip_obj.test_flip(test_swap, tour);
	};
	void flip(const TSPSwap &swap, const quality_delta_t delta)
	{
		flip_obj.flip(swap, delta, tour);
		update_with_delta_cost(delta);
		last_choice = swap;
	}
	// Objective
	double compute_quality_metric()
	{
		return tsp_compute_quality_metric(tour, dist);
	}
	// Getters
	[[nodiscard]] const int *get_tour() { return tour.data(); }
	const std::vector<double> &get_towns_pos() { return dist.get_town_pos(); }
	// Utility

	void print()
	{

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