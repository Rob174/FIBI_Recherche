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

    // Calculate the cost before and after the flip
    double var = 0;
    test_swap.possibility_idx = 0;

    double tmp_var = 0;
    // ACBEDF
    tmp_var = - dist.get(tour[a], tour[b]) - dist.get(tour[c], tour[d]) - dist.get(tour[e], tour[f]) + dist.get(tour[a], tour[c]) + dist.get(tour[b], tour[e]) + dist.get(tour[d], tour[f]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 1;
    }
    // ADEBCF
    tmp_var = - dist.get(tour[a], tour[b]) - dist.get(tour[c], tour[d]) - dist.get(tour[e], tour[f]) + dist.get(tour[a], tour[d]) + dist.get(tour[e], tour[b]) + dist.get(tour[c], tour[f]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 2;
    }
    // AEDBCF
    tmp_var = - dist.get(tour[a], tour[b]) - dist.get(tour[c], tour[d]) - dist.get(tour[e], tour[f]) + dist.get(tour[a], tour[e]) + dist.get(tour[d], tour[b]) + dist.get(tour[c], tour[f]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 3;
    }
    // AEDCBF
    tmp_var = - dist.get(tour[a], tour[b]) - dist.get(tour[e], tour[f]) + dist.get(tour[b], tour[f]) + dist.get(tour[a], tour[e]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 4;
    }
    // FDECBA
    tmp_var = - dist.get(tour[c], tour[d]) - dist.get(tour[e], tour[f]) + dist.get(tour[e], tour[c]) + dist.get(tour[f], tour[d]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 5;
    }
    // ACBDEF
    tmp_var = - dist.get(tour[a], tour[b]) - dist.get(tour[c], tour[d]) + dist.get(tour[a], tour[c]) + dist.get(tour[b], tour[d]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 6;
    }
    // ADECBF
    tmp_var = - dist.get(tour[a], tour[b]) - dist.get(tour[c], tour[d]) - dist.get(tour[e], tour[f]) + dist.get(tour[a], tour[d]) + dist.get(tour[b], tour[f]) + dist.get(tour[e], tour[c]);
    if (tmp_var < var) {
        var = tmp_var;
        test_swap.possibility_idx = 7;
    }
    

    return var;
}

void fn_flip(const TSPThreeOptSwap &swap, vector<town_in_tour_id_t> &tour)
{
    town_in_tour_id_t a = swap.i;
    town_in_tour_id_t b = (swap.i + 1) % tour.size();
    town_in_tour_id_t c = swap.j;
    town_in_tour_id_t d = (swap.j + 1) % tour.size();
    town_in_tour_id_t e = swap.k;
    town_in_tour_id_t f = (swap.k + 1) % tour.size();

    // Assuming that a < b < c < d < e < f

    // Perform the 3-opt move based on possibility_idx
    if (swap.possibility_idx == 0)
    {
        // Do nothing (identity)
    }
    else if (swap.possibility_idx == 1) {
        // Flip possibility 1: ABCDEF -> ACBEDF
        reverse(tour.begin() + b, tour.begin() + d);
        reverse(tour.begin() + d, tour.begin() + f);

    } else if (swap.possibility_idx == 2) {
        // Flip possibility 2: ABCDEF -> ADEBCF
        vector<int> idx{tour[a]};
        for(int q=d; q<e+1; q++) {
            idx.push_back(tour[q]);
        }
        for(int q=b; q<c+1; q++) {
            idx.push_back(tour[q]);
        }
        for(int q=f; q<tour.size()+a; q++) {
            idx.push_back(tour[q%tour.size()]);
        }
        tour = idx;
    } else if (swap.possibility_idx == 3) {
        // Flip possibility 3: ABCDEF -> AEDBCF
        vector<int> idx{tour[a]};
        for(int q=e; q>=d; q--) {
            idx.push_back(tour[q]);
        }
        for(int q=b; q<c+1; q++) {
            idx.push_back(tour[q]);
        }
        for(int q=f; q<tour.size()+a; q++) {
            idx.push_back(tour[q%tour.size()]);
        }
        tour = idx;
    } else if (swap.possibility_idx == 4) {
        // Flip possibility 4: ABCDEF -> AEDCBF
        vector<int> idx{tour[a]};
        for(int q=e; q>=d; q--) {
            idx.push_back(tour[q]);
        }
        for(int q=c; q>=b; q--) {
            idx.push_back(tour[q]);
        }
        for(int q=f; q<tour.size()+a; q++) {
            idx.push_back(tour[q%tour.size()]);
        }
        tour = idx;
    } else if (swap.possibility_idx == 5) {
        // Flip possibility 5: ABCDEF -> FDECBA
        vector<int> idx{};
        for(int q=a; q>=(int)(f-tour.size()); q--) {
            idx.push_back(tour[(q+tour.size())%tour.size()]);
        }
        for(int q=d; q<e+1; q++) {
            idx.push_back(tour[q]);
        }
        for(int q=c; q>=b; q--) {
            idx.push_back(tour[q]);
        }
        tour = idx;
    } else if (swap.possibility_idx == 6) {
        // Flip possibility 6: ABCDEF -> ACBDEF
        vector<int> idx{tour[a]};
        for(int q=c; q>=b; q--) {
            idx.push_back(tour[q]);
        }
        for(int q=d; q<e+1; q++) {
            idx.push_back(tour[q]);
        }
        for(int q=f; q<tour.size()+a; q++) {
            idx.push_back(tour[q%tour.size()]);
        }
        tour = idx;
    } else if (swap.possibility_idx == 7) {
        // Flip possibility 6: ABCDEF -> ADECBF
        vector<int> idx{tour[a]};
        for(int q=d; q<e+1; q++) {
            idx.push_back(tour[q]);
        }
        for(int q=c; q>=b; q--) {
            idx.push_back(tour[q]);
        }
        for (int q = f; q < (a > f ? a : tour.size() + a); q++) {
            idx.push_back(tour[q%tour.size()]);
        }
        tour = idx;
    }
}


