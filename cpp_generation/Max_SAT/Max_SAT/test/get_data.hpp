#pragma once

#include "../src/get_data.hpp"
#include "../libs/json.hpp"
#include <random>
#include <limits>
#include <iostream>
#include <sstream>
#include <exception>

using json = nlohmann::json;
using namespace std;
void test_uniform_points() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_ints(1, std::numeric_limits<int>::max());
    const int num_points = dis_ints(gen);
    const int num_dims = dis_ints(gen);
    const int seed = dis_ints(gen)-1;
    vector<double> points = uniform_points(num_points, num_dims, seed);
    // Check that the number of points is correct
    if (points.size() != num_points * num_dims) {
        throw runtime_error("The number of points is not correct: expected " + to_string(num_points * num_dims) + " but got " + to_string(points.size()));
    }
}
void test_normal_points() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_ints(1, std::numeric_limits<int>::max());
    const int num_points = dis_ints(gen);
    const int num_dims = dis_ints(gen);
    const int seed = dis_ints(gen)-1;
    vector<double> points = normal_points(num_points, num_dims, seed);
    // Check that the number of points is correct
    if (points.size() != num_points * num_dims) {
        throw runtime_error("The number of points is not correct: expected " + to_string(num_points * num_dims) + " but got " + to_string(points.size()));
    }
}
void test_random_clust() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_ints(1, std::numeric_limits<int>::max());
    const int num_clust = dis_ints(gen);
    const int num_points = dis_ints(gen);
    const int seed = dis_ints(gen)-1;
    vector<int> clust = random_clust(num_clust, num_points, seed);
    // Check that the number of points is correct
    if (clust.size() != num_points) {
        throw runtime_error("The number of clusters is not correct: expected " + to_string(num_points) + " but got " + to_string(clust.size()));
    }
}