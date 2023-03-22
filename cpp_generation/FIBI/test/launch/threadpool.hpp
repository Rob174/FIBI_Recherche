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
	TSPFactory f;
	TSPConfig cf(args);
	f.run(cf, true, root_data, args.at("SEED_GLOB") == 0);

	cout << "\x1B[32m \tOK ";
	cf.print();
	cout << "\033[0m " << endl;
}
void test_thread_pool_TSPFactory() {
	const int num_threads = 4;
	ThreadPool pool(num_threads);
	int i = 0;
	for (int dataset = 0; dataset < 3; dataset++) {
		for (int FI_BI = 0; FI_BI < 2; FI_BI++) {
			for (int impr = 0; impr < 2; impr++) {
				map<string, int> args{
					{"DATASET",dataset},
					{"SEED_GLOB",i},
					{"SEED_PROBLEM",0},
					{"SEED_ASSIGN",0},
					{"FI_BI",FI_BI},
					{"IMPR",impr},
					{"NUM_TOWNS",20},
					{"NUM_DIM",2}
				};
				pool.submit(tspfactory_run, args);
				i++;
			}
		}
	}
}