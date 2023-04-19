#pragma once
#include "../../../src/data/solution_container/tsp.hpp"
#include "../../../src/data/get_data.hpp"
#include "../../../test/constants.hpp"
#include <utility>
#include "../../../libs/json.hpp"
#include <fstream>
#include <filesystem>

using namespace std;

/**
* @brief Test the DistanceMatrix object:
* - Check for a toy example that the distances are correct
*/
void test_DistanceMatrix() {
	const string path_json = "../../../data/test_instance1.json";
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
	const vector<double> towns = j["towns"];
	const vector<vector<double>> exp_distances = j["distances"];
	DistanceMatrix m(towns.size() / 2, 2, towns);
	// Check distances
	for (int i = 0; i < towns.size() / 2; i++) {
		for (int j = 0; j < towns.size() / 2; j++) {
			if (m.get(i, j) != exp_distances[i][j]) {
				throw runtime_error("The distance between towns " + to_string(i) + " and " + to_string(j) + " is not correct: expected " + to_string(exp_distances[i][j]) + " but got " + to_string(m.get(i, j)));
			}
		}
	}
}