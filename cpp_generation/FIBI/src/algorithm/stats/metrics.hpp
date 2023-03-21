#pragma once
#include <chrono>
#include <vector>
#include <iostream>
#include <optional>

#include "../../AbstractWritable.hpp"
#include "../../../libs/json.hpp"
#include "utils.h"
#include "../observers/AlgorithmObserver.hpp"
#include "../../data/solution_container/abstract.hpp"

using namespace std;
using json = nlohmann::json;

template <typename T_Swap, typename T_Container>
class Metrics : public AlgorithmObserver<T_Swap, T_Container>
{
public:
	Uninitialized<unsigned long> num_iter;
	Uninitialized<unsigned long> num_iter_glob;
	Uninitialized<double> init_cost;
	Uninitialized<double> final_cost;
	Uninitialized<chrono::steady_clock::time_point> time_start;
	Uninitialized<double> duration

public:
	void on_start(const T_Container& c, const optional<T_Swap>& s = nullopt) override {
		init_cost = c.get_quality_metric();
		final_cost = this->init_cost;
		num_iter = 0;
		num_iter_glob = 0;
		time_start = get_time();
	}
	void on_test_end(const T_Container& c, const optional<double> delta = nullopt, const optional<T_Swap>& s = nullopt) override {
		
	}
	void on_glob_iter_end(const T_Container& c, const optional<double> delta = nullopt, const optional<T_Swap>& s = nullopt) override {
		num_iter_glob++;
	}
	void on_iter_end(const T_Container& c, const optional<T_Swap>& s = nullopt) override {
		num_iter++;
		final_cost = c.get_quality_metric();
	}
	void on_end(const T_Container& c, const optional<T_Swap>& s = nullopt) override {
		final_cost = c.get_quality_metric();
		duration = get_final_time();
	}
	double get_final_time() {
		return chrono::duration_cast<chrono::microseconds>(get_time() - time_start).count();
	};
	chrono::steady_clock::time_point get_time() {
		return chrono::steady_clock::now();
	}
	json get_data() {
		return json{
			{"num_iter":num_iter},
			{"num_iter_glob":num_iter_glob},
			{"init_cost":init_cost},
			{"final_cost":final_cost},
			{"duration":duration}
		};
	}
	void print() {
		cout << "Metrics:";
		json data = get_data();
		for (json::iterator it = data.begin(); it != data.end(); ++it) {
			cout << " " << it.key() << ": " << it.value();
			if (it != --data.end()) {
				cout << ",";
			}
		}
	}
};