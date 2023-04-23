#pragma once
#include "../../../src/data/get_data.hpp"
#include "../../../libs/json.hpp"
#include <fstream>
#include <filesystem>



void test_maxsat_benchmark() {
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
		map<string,long> *m = new map<string,long>{ };
		MAXSATConfig c(m);

		// Get the data
		unique_ptr<const vector<double>> data(open_maxsat_benchmark(instance, path_hdf5));
		auto [clauses_ptr, weights_ptr, n_vars] = parse_maxsat(*data);
		unique_ptr<vector<clause_t>> clauses(clauses_ptr);
		unique_ptr<vector<double>> weights(weights_ptr);
		delete m;
		// Check number of clauses
		if (num_clauses != clauses->size()) {
			throw runtime_error("Number of clauses does not match for instance " + to_string(instance) + " with expected " + to_string(num_clauses) + " but seen " + to_string(clauses->size()));
		}
		// check number of variables
		if (num_vars != n_vars) {
			throw runtime_error("Number of variables does not match for instance " + to_string(instance) + " with expected " + to_string(num_vars) + " but seen " + to_string(n_vars));
		}
		// check number of weights
		if (num_weights != weights->size()) {
			throw runtime_error("Number of weights does not match for instance " + to_string(instance) + " with expected " + to_string(num_weights) + " but seen " + to_string(weights->size()));
		}
	}
}