quality_delta_t fn_test_flip(TSPInsertionSwap &test_swap, const vector<town_in_tour_id_t> &tour, const DistanceMatrix &dist)
{
    using distance_t = double;
    // Check if i and new_pos are equal or if they are negative
    if (test_swap.i == test_swap.new_pos || test_swap.i < 0 || test_swap.new_pos < 0) {
        // Return 0 if i equals new_pos or if either i or new_pos is negative
        return 0;
    }

    // Get the IDs of the towns to be swapped
    town_in_tour_id_t town1_id = tour[test_swap.i];
    town_in_tour_id_t town2_id = tour[test_swap.new_pos];

    // Calculate the distance between town1 and its predecessor
    town_in_tour_id_t prev1_id = (test_swap.i == 0) ? tour.back() : tour[test_swap.i - 1];
    distance_t dist1 = dist.get(prev1_id, town1_id);

    // Calculate the distance between town1 and its successor
    town_in_tour_id_t next1_id = (test_swap.i == tour.size() - 1) ? tour[0] : tour[test_swap.i + 1];
    distance_t dist2 = dist.get(town1_id, next1_id);

    int prev_new_pos = test_swap.new_pos;
    //if (test_swap.i > test_swap.new_pos) prev_new_pos = (prev_new_pos - 1 < 0) ? tour.size()-1 : prev_new_pos - 1;
    // Calculate the distance between town2 and its predecessor
    town_in_tour_id_t prev2_id = tour[prev_new_pos];
    distance_t dist3 = dist.get(prev2_id, tour[test_swap.i]);

    // Calculate the distance between town2 and its successor
    town_in_tour_id_t next2_id = tour[(prev_new_pos + 1)%tour.size()];
    distance_t dist4 = dist.get(tour[test_swap.i], next2_id);

    town_in_tour_id_t next2_id_i = tour[(test_swap.i + 1) % tour.size()];
    town_in_tour_id_t prev2_id_i = tour[(test_swap.i - 1 + tour.size()) % tour.size()];
    distance_t dist5 = dist.get(prev2_id_i, next2_id_i);

    distance_t dist6 = dist.get(tour[prev_new_pos], next2_id);
    // Calculate the change in tour length (delta) after the swap
    quality_delta_t delta = dist3 + dist4 + dist5 - (dist1 + dist2 + dist6);

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
		tour.insert(tour.begin() + swap.new_pos, id);
	}
	else
	{
		// In this case, we want to insert the town at the position swap.new_pos itself, as there is no need to adjust for a previously removed town.
		tour.insert(tour.begin() + swap.new_pos+1, id);
	}
}