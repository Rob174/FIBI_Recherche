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

void reverse_segment(vector<town_in_tour_id_t> &tour, town_in_tour_id_t start, town_in_tour_id_t end)
{
    while (start < end)
    {
        swap(tour[start], tour[end]);
        start++;
        end--;
    }
}

// Helper function to reorder a segment of the tour
void reorder_segment(vector<town_in_tour_id_t> &tour, town_in_tour_id_t i, town_in_tour_id_t j, town_in_tour_id_t k)
{
	// tmp = tour[j:k] + tour[i:j]; 
	// tour[i:k] = tmp
	
    vector<town_in_tour_id_t> segment;
    // Extract the segment between j and k
	int ktmp = k;
	if(k<j) ktmp = tour.size()+k;
    for (town_in_tour_id_t idx = j; idx <= ktmp; idx++)
    {
        segment.push_back(tour[idx % tour.size()]);
    }
    // Add the segment between i and j
	int jtmp = k;
	if(j<i) jtmp = tour.size()+j;
    for (town_in_tour_id_t idx = i; idx <= jtmp; idx++)
    {
        segment.push_back(tour[idx % tour.size()]);
    }

    // Insert the segment between i and k
    for (town_in_tour_id_t idx = 0; idx < segment.size(); idx++)
    {
        tour[(i + idx)%tour.size()] = segment[idx%tour.size()];
    }
}
class OptFlip
{
public:
	DistanceMatrix &dist;
	OptFlip(DistanceMatrix &dist) : dist(dist){};
	virtual quality_delta_t test_flip(TSPSwap &test_swap, vector<town_in_tour_id_t> &tour) const = 0;
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
	quality_delta_t test_flip(TSPSwap &test_swap, vector<town_in_tour_id_t> &tour) const
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

	quality_delta_t test_flip(TSPSwap &test_swap, vector<town_in_tour_id_t> &tour) const
	{
		cout << test_swap.str() << endl;
		const town_id_t pi = tour.at(test_swap.i),
                    pj = tour.at(test_swap.j),
                    pk = tour.at(test_swap.k),
                    pi_1 = tour.at(cycle_id(test_swap.i + 1, tour)),
                    pj_1 = tour.at(cycle_id(test_swap.j + 1, tour)),
                    pk_1 = tour.at(cycle_id(test_swap.k + 1, tour));

		// Calculate the current edge lengths
		double e0 = dist.get(pi, pi_1); // A-B
		double e1 = dist.get(pj, pj_1); // C-D
		double e2 = dist.get(pk, pk_1); // E-F

		double e3 = dist.get(pi, pj); // A-C
		double e4 = dist.get(pj_1, pk); // D-E
		double e5 = dist.get(pk_1, pi_1); // F-B
		double e6 = dist.get(pi, pk_1); // A-F
		double e7 = dist.get(pj, pi_1); // C-B
		double e8 = dist.get(pj_1, pk); // D-E

		// Calculate the possibilities for the 3-opt heuristic
		double d0 = e0 + e1 + e2;         // [...A-B...C-D...E-F...]
		double d1 = e3 + e4 + e5;         // [...A-C...D-E...B-F...]
		double d2 = e0 + e3 + e6;         // [...A-C...E-F...B-D...]
		double d3 = e1 + e4 + e7;         // [...A-D...E-B...C-F...]
		double d4 = e2 + e5 + e8;         // [...F-B...C-D...E-A...]
		// Find the minimum distance and the index of the chosen possibility
		double min_distance = d0;
		test_swap.possibility_idx = -1;

		if (d1 < min_distance) {
			min_distance = d1;
			test_swap.possibility_idx = 0;
		}

		if (d2 < min_distance) {
			min_distance = d2;
			test_swap.possibility_idx = 1;
		}

		if (d3 < min_distance) {
			min_distance = d3;
			test_swap.possibility_idx = 2;
		}

		if (d4 < min_distance) {
			min_distance = d4;
			test_swap.possibility_idx = 3;
		}
		return min_distance;
	}

	void flip(const TSPSwap &swap, const quality_delta_t delta, vector<town_in_tour_id_t> &tour) const
	{
		cout << "start1" <<endl;
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

		// Extract the indices of cities involved in the swap
		town_in_tour_id_t i = swap.i;
		town_in_tour_id_t j = swap.j;
		town_in_tour_id_t k = swap.k;
		cout << "step1" <<endl;
		// Perform the flip based on the chosen possibility (stored in swap.possibility_idx)
		if (swap.possibility_idx == 0)// [...A-C...D-E...B-F...]: tour[i:j] = reversed(tour[i:j])
		{
		cout << "step11" <<endl;
			// Reverse the segment between i and j
			int jtmp = j;
			if (j<i) jtmp = tour.size()+j;
			reverse_segment(tour, i, jtmp);
		}
		else if (swap.possibility_idx == 1) // [...A-C...E-F...B-D...] tour[j:k] = reversed(tour[j:k])
		{
		cout << "step12" <<endl;
			// Reverse the segment between j and k
			int ktmp = k;
			if (k<j) ktmp = tour.size()+k;
			reverse_segment(tour, j, ktmp);
		}
		else if (swap.possibility_idx == 2)// [...A-D...E-B...C-F...] tmp = tour[j:k] + tour[i:j]; tour[i:k] = tmp
		{
		cout << "step13" <<endl;
			// Reorder the segment between i and k
			reorder_segment(tour, i, j, k);
		}
		else if (swap.possibility_idx == 3)// [...F-B...C-D...E-A...] our[i:k] = reversed(tour[i:k])
		{
		cout << "step14" <<endl;
			// Reverse the segment between i and k
			int ktmp = k;
			if (k<i) ktmp = tour.size()+k;
			reverse_segment(tour, i, ktmp);
		}
		cout << "step2" <<endl;
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
	[[nodiscard]] quality_delta_t test_flip(TSPSwap &test_swap) const 
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