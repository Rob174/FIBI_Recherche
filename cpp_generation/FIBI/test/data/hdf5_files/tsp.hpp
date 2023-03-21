#pragma	once
#include "../../../src/data/get_data.hpp"
#include "../../../libs/json.hpp"
#include <fstream>
#include <filesystem>

using namespace std;
using json = nlohmann::json;
void test_tsplib() {
	const string path_hdf5 = "../../../data/tsplib.hdf5";
	const string path_json = "../../../data/tsplib.json";

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

	// Iterate over the keys of the json file dict
	for (json::iterator it = j.begin(); it != j.end(); ++it) {
		// Get the key and the value
		string key = it.key();
		// Cast key into int
		const int instance = stoi(key);
		const unsigned int exp_n_points = it.value()["n_pts"];
		map<string, int> m = { {"NUM_DIM",2} };
		TSPConfig c(m);

		// Get the data
		unique_ptr<const vector<double>> data(open_tsplib(instance, path_hdf5, &c));

		// Check that the number of points is correct
		if (data->size() != exp_n_points * 2) {
			throw runtime_error("The number of points is not correct: expected " + to_string(exp_n_points * 2) + " elements but got " + to_string(data->size()));
		}
	}
}