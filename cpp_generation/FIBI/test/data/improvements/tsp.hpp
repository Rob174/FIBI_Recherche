#pragma once
#include "../../../src/data/improvements/tsp.hpp"
#include "../../../src/data/get_data.hpp"
#include "../../../test/constants.hpp"

/**
* @brief Test the random_tour function:
* - Check that the number of towns is correct
* - Check that the tour is valid
*/
void test_random_tour() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis_ints(1, max_int);
	const int num_towns = dis_ints(gen);
	const int seed = dis_ints(gen)-1;
	unique_ptr<vector<int>> tour(random_tour(num_towns, seed));
	// Check that the number of towns is correct
	if (tour->size() != num_towns) {
		throw runtime_error("The number of towns is not correct: expected " + to_string(num_towns) + " but got " + to_string(tour->size()));
	}
	// Check that the tour is valid
	sort(tour->begin(), tour->end());
	for (int i = 0; i < num_towns; i++) {
		if (tour->at(i) != i) {
			throw runtime_error("The tour is not valid: sorted tour expected " + to_string(i) + " but got " + to_string(tour->at(i)));
		}
	}
}	
/**
* @brief Test the init_tsp_greedy function:
* - Check that the number of towns is correct
* - Check that the tour is valid
*/
void test_improved_tour_1() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis_ints(1, max_int);
	const int num_towns = dis_ints(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2; 
	const int seed = dis_ints(gen)-1;
	unique_ptr<const vector<double>> towns(uniform_points(num_towns, num_dims, seed));
	DistanceMatrix m(num_towns, num_dims, *towns);
	unique_ptr < vector<int>> tour(init_tsp_greedy(num_towns, seed, m));
	// Check that the number of towns is correct
	if (tour->size() != num_towns) {
		throw runtime_error("The number of towns is not correct: expected " + to_string(num_towns) + " but got " + to_string(tour->size()));
	}
	// Check that the tour is valid
	sort(tour->begin(), tour->end());
	for (int i = 0; i < num_towns; i++) {
		if (tour->at(i) != i) {
			throw runtime_error("The tour is not valid: sorted tour expected " + to_string(i) + " but got " + to_string(tour->at(i)));
		}
	}
}
/**
* @brief Test the init_tsp_greedy function:
* - Check on a particular example that the result is valid
*/
void test_improved_tour_2() {
	// To have always the same random numbers
	const int rd = 0;
	mt19937 gen(rd);
	const int num_towns = 10;
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = 0;
	const vector<double> *towns = uniform_points(num_towns, num_dims, seed);
	/*
		(59.284461651668266,84.426574425659823),
		(85.794561998982985,84.725173738433128),
		(62.356369649610834,38.438170837375665),
		(29.753460535723420,5.6712975933163667),
		(27.265629474158931,47.766511174464632),
		(81.216872664907129,47.997717152556753),
		(39.278479329497664,83.607876904439109),
		(33.739616164728908,64.817187657745762),
		(36.824153736704460,95.715515451333943),
		(14.035077760418801,87.008725126972706)
	*/
	DistanceMatrix m(num_towns, num_dims, *towns);
	const int start_town = 5; // Determined with the debugger for rthe current code
	vector<int> *tour = init_tsp_greedy(num_towns, seed, m);
	vector<int> expected_tour = { 5, 2, 4, 7, 6, 8, 9, 0, 1, 3 };
	// Check that the final tour is correct
	cout << "Checking... ";
	for (int i = 0; i < num_towns; i++) {
		if (tour->at(i) != expected_tour.at(i)) {
			cout << "\x1B[31m " << tour->at(i) << " <- \033[0m";
			throw runtime_error("The tour is not correct: expected " + to_string(expected_tour.at(i)) + " but got " + to_string(tour->at(i)) + " at position " + to_string(i));
		}
		else {
			// In green show the tour processed
			cout << "\x1B[32m " << tour->at(i) << "\033[0m";
			if (i < num_towns - 1) cout << ", ";
		}
	}
	cout << endl;
}
/**
* @brief Test the init_tsp_greedy_topk function:
* - Check that the number of towns is correct
* - Check that the tour is valid
*/
void test_improved_rand_tour_1() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis_ints(1, max_int);
	const int num_towns = dis_ints(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> towns(uniform_points(num_towns, num_dims, seed));
	DistanceMatrix m(num_towns, num_dims, *towns);
	unique_ptr < vector<int>> tour(init_tsp_greedy(num_towns, seed, m));
	// Check that the number of towns is correct
	if (tour->size() != num_towns) {
		throw runtime_error("The number of towns is not correct: expected " + to_string(num_towns) + " but got " + to_string(tour->size()));
	}
	// Check that the tour is valid
	sort(tour->begin(), tour->end());
	for (int i = 0; i < num_towns; i++) {
		if (tour->at(i) != i) {
			throw runtime_error("The tour is not valid: sorted tour expected " + to_string(i) + " but got " + to_string(tour->at(i)));
		}
	}
}

/**
* @brief Test the init_tsp_greedy_topk function:
* - Check that with a top 1 we have the same result as for test_improved_2
*/
void test_improved_rand_tour_2() {
	// To have always the same random numbers
	const int rd = 0;
	mt19937 gen(rd);
	const int num_towns = 10;
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = 2;
	const int seed = 0;
	const vector<double>* towns = uniform_points(num_towns, num_dims, seed);
	/*
		(59.284461651668266,84.426574425659823),
		(85.794561998982985,84.725173738433128),
		(62.356369649610834,38.438170837375665),
		(29.753460535723420,5.6712975933163667),
		(27.265629474158931,47.766511174464632),
		(81.216872664907129,47.997717152556753),
		(39.278479329497664,83.607876904439109),
		(33.739616164728908,64.817187657745762),
		(36.824153736704460,95.715515451333943),
		(14.035077760418801,87.008725126972706)
	*/
	DistanceMatrix m(num_towns, num_dims, *towns);
	const int start_town = 5; // Determined with the debugger for rthe current code
	vector<int>* tour = init_tsp_greedy_topk(num_towns, seed, m, 1);
	vector<int> expected_tour = { 5, 2, 4, 7, 6, 8, 9, 0, 1, 3 };
	// Check that the final tour is correct
	cout << "Checking... ";
	for (int i = 0; i < num_towns; i++) {
		if (tour->at(i) != expected_tour.at(i)) {
			cout << "\x1B[31m " << tour->at(i) << " <- \033[0m";
			throw runtime_error("The tour is not correct: expected " + to_string(expected_tour.at(i)) + " but got " + to_string(tour->at(i)) + " at position " + to_string(i));
		}
		else {
			// In green show the tour processed
			cout << "\x1B[32m " << tour->at(i) << "\033[0m";
			if (i < num_towns - 1) cout << ", ";
		}
	}
	cout << endl;
}