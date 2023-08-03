#pragma once

#include <vector>
#include "../../algorithm/swaps/tsp.hpp"
#include "../solution_container/tsp.hpp"

using namespace std;

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
	if (k < j)
		ktmp = tour.size() + k;
	for (town_in_tour_id_t idx = j; idx <= ktmp; idx++)
	{
		segment.push_back(tour[idx % tour.size()]);
	}
	// Add the segment between i and j
	int jtmp = k;
	if (j < i)
		jtmp = tour.size() + j;
	for (town_in_tour_id_t idx = i; idx <= jtmp; idx++)
	{
		segment.push_back(tour[idx % tour.size()]);
	}

	// Insert the segment between i and k
	for (town_in_tour_id_t idx = 0; idx < segment.size(); idx++)
	{
		tour[(i + idx) % tour.size()] = segment[idx % tour.size()];
	}
}
quality_delta_t fn_test_flip1(TSPTwoOptSwap &test_swap, const vector<town_in_tour_id_t> &tour, const DistanceMatrix &dist)
{
	const town_id_t pi = tour.at(test_swap.i),
					pj = tour.at(test_swap.j),
					pi_1 = tour.at(cycle_id(test_swap.i + 1, tour)),
					pj_1 = tour.at(cycle_id(test_swap.j + 1, tour));
	return dist.get(pi, pj) + dist.get(pi_1, pj_1) - dist.get(pi, pi_1) - dist.get(pj, pj_1);
};
void fn_flip(const TSPTwoOptSwap &swap, vector<town_in_tour_id_t> &tour)
{
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
}
quality_delta_t fn_test_flip(TSPThreeOptSwap &test_swap, const vector<town_in_tour_id_t> &tour, const DistanceMatrix &dist)
{
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

	double e3 = dist.get(pi, pj);	  // A-C
	double e4 = dist.get(pj_1, pk);	  // D-E
	double e5 = dist.get(pk_1, pi_1); // F-B
	double e6 = dist.get(pi, pk_1);	  // A-F
	double e7 = dist.get(pj, pi_1);	  // C-B
	double e8 = dist.get(pj_1, pk);	  // D-E

	// Calculate the possibilities for the 3-opt heuristic
	double d0 = e0 + e1 + e2; // [...A-B...C-D...E-F...]
	double d1 = e3 + e4 + e5; // [...A-C...D-E...B-F...]
	double d2 = e0 + e3 + e6; // [...A-C...E-F...B-D...]
	double d3 = e1 + e4 + e7; // [...A-D...E-B...C-F...]
	double d4 = e2 + e5 + e8; // [...F-B...C-D...E-A...]
	// Find the minimum distance and the index of the chosen possibility
	double min_distance = d0;
	test_swap.possibility_idx = -1;

	if (d1 < min_distance)
	{
		min_distance = d1;
		test_swap.possibility_idx = 0;
	}

	if (d2 < min_distance)
	{
		min_distance = d2;
		test_swap.possibility_idx = 1;
	}

	if (d3 < min_distance)
	{
		min_distance = d3;
		test_swap.possibility_idx = 2;
	}

	if (d4 < min_distance)
	{
		min_distance = d4;
		test_swap.possibility_idx = 3;
	}
	return min_distance;
}

void fn_flip(const TSPThreeOptSwap &swap, vector<town_in_tour_id_t> &tour)
{

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
	// Perform the flip based on the chosen possibility (stored in swap.possibility_idx)
	if (swap.possibility_idx == 0) // [...A-C...D-E...B-F...]: tour[i:j] = reversed(tour[i:j])
	{
		// Reverse the segment between i and j
		int jtmp = j;
		if (j < i)
			jtmp = tour.size() + j;
		reverse_segment(tour, i, jtmp);
	}
	else if (swap.possibility_idx == 1) // [...A-C...E-F...B-D...] tour[j:k] = reversed(tour[j:k])
	{
		// Reverse the segment between j and k
		int ktmp = k;
		if (k < j)
			ktmp = tour.size() + k;
		reverse_segment(tour, j, ktmp);
	}
	else if (swap.possibility_idx == 2) // [...A-D...E-B...C-F...] tmp = tour[j:k] + tour[i:j]; tour[i:k] = tmp
	{
		// Reorder the segment between i and k
		reorder_segment(tour, i, j, k);
	}
	else if (swap.possibility_idx == 3) // [...F-B...C-D...E-A...] our[i:k] = reversed(tour[i:k])
	{
		// Reverse the segment between i and k
		int ktmp = k;
		if (k < i)
			ktmp = tour.size() + k;
		reverse_segment(tour, i, ktmp);
	}
}


quality_delta_t fn_test_flip(TSPInsertionSwap &test_swap, const vector<town_in_tour_id_t> &tour, const DistanceMatrix &dist)
{
	// Test the impact of changing the position of a town in the tour:
	// [...,A,B,C,...,D,E,F,...] we choose E to insert at position of B
	// [...,A,E,B,C,...,D,F,...]: result
	town_in_tour_id_t town_to_move = tour[test_swap.i]; // Store the town to be moved.

	const int a = test_swap.i - 1 < 0 ? tour.size() - 1 : test_swap.i - 1, b = test_swap.i + 1 == tour.size() ? 0 : test_swap.i + 1;
	const int c = test_swap.new_pos - 1 < 0 ? tour.size() - 1 : test_swap.new_pos - 1, d = test_swap.new_pos + 1 == tour.size() ? 0 : test_swap.new_pos + 1;

	double delta = 0;
	// We remove the edges around the position in the tour
	delta -= dist.get(tour.at(a), tour.at(test_swap.i));
	delta -= dist.get(tour.at(test_swap.i), tour.at(b));
	// And the one at new_pos
	delta -= dist.get(tour.at(c), tour.at(test_swap.new_pos));
	// And we add the new edges between a and b and between c and i and between i and new_pos
	delta += dist.get(tour.at(a), tour.at(b));
	delta += dist.get(tour.at(c), tour.at(test_swap.i));
	delta += dist.get(tour.at(test_swap.i), tour.at(d));
	// Return the computed variation of tour length.
	return delta;
}

void fn_flip(const TSPInsertionSwap &swap, vector<town_in_tour_id_t> &tour)
{
	// Apply the flip operation to the tour:
	// Remove the town to be moved from its current position
	const town_id_t id = tour.at(swap.i);
	tour.erase(tour.begin() + swap.i);

	// Insert the town at the new position
	if (swap.i < swap.new_pos)
	{
		// In this case, we want to insert the town just before the position swap.new_pos, so we use tour.begin() + swap.new_pos - 1 as the insertion point. The -1 is there to account for the fact that we removed the town from its original position before inserting it at the new position.
		tour.insert(tour.begin() + swap.new_pos - 1, id);
	}
	else
	{
		// In this case, we want to insert the town at the position swap.new_pos itself, as there is no need to adjust for a previously removed town.
		tour.insert(tour.begin() + swap.new_pos, id);
	}
}