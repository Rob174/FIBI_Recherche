#pragma	once
#include "../../../src/data/get_data.hpp"
#include "../../../libs/json.hpp"
#include <fstream>
#include <filesystem>

using namespace std;
using json = nlohmann::json;
void test_clustering_aloise() {
	const string path_hdf5 = "../../../data/folder/aloise_benchmark/";
	const string path_json = "../../../data/aloise_benchmark.json";

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
		const int instance = it.value()["ID"];
		const int num_dims = it.value()["NUM_DIM"];
		const int num_points = it.value()["NUM_POINTS"];
		map<string,long> *m = new map<string,long>{ };
		ClusteringConfig c(m);

		// Get the data
		unique_ptr<const vector<double>> data(open_clustering(instance, path_hdf5, &c, false));
		delete m;
		// Check that the number of dimensions is correct
		if (c.NUM_DIM.get() != num_dims) {
			throw runtime_error("The number of dimensions is not correct: expected " + to_string(num_dims) + " but got " + to_string(c.NUM_DIM.get()));
		}

		// Check that the number of points is correct
		if (c.NUM_POINTS.get() != num_points)
		{
			throw runtime_error("The number of points is not correct: expected " + to_string(num_points) + " but got " + to_string(c.NUM_POINTS.get()));
		}

		// Check that the number of points is correct
		if (data->size() != num_points * num_dims) {
			throw runtime_error("The number of data is not correct: expected " + to_string(num_points * num_dims) + " elements but got " + to_string(data->size()));
		}
	}

}


void test_clustering_franti() {
	const string path_hdf5 = "../../../data/folder/franti_benchmark/";
	const string path_json = "../../../data/franti_benchmark.json";

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
		const int instance = stoi(it.key());
		const int num_dims = it.value()["NUM_DIM"];
		const int num_points = it.value()["NUM_POINTS"];
		const int num_clust = it.value()["NUM_CLUST"];
		map<string,long> *m = new map<string,long>{ };
		ClusteringConfig c(m);

		// Get the data
		const vector<double>* data = open_clustering(instance, path_hdf5, &c, true);

		// Check that the number of dimensions is correct
		if (c.NUM_DIM.get() != num_dims) {
			throw runtime_error("The number of dimensions is not correct: expected " + to_string(num_dims) + " but got " + to_string(c.NUM_DIM.get()) + " for instance " + to_string(instance));
		}

		// Check that the number of points is correct
		if (c.NUM_POINTS.get() != num_points)
		{
			throw runtime_error("The number of points is not correct: expected " + to_string(num_points) + " but got " + to_string(c.NUM_POINTS.get()) + " for instance " + to_string(instance));
		}

		// Check that the number of clusters is correct
		if (c.NUM_CLUST.get() != num_clust)
		{
			throw runtime_error("The number of clusters is not correct: expected " + to_string(num_clust) + " but got " + to_string(c.NUM_CLUST.get()) + " for instance " + to_string(instance));
		}

		// Check that the number of points is correct
		if (data->size() != num_points * num_dims) {
			throw runtime_error("The number of data is not correct: expected " + to_string(num_points * num_dims) + " elements but got " + to_string(data->size()) + " for instance " + to_string(instance));
		}
		delete m;
	}


}