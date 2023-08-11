#pragma once

#include <vector>
#include "../../algorithm/swaps/tsp.hpp"
#include "../solution_container/tsp.hpp"

using namespace std;

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
    town_in_tour_id_t a = test_swap.i;
    town_in_tour_id_t b = (test_swap.i + 1) % tour.size();
    town_in_tour_id_t c = test_swap.j;
    town_in_tour_id_t d = (test_swap.j + 1) % tour.size();
    town_in_tour_id_t e = test_swap.k;
    town_in_tour_id_t f = (test_swap.k + 1) % tour.size();

    // Reorder the indices for simplicity
    if (b > d)
    {
        std::swap(b, d);
    }
    if (d > f)
    {
        std::swap(d, f);
    }
    if (b > d)
    {
        std::swap(b, d);
    }

    // Calculate the cost before and after the flip
    double cost_before = dist.get(tour[a], tour[b]) + dist.get(tour[c], tour[d]) + dist.get(tour[e], tour[f]);
    double cost_after;

    if (test_swap.possibility_idx == 0)
    {
        // No change (identity)
        cost_after = cost_before;
    }
    else if (test_swap.possibility_idx == 1)
    {
        cost_after = cost_before - dist.get(tour[b], tour[d]) - dist.get(tour[c], tour[e]) + dist.get(tour[b], tour[c]) + dist.get(tour[d], tour[e]);
    }
    else if (test_swap.possibility_idx == 2)
    {
        cost_after = cost_before - dist.get(tour[d], tour[f]) - dist.get(tour[a], tour[b]) + dist.get(tour[a], tour[d]) + dist.get(tour[b], tour[f]);
    }
    else if (test_swap.possibility_idx == 3)
    {
        cost_after = cost_before - dist.get(tour[b], tour[d]) - dist.get(tour[c], tour[e]) - dist.get(tour[a], tour[b]) + dist.get(tour[a], tour[d]) + dist.get(tour[b], tour[c]) + dist.get(tour[c], tour[e]);
    }
    else if (test_swap.possibility_idx == 4)
    {
        cost_after = cost_before - dist.get(tour[b], tour[c]) - dist.get(tour[d], tour[e]) - dist.get(tour[a], tour[d]) + dist.get(tour[a], tour[b]) + dist.get(tour[c], tour[e]) + dist.get(tour[d], tour[f]);
    }
    else if (test_swap.possibility_idx == 5)
    {
        cost_after = cost_before - dist.get(tour[a], tour[d]) - dist.get(tour[c], tour[f]) - dist.get(tour[b], tour[d]) + dist.get(tour[a], tour[b]) + dist.get(tour[c], tour[e]) + dist.get(tour[d], tour[e]);
    }
    else if (test_swap.possibility_idx == 6)
    {
        cost_after = cost_before - dist.get(tour[a], tour[d]) - dist.get(tour[b], tour[c]) - dist.get(tour[d], tour[f]) + dist.get(tour[a], tour[b]) + dist.get(tour[c], tour[d]) + dist.get(tour[e], tour[f]);
    }
    else if (test_swap.possibility_idx == 7)
    {
        cost_after = cost_before - dist.get(tour[c], tour[e]) + dist.get(tour[a], tour[c]) + dist.get(tour[e], tour[f]);
    }

    return cost_after - cost_before;
}

void fn_flip(const TSPThreeOptSwap &swap, vector<town_in_tour_id_t> &tour)
{
    town_in_tour_id_t a = swap.i;
    town_in_tour_id_t b = (swap.i + 1) % tour.size();
    town_in_tour_id_t c = swap.j;
    town_in_tour_id_t d = (swap.j + 1) % tour.size();
    town_in_tour_id_t e = swap.k;
    town_in_tour_id_t f = (swap.k + 1) % tour.size();

    // Reorder the indices for simplicity
    if (b > d)
    {
        std::swap(b, d);
    }
    if (d > f)
    {
        std::swap(d, f);
    }
    if (b > d)
    {
        std::swap(b, d);
    }

    // Perform the 3-opt move based on possibility_idx
    if (swap.possibility_idx == 0)
    {
        // Do nothing (identity)
    }
    else if (swap.possibility_idx == 1)
    {
        reverse(tour.begin() + b, tour.begin() + d);
    }
    else if (swap.possibility_idx == 2)
    {
        reverse(tour.begin() + d, tour.begin() + f);
    }
    else if (swap.possibility_idx == 3)
    {
        reverse(tour.begin() + b, tour.begin() + d);
        reverse(tour.begin() + d, tour.begin() + f);
    }
    else if (swap.possibility_idx == 4)
    {
        reverse(tour.begin() + b, tour.begin() + d);
        reverse(tour.begin() + c, tour.begin() + e);
    }
    else if (swap.possibility_idx == 5)
    {
        reverse(tour.begin() + d, tour.begin() + f);
        reverse(tour.begin() + c, tour.begin() + e);
    }
    else if (swap.possibility_idx == 6)
    {
        reverse(tour.begin() + b, tour.begin() + d);
        reverse(tour.begin() + c, tour.begin() + e);
        reverse(tour.begin() + d, tour.begin() + f);
    }
    else if (swap.possibility_idx == 7)
    {
        reverse(tour.begin() + c, tour.begin() + e);
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