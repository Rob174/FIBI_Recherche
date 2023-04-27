#pragma once
#include <vector>
#include <random>
#include <set>
#include "../solution_container/distance_matrix.hpp"
using namespace std;

/* Return an improved tour by starting from a random point and then choosing the nearest point
* @param num_towns number of towns in the tour
* @param seed seed
* @param distances distance matrix
* @return a random tour in the form of a vector of integers
*/
vector<int>* init_tsp_greedy(const long num_towns, const long seed, const DistanceMatrix& distances)
{
	vector<int>* tour = new vector<int>(num_towns);
	// Add a set to avoid duplicates (a points can be the nearest one for two different points)
	set<int> available_points;
	for (int i = 0; i < num_towns; i++)
	{
		available_points.insert(i);
	}
	mt19937 gen_dist(seed);
	uniform_int_distribution<> dis_choice(0, num_towns - 1);
	int start_point = dis_choice(gen_dist);
	tour->at(0) = start_point;
	available_points.erase(start_point);
	for (int i = 0; i < num_towns - 1; i++)
	{
		// Find the argmin of distance
		double min = 1e100;
		int argmin = -1;
		for (int j : available_points)
		{
			double d = distances.get(tour->at(i), j);
			if (d < min)
			{
				min = d;
				argmin = j;
			}
		}
		tour->at(i + 1) = argmin;
		// Erase the point to avoid duplicates
		available_points.erase(argmin);
	}
	return tour;
}
/** Return an improved tour by starting from a random point and then choosing the nearest point among the top N
* @param num_towns number of towns in the tour
* @param seed seed
* @param distances distance matrix
* @param topk number of nearest points to consider
* @return a random tour in the form of a vector of integers
*/
vector<int>* init_tsp_greedy_topk(const long num_towns, const long seed, const DistanceMatrix& distances, const int topk)
{
	const int N = topk;
	vector<int>* tour = new vector<int>(num_towns);
	// Add a set to avoid duplicates (a points can be the nearest one for two different points)
	set<int> available_points;
	for (int i = 0; i < num_towns; i++)
	{
		available_points.insert(i);
	}
	mt19937 gen_dist(seed);
	uniform_int_distribution<> dis_choice(0, num_towns - 1);
	int start_point = dis_choice(gen_dist);
	tour->at(0) = start_point;
	available_points.erase(start_point);
	for (int i = 0; i < num_towns - 1; i++)
	{
		// Prepare for the top N
		vector<pair<double, int>> top_x_data;
		for (int j : available_points)
		{
			double d = distances.get(tour->at(i), j);
			top_x_data.push_back(make_pair(d, j));
		}
		// Take top N
		const int size_top = N < (int)(top_x_data.size()) ? N : (int)(top_x_data.size());
		nth_element(top_x_data.begin(), top_x_data.begin() + size_top, top_x_data.end(),
			[](auto& l, auto& r)
			{ return l.first < r.first; });
		// Select one of the top N
		uniform_int_distribution<> dis_top(0, size_top - 1);
		const int selected_of_top_id = dis_top(gen_dist);
		tour->at(i + 1) = top_x_data.at(selected_of_top_id).second;
		// Erase the point to avoid duplicates
		available_points.erase(top_x_data.at(selected_of_top_id).second);
	}
	return tour;
}
/** Return an improved tour by starting from a random point and then choosing randomly among the remaining points ponderating probabilities by their distance
* @param num_towns number of towns in the tour
* @param seed seed
* @param distances distance matrix
* @param topk number of nearest points to consider
* @return a random tour in the form of a vector of integers
*/
vector<int>* init_tsp_greedy_randomized(const long num_towns, const long seed, const DistanceMatrix& distances)
{
	vector<int>* tour = new vector<int>(num_towns);
	// Add a set to avoid duplicates (a points can be the nearest one for two different points)
	set<int> available_points;
	for (int i = 0; i < num_towns; i++)
	{
		available_points.insert(i);
	}
	mt19937 gen_dist(seed);
	uniform_int_distribution<> dis_choice(0, num_towns - 1);
	int start_point = dis_choice(gen_dist);
	tour->at(0) = start_point;
	available_points.erase(start_point);
	for (int i = 0; i < num_towns - 1; i++)
	{
		// Prepare for the top N
		vector<double> weights;
		for (int j : available_points)
		{
			weights.push_back(distances.get(tour->at(i), j));
		}
		// Weighted distribution by the distance
		discrete_distribution<int> distribution(weights.begin(), weights.end());
		const int chosen_point = distribution(gen_dist);
		tour->at(i + 1) = chosen_point;
		// Erase the point to avoid duplicates
		available_points.erase(chosen_point);
	}
	return tour;
}