#pragma once

#include <vector>
#include <algorithm>
#include "../../../src/data/solution_container/tsp.hpp"
#include "../../constants.hpp"
#include "../../../src/data/get_data.hpp"

using namespace std;
/**
* @brief Shift all values in a vector by one, makes last element first
* @param v The vector to shift
*/
template <typename T>
void shift_one(vector<T>& v) {
	int temp = v.back(); // store the last element

	for (int i = v.size() - 1; i > 0; i--) {
		v[i] = v[i - 1]; // shift elements by one index
	}

	v[0] = temp; // set the first element to the stored last element
};
/**
* @brief Test the compute_quality_metric method of TSPSolutionContainer:
* - A shifted tour has the same quality metric as the original tour: cost(1,2,3,4,5) = cost(2,3,4,5,1) = ...
*/
void test_TSPSolutionContainer_compute_quality_metric_1() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_towns = dis_ints(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> towns(uniform_points(num_towns, num_dims, seed));
	DistanceMatrix m(num_towns, num_dims, *towns);
	vector<int> tour;
	for (int i = 0; i < num_towns; i++) {
		tour.push_back(i);
	}
	TSPSolutionContainer c(tour, m);
	const double quality = c.compute_quality_metric();
	// Check that the compute_quality_metric gives the same result if we shift the tour
	for (int shift = 0; shift < num_towns; shift++) {
		shift_one(tour);
		const double quality_shifted = c.compute_quality_metric();
		if ((quality - quality_shifted) >= EPSILON) {
			throw runtime_error("The quality metric is not invariant to shifting the tour: expected " + to_string(quality) + " but got " + to_string(quality_shifted));
		}
	}
}
/**
* @brief Test the compute_quality_metric method of TSPSolutionContainer:
* - Given towns where only the x coordinate is not null, the x coordinates are sorted, the quality metric is the difference between the first and last x coordinates times 2
*/
void test_TSPSolutionContainer_compute_quality_metric_2() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_towns = dis_ints(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	uniform_real_distribution<> dis_X(0, max_int);
	vector<double> towns;
	vector<double> x_coords;
	for (int i = 0; i < num_towns; i++) {
		x_coords.push_back(dis_X(gen));
	}
	// Sort the x coordinates
	sort(x_coords.begin(), x_coords.end());
	for (int i = 0; i < num_towns; i++) {
		towns.push_back(x_coords[i]);
		towns.push_back(0);
	}

	DistanceMatrix m(num_towns, num_dims, towns);
	vector<int> tour;
	for (int i = 0; i < num_towns; i++) {
		tour.push_back(i);
	}
	// The distance expected is the difference between the first and last x coordinates times 2
	TSPSolutionContainer c(tour, m);
	const double quality = c.compute_quality_metric();
	if ((quality - (x_coords.back() - x_coords.front()) * 2) >= EPSILON) {
		throw runtime_error("The quality metric is not correct for a toy example: expected " + to_string((x_coords.back() - x_coords.front()) * 2) + " but got " + to_string(quality));
	}
}
/**
* @brief Test the test_flip method of TSPSolutionContainer:
* - Given towns where only the x coordinate is not null, the x coordinates are sorted, if we flip the second and the second to last towns, the expected length will be
		2 times the difference between the first and last x coordinates plus the difference between the 3rd and second to last x coordinates (00000 portion)
  +------------------------------------------------------+
  |                                                      |
  |                            +-------------------------+
  |                            |                         |
+->+            +-+           +++               +-+     +v+
+--+----------->+-+---------->+-+-------------->+-+---->+-+
				 x                               x
				 |                0000000000000  ^
				 +-------------------------------+
*/
void test_TSPSolutionContainer_test_flip() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_towns = 5;
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	uniform_real_distribution<> dis_X(0, max_int);
	vector<double> towns;
	vector<double> x_coords;
	for (int i = 0; i < num_towns; i++) {
		x_coords.push_back(dis_X(gen));
	}
	// Sort the x coordinates
	sort(x_coords.begin(), x_coords.end());
	for (int i = 0; i < num_towns; i++) {
		towns.push_back(x_coords[i]);
		towns.push_back(0);
	}

	DistanceMatrix m(num_towns, num_dims, towns);
	vector<int> tour;
	for (int i = 0; i < num_towns; i++) {
		tour.push_back(i);
	}
	// The distance expected is the difference between the first and last x coordinates times 2
	TSPSolutionContainer c(tour, m);
	const double quality = c.compute_quality_metric();
	TSPSwap s(1, num_towns - 2);
	const double delta = c.test_flip(s);
	double expected_delta = x_coords[num_towns - 3] - x_coords[num_towns - 2];
	if ((expected_delta - delta) >= EPSILON) {
		string x_coords_str = "";
		for (int i = 0; i < num_towns; i++) {
			x_coords_str += to_string(x_coords[i]) + " ";
		}
		throw runtime_error(
			"The delta metric is not correct for a toy example: expected "
			+ to_string(expected_delta)
			+ " = overlapping portion "
			+ to_string(x_coords[num_towns - 2] - x_coords[2])
			+ " \n but got "
			+ to_string(delta)
			+ " \n(x_coords: "
			+ x_coords_str
			+ ")"
		);
	}
}

/**
* @brief Test the test_flip method of TSPSolutionContainer:
* - Given towns where only the x coordinate is not null, the x coordinates are sorted, if we flip the second and the second to last towns, the expected length will be
		2 times the difference between the first and last x coordinates plus the difference between the 3rd and second to last x coordinates (00000 portion)
  +------------------------------------------------------+
  |                                                      |
  |                            +-------------------------+
  |                            |                         |
+->+            +-+           +++               +-+     +v+
+--+----------->+-+---------->+-+-------------->+-+---->+-+
				 x                               x
				 |                0000000000000  ^
				 +-------------------------------+
*/
void test_TSPSolutionContainer_flip() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_towns = 5;
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	uniform_real_distribution<> dis_X(0, max_int);
	vector<double> towns;
	vector<double> x_coords;
	for (int i = 0; i < num_towns; i++) {
		x_coords.push_back(dis_X(gen));
	}
	// Sort the x coordinates
	sort(x_coords.begin(), x_coords.end());
	for (int i = 0; i < num_towns; i++) {
		towns.push_back(x_coords[i]);
		towns.push_back(0);
	}

	DistanceMatrix m(num_towns, num_dims, towns);
	vector<int> tour;
	for (int i = 0; i < num_towns; i++) {
		tour.push_back(i);
	}
	// The distance expected is the difference between the first and last x coordinates times 2
	TSPSolutionContainer c(tour, m);
	const double quality = c.compute_quality_metric();
	TSPSwap s(1, num_towns - 2);
	const double delta = c.test_flip(s);
	c.flip(s, delta);
	const double new_quality = c.compute_quality_metric();
	// Check that the quality is correct
	if ((c.get_quality_metric() - new_quality) >= EPSILON) {
		throw runtime_error("The quality metric is not correct after a flip: expected " + to_string(new_quality) + " but got " + to_string(c.get_quality_metric()));
	}
}