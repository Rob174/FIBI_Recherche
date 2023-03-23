#pragma once
#include <map>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include <stdio.h>
#include "../../src/launch/threapool.hpp"
#include "../../src/out/abstract.hpp"
#include "../../src/data/get_data.hpp"

#include <unordered_map>
#include <map>
#include "../../src/factories/tsp.hpp"
#include "../../src/data/constants/tsp.hpp"

using namespace std;
void print_map(const map<string, int>& my_map, int thread_id) {
	// Determine a random number of ms
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, 1000);
	this_thread::sleep_for(chrono::milliseconds(dis(gen)));
	cout << "Thread " << thread_id << ": ";
	for (const auto& kv : my_map) {
		cout << kv.first << " = " << kv.second << ", ";
	}
	cout << endl;
}

/** @brief Test simple thread pool usage
* */
void test_thread_pool() {
	const int num_threads = 4;
	ThreadPool pool(num_threads);
	map<string, int> map;

	for (int i = 0; i < 50; i++) {
		map["a"] = i;
		map["b"] = 2 * i;
		map["c"] = 3 * i;
		pool.submit(print_map, map);
	}
}

void print_map_read_tsplib(const map<string, int>& my_map, int thread_id) {
	const string path_hdf5 = "../../../data/folder/";
	string root_data = "../../../data/folder/";
	// Determine a random number of ms
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, 1000);
	this_thread::sleep_for(chrono::milliseconds(dis(gen)));
	cout << "\tThread " << thread_id << ": ";
	for (const auto& kv : my_map) {
		cout << kv.first << " = " << kv.second << ", ";
	}
	cout << endl;
	TSPConfig cf({ { "SEED_GLOB",0 }, {"SEED_PROBLEM",0}, {"NUM_DIM", 2} });
	unique_ptr<const vector<double>> v(open_tsplib(0, root_data + "tsplib/", &cf));
}
/** @brief Test thread pool with hdf5 file write
* */
void test_thread_pool_hdf5_read() {
	const int num_threads = 4;
	// For each thread remove dataset{thread_id}.hdf5
	for (int i = 0; i < num_threads; i++) {
		remove(("dataset" + to_string(i) + ".hdf5").c_str());
	}
	ThreadPool pool(num_threads);
	map<string, int> map;

	for (int i = 0; i < 50; i++) {
		map["a"] = i;
		map["b"] = 2 * i;
		map["c"] = 3 * i;
		pool.submit(print_map_read_tsplib, map);
	}
}
void print_map_write_tsplib(const map<string, int>& my_map, int thread_id) {
	const string path_hdf5 = "../../../data/folder/";
	string root_data = "../../../data/folder/";
	// Determine a random number of ms
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, 1000);
	this_thread::sleep_for(chrono::milliseconds(dis(gen)));
	cout << "\tThread " << thread_id << ": ";
	vector<pair<string, double>> v;
	for (const auto& kv : my_map) {
		cout << kv.first << " = " << kv.second << ", ";
		v.push_back({ kv.first, kv.second });
	}
	cout << endl;
	TSPConfig cf({ { "SEED_GLOB",my_map.at("a")}, {"SEED_PROBLEM",0}, {"NUM_DIM", 2} });
	save_metadata(my_map.at("a"), v);
}
/** @brief Test thread pool with hdf5 file write
* */
void test_thread_pool_hdf5_write() {
	const int num_threads = 4;
	// For each thread remove dataset{thread_id}.hdf5
	for (int i = 0; i < num_threads; i++) {
		remove(("dataset" + to_string(i) + ".hdf5").c_str());
	}
	ThreadPool pool(num_threads);
	map<string, int> map;

	for (int i = 0; i < 50; i++) {
		map["a"] = i;
		map["b"] = 2 * i;
		map["c"] = 3 * i;
		pool.submit(print_map_write_tsplib, map);
	}
}
void tspfactory_run(const map<string, int>& args, int thread_id) {
	string root_data = "../../../data/folder/";
	string out_data = "../../../data/out/";
	TSPFactory f;
	TSPConfig cf(args);
	f.run(cf, root_data, out_data, args.at("SEED_GLOB") == 0);

	cout << "\x1B[32m \tOK ";
	cf.print();
	cout << "\033[0m " << endl;
}
void test_thread_pool_TSPFactory() {
	const int num_threads = 10;
	ThreadPool pool(num_threads);
	vector<int> range_0_26(26);
	iota(range_0_26.begin(), range_0_26.end(), 0);
	vector<int> range_0_49(49);
	iota(range_0_49.begin(), range_0_49.end(), 0);
	map<int, vector<int>> seeds_problem{
		{0,range_0_26},
		{1,range_0_49},
		{2,range_0_26}
	};
	// map num_towns map for each dataset a function that takes the seed_problem and returns the number of towns
	vector <int> num_towns{ 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 };
	map<int, function<int(int)>> num_towns_map{
		{0, [&num_towns](int seed_problem) {return num_towns.at(seed_problem); }},
		{1, [](int seed_problem) {return -1; }},
		{2, [&num_towns](int seed_problem) {return num_towns.at(seed_problem); }}
	};
	int i = 0;
	for (int dataset = 0; dataset < 3; dataset++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 5; impr++) {
				for (int seed_assign = 0; seed_assign < 1000; seed_assign++) {
					for (int seed_problem : seeds_problem.at(dataset)) {
						map<string, int> args{
							{"DATASET",dataset},
							{"SEED_GLOB",i},
							{"SEED_PROBLEM",seed_problem},
							{"SEED_ASSIGN",seed_assign},
							{"FI_BI",FI_BI},
							{"IMPR",impr},
							{"NUM_TOWNS",num_towns_map.at(dataset)(seed_problem)},
							{"NUM_DIM",2}
						};
						//tspfactory_run(args, 0);
						pool.submit(tspfactory_run, args);
						i++;
					}
				}
			}
		}
	}
}