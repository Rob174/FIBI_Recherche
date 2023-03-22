#pragma once
#include <chrono>
#include <vector>
#include <iostream>
#include <optional>

#include "../../../libs/json.hpp"
#include "../observers/AlgorithmObserver.hpp"
#include "../../data/solution_container/abstract.hpp"

using namespace std;
using json = nlohmann::json;

template <typename T_Swap, typename T_Container>
class Metrics : public AlgorithmObserver<T_Swap, T_Container>
{
public:
	unsigned long num_iter;
	unsigned long num_iter_glob;
	double init_cost;
	double final_cost;
	chrono::steady_clock::time_point time_start;
	double duration;

public:
	void on_start(const T_Container& c, const optional<T_Swap>& s = nullopt) override {
		init_cost = c.get_quality_metric();
		final_cost = this->init_cost;
		num_iter = 0;
		num_iter_glob = 0;
		time_start = get_time();
	};
	void on_test_end(const T_Container& c, const optional<double>& delta = nullopt, const optional<T_Swap>& s = nullopt) override {
		return;
	};
	void on_glob_iter_end(const T_Container& c, const optional<double>& delta = nullopt, const optional<T_Swap>& s = nullopt) override {
		num_iter_glob++;
	};
	void on_iter_end(const T_Container& c, const optional<T_Swap>& s = nullopt) override {
		num_iter++;
		final_cost = c.get_quality_metric();
	};
	void on_end(const T_Container& c, const optional<T_Swap>& s = nullopt) override {
		final_cost = c.get_quality_metric();
		duration = get_final_time();
	};
	double get_final_time() {
		return chrono::duration_cast<chrono::microseconds>(get_time() - time_start).count();
	};
	chrono::steady_clock::time_point get_time() {
		return chrono::steady_clock::now();
	};
	vector<pair<string, double>> get_data() const {
		return vector<pair<string, double>>{
			{"num_iter",(double)num_iter},
			{"num_iter_glob",(double)num_iter_glob},
			{"init_cost",(double)init_cost},
			{"final_cost",(double)final_cost},
			{"duration",(double)duration}
		};
	};
	void print() {
		cout << "Metrics:";
		json data = get_data();
		for (json::iterator it = data.begin(); it != data.end(); ++it) {
			cout << " " << it.key() << ": " << it.value();
			if (it != --data.end()) {
				cout << ",";
			}
		}
	};
};