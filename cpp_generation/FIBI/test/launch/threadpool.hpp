#pragma once
#include <map>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include <stdio.h>
#include "../../src/launch/threapool.hpp"
#include "../../src/out/abstract.hpp"

void print_map(const map<string, int>& my_map, int thread_id) {
	// Determine a random number of ms
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, 1000);
	this_thread::sleep_for(chrono::milliseconds(dis(gen)));
	cout << "Thread " << thread_id << ": ";
	vector<pair<string, double> > data;
	for (const auto& kv : my_map) {
		cout << kv.first << " = " << kv.second << ", ";
		data.push_back(make_pair(kv.first, kv.second));
	}
	cout << endl;

	save_metadata(my_map.at("a"), data, "dataset" + to_string(thread_id) + ".hdf5");
}

/** @brief Test simple thread pool usage
* */
void test_thread_pool() {
	const int num_threads = 4;
	ThreadPool pool(num_threads);
	map<string, int> map;

	for (int i = 0; i < 250; i++) {
		map["a"] = i;
		map["b"] = 2 * i;
		map["c"] = 3 * i;
		pool.submit(print_map, map);
	}
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

	for (int i = 0; i < 250; i++) {
		map["a"] = i;
		map["b"] = 2 * i;
		map["c"] = 3 * i;
		pool.submit(print_map, map);
	}
}