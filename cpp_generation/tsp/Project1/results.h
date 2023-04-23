#pragma once
#include <chrono>
#include <iostream>
#include "constants.h"
#include <iomanip>
#include <vector>
using hrclk_t = std::chrono::high_resolution_clock;
using timepoint_t = std::chrono::high_resolution_clock::time_point;
class Result {
private:
	Config* config;
	unsigned long num_iter;
	unsigned long num_iter_glob;
	double init_cost;
	double final_cost;
	double final_length;
	std::chrono::steady_clock::time_point time_start;
	double duration;
	double* points;
	int* init_tour;
public:
	Result(Config* config) : config(config), final_length(-1) {
		this->points = new double[config->NUM_DIM * config->NUM_POINTS];
		this->init_tour = new int[config->NUM_POINTS];
	};
	Result(Config* config, int num_iter, int num_iter_glob, double init_cost, double final_cost, double duration) :
		config(config), num_iter(num_iter), num_iter_glob(num_iter_glob), init_cost(init_cost), final_cost(final_cost), duration(duration), final_length(-1) {
		this->points = new double[config->NUM_DIM * config->NUM_POINTS];
		this->init_tour = new int[config->NUM_POINTS];
	};
	~Result() {
		delete[] this->points;
		delete[] this->init_tour;
	}
	void set_next_iter();
	void set_next_iter_glob();
	void set_final_length(double final_length);
	void set_final_cost(double final_cost);
	void set_init_cost(double init_cost);
	void set_time_start();
	void set_time_end();
	void set_init_tour(double*points,int* tour);
	double* get_points();
	int* get_init_tour();
	double get_time_elapsed();
	void print_results();
	std::vector<double>* get_result();
	Config* get_config() { return this->config; }
};
