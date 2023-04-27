#pragma once

#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>

#include "../../../src/types.h"
#include "../../../src/data/solution_container/maxsat.hpp"

using namespace std;


/**
* @brief Test the compute_quality_metric method of MAXSATSolutionContainer:
* - Toy example
Maximize: (v0 V NOT v1 V v2) AND (v1 V NOT v3 V v4) AND (NOT v0 V v3) AND (NOT v2 V NOT v3) AND (v0 V NOT v4)
With weights constant to 1
Subject to: v0, v1, v2, v3, v4 in {false, true}

Initial Assignment: v0=true v1=false v2=true v3=false v4=true
Expected quality: 4

*/
void test_MAXSATSolutionContainer_compute_quality_metric() {
	vector<bool> variable_assignments = { true, false, true, false, true };

	clause_t clause1 = { {0, true}, {1, false}, {2, true} };
	clause_t clause2 = { {1, true}, {3, false}, {4, true} };
	clause_t clause3 = { {0, false}, {3, true} };
	clause_t clause4 = { {2, false}, {3, false} };
	clause_t clause5 = { {0, true}, {4, false} };

	vector<clause_t> clauses = { clause1, clause2, clause3, clause4, clause5 };

	vector<double> weights;
	for (int i = 0; i < clauses.size(); i++) {
		weights.push_back(1);
	}
	MAXSATSolutionContainer c(weights, variable_assignments, clauses);
	const double quality = c.compute_quality_metric();
	if (abs(quality - 4) >= EPSILON) {
		throw runtime_error("The quality metric is not correct: expected 4 but got " + to_string(quality));
	}
}

/**
* @brief Test the flip method of MAXSATSolutionContainer:
* - With any solution, flipping two times the same variable must lead to the same original cost
*/
void test_MAXSATSolutionContainer_test_flip() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_variables = dis_ints(gen);
	const int num_clauses = dis_ints(gen);
	const int seed = dis_ints(gen) - 1;
	unique_ptr<vector<bool>> variable_assignments(random_assignements(num_variables, seed));

	unique_ptr<vector<clause_t>> clauses(random_clauses(num_variables, num_clauses, seed));

	unique_ptr<vector<double>> weights(fill_vector_with<double>(uniform_real_distribution<> {1, 1000}, num_clauses, seed));
	MAXSATSolutionContainer c(*weights, *variable_assignments, *clauses);
	const double quality = c.compute_quality_metric();
	for (int i = 0; i < 100; i++) {
		const int var_id = (uniform_int_distribution<>{ 0, num_variables - 1 })(gen);
		MAXSATSwap s1(var_id);
		const double delta1 = c.test_flip(s1);
		c.flip(s1, delta1);
		const double delta2 = c.test_flip(s1);
		c.flip(s1, delta2);
		if ((quality - c.compute_quality_metric()) >= EPSILON) {
			throw runtime_error("The quality metric is not correct: expected " + to_string(quality) + " but got " + to_string(c.compute_quality_metric()));
		}
	}
}