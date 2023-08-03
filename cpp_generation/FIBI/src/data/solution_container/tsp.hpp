#pragma once

#include <vector>
#include "./abstract.hpp"
#include "distance_matrix.hpp"
#include "../../algorithm/swaps/tsp.hpp"


town_in_tour_id_t cycle_id(const town_in_tour_id_t i, const vector<town_in_tour_id_t> &tour)
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
class TSPSolutionContainer : public AbstractSolutionContainer
{
public:
	vector<town_in_tour_id_t> &tour;
	const DistanceMatrix &dist;

public:
	TSPSolutionContainer(vector<town_in_tour_id_t> &tour, const DistanceMatrix &m) : dist(m), AbstractSolutionContainer(), tour(tour){};
	TSPSolutionContainer(const TSPSolutionContainer &other) : tour(other.tour), dist(other.dist)
	{
	}
	// Objective
	double compute_quality_metric() override
	{
		return tsp_compute_quality_metric(tour, dist);
	}
	// Getters
	[[nodiscard]] const int *get_tour() { return tour.data(); }
	const std::vector<double> &get_towns_pos() { return dist.get_town_pos(); }
	// Utility

	void print() override
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