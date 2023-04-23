#ifndef METRICS_H
#define METRICS_H
#include <chrono>
#include <vector>
#include <iostream>

#include "AbstractWritable.h"
#include "utils.h"
class Metrics : public AbstractWritable<double>
{
public:
    unsigned long num_iter;
    unsigned long num_iter_glob;
    double init_cost;
    double final_cost;
    std::chrono::steady_clock::time_point time_start;
    double duration;

public:
    double set_final_time();
    std::chrono::steady_clock::time_point get_time();

    std::vector<std::string> get_group();
    std::vector<std::vector<double>> get_data();
    std::vector<std::string> get_data_names();
    void print();
};
#endif