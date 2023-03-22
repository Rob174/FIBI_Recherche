#pragma once
#include <random>
#include <string>
#include <vector>
#include "../../../test/constants.hpp"
#include "../../../src/data/improvements/maxsat.hpp"
#include "../../../src/data/get_data.hpp"
#include "../../../src/types.h"
#include "../../../libs/json.hpp"



using namespace std;

/** @brief Test the general coherence of the result of the improve_maxsat function}
* - Expectedd number of assignements (as many as variables)
*/
void test_improve_maxsat() {
	const string path_hdf5 = "../../../data/folder/maxsat_benchmark/";
	const string path_json = "../../../data/maxsat_benchmark.json";

	// Read the json file
	ifstream json_file(path_json);
	// Check if opening the file was successful
	if (!json_file.is_open()) {
		string abs_path_json = filesystem::absolute(path_json).string();
		throw runtime_error("Could not open the json file at path " + path_json + " with current folder dir " + filesystem::current_path().string() + " = " + abs_path_json);
	}
	string content((istreambuf_iterator<char>(json_file)), istreambuf_iterator<char>());
	json_file.close();
	json j;
	j = json::parse(content);

	// Iterate over the keys of the json file list [{id: ..., name: ..., num_clauses: ..., num_variables: ...}, {...}, ...]
	for (auto it = j.begin(); it != j.end(); ++it) {
		// Get the instance name
		const int instance = it.value()["id"];
		const int num_clauses = it.value()["num_clauses"];
		const int num_vars = it.value()["num_variables"];
		const int num_weights = it.value()["num_clauses"];
		map<string, int> m = { {"SEED_ASSIGN", 0} };
		MAXSATConfig c(m);

		// Get the data
		unique_ptr < const vector<double>> data(open_maxsat_benchmark(instance, path_hdf5));
		auto [clauses_ptr, weights_ptr, n_vars] = parse_maxsat(*data);
		unique_ptr<vector<clause_t>> clauses((clauses_ptr));
		unique_ptr<vector<double>> weights((weights_ptr));
		map<const var_id_t, vector<clause_id_t>> var_to_clauses;
		for (int clause_id = 0; clause_id < clauses->size(); clause_id++) {
			for (auto& var : (*clauses)[clause_id]) {
				var_to_clauses[var.first].push_back(clause_id);
			}
		}

		unique_ptr<vector<var_assignment_t>> assignment(improve_maxsat(var_to_clauses, *clauses, *weights, n_vars, c.SEED_ASSIGN.get()));


		// Check that there are as many variables as expected
		if (assignment->size() != num_vars) {
			throw runtime_error("The number of variables in the assignment is not the expected one. Expected " + to_string(num_vars) + " but got " + to_string(assignment->size()));
		}
	}
}