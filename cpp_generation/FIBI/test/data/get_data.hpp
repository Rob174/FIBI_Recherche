#pragma once

#include "../../src/data/get_data.hpp"
#include "../../libs/json.hpp"
#include <random>
#include <limits>
#include <iostream>
#include <sstream>
#include <exception>
#include "../../test/constants.hpp"

using json = nlohmann::json;
using namespace std;

/** 
* * @brief Test the uniform_points function general functionalities
* * - Check that the number of points is correct
* */
void test_uniform_points() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_ints(1, max_int);
    const int num_points = dis_ints(gen);
    const int num_dims = dis_ints(gen);
    const int seed = dis_ints(gen)-1;
    unique_ptr<const vector<double>> points(uniform_points(num_points, num_dims, seed));
    // Check that the number of points is correct
    if (points->size() != num_points * num_dims) {
        throw runtime_error("The number of points is not correct: expected " + to_string(num_points * num_dims) + " but got " + to_string(points->size()));
    }
}
/**
* * @brief Test the uniform_points repetability
* * - Check that calling it twice with the same seed returns the same points
* */
void test_uniform_points_repetability() {
    const int seed_rep = 0;
    random_device rd;
    mt19937 gen(seed_rep);
    uniform_real_distribution<> dis_ints(1, max_int);
    const int num_points = dis_ints(gen);
    const int num_dims = dis_ints(gen);
    const int seed = 0;
    vector<vector<double>> values_found;
    const int num_tests = 3;
    for (int i = 0; i < num_tests; i++) {
        const vector<double> *points = uniform_points(num_points, num_dims, seed);
        values_found.push_back(*points);
    }
    // Check that all points at the index are the same
    for (int i_pt = 0; i_pt < num_points * num_dims; i_pt++) {
        for (int i_vect = 0; i_vect < values_found.size(); i_vect++) {
            if (!(values_found[i_vect][i_pt] == values_found[0][i_pt])) {
                throw runtime_error("Repeatability results failed: expected all points at index " + to_string(i_pt) + " to be " + to_string(values_found[0][i_pt]) + ". Ok up to vector of index " + to_string(i_vect) + " with value " + to_string(values_found[i_vect][i_pt]));
            }
        }
    }
}
/** @brief Test the test_normal_points function:
* * - Check that the number of points is correct
* */
void test_normal_points() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_ints(1, max_int);
    const int num_points = dis_ints(gen);
    const int num_dims = dis_ints(gen);
    const int seed = dis_ints(gen)-1;
    unique_ptr < const vector<double>>points(normal_points(num_points, num_dims, seed));
    // Check that the number of points is correct
    if (points->size() != num_points * num_dims) {
        throw runtime_error("The number of points is not correct: expected " + to_string(num_points * num_dims) + " but got " + to_string(points->size()));
    }
}
/**
 * @brief Test the normal_points repetability
 * - Check that calling it twice with the same seed returns the same points
 */
void test_normal_points_repetability() {
    const int seed_rep = 0;
    random_device rd;
    mt19937 gen(seed_rep);
    uniform_real_distribution<> dis_ints(1, max_int);
    const int num_points = dis_ints(gen);
    const int num_dims = dis_ints(gen);
    const int seed = 0;
    vector<vector<double>> values_found;
    const int num_tests = 3;
    for (int i = 0; i < num_tests; i++) {
        const vector<double>* points = normal_points(num_points, num_dims, seed);
        values_found.push_back(*points);
    }
    // Check that all points at the index are the same
    for (int i_pt = 0; i_pt < num_points * num_dims; i_pt++) {
        for (int i_vect = 0; i_vect < values_found.size(); i_vect++) {
            if (!(values_found[i_vect][i_pt] == values_found[0][i_pt])) {
                throw runtime_error("Repeatability results failed: expected all points at index " + to_string(i_pt) + " to be " + to_string(values_found[0][i_pt]) + ". Ok up to vector of index " + to_string(i_vect) + " with value " + to_string(values_found[i_vect][i_pt]));
            }
        }
    }
}
/**
/** @brief Test the test_normal_points function:
* * - Check that the number of clusters is correct
 */
void test_random_clust() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_ints(1, max_int);
    const int num_clust = dis_ints(gen);
    const int num_points = dis_ints(gen);
    const int seed = dis_ints(gen)-1;
    unique_ptr < const vector<int> >clust(random_clust(num_clust, num_points, seed));
    // Check that the number of points is correct
    if (clust->size() != num_points) {
        throw runtime_error("The number of clusters is not correct: expected " + to_string(num_points) + " but got " + to_string(clust->size()));
    }
}
/**
 * @brief Test the repeatability of the random_clust function
 *
 * - Check that calling the function twice with the same seed returns the same clustering
 */
void test_random_clust_repeatability() {
    const int seed_rep = 0;
    random_device rd;
    mt19937 gen(seed_rep);
    uniform_real_distribution<> dis_ints(1, max_int);
    const int num_clust = dis_ints(gen);
    const int num_points = dis_ints(gen);
    const int seed = 0;
    vector<vector<int>> values_found;
    const int num_tests = 3;
    for (int i = 0; i < num_tests; i++) {
        const vector<int>* clust = random_clust(num_clust, num_points, seed);
        values_found.push_back(*clust);
    }
    // Check that all points at the index are the same
    for (int i_pt = 0; i_pt < num_points; i_pt++) {
        for (int i_vect = 0; i_vect < values_found.size(); i_vect++) {
            if (!(values_found[i_vect][i_pt] == values_found[0][i_pt])) {
                throw runtime_error("Repeatability results failed: expected all clusters at point index " + to_string(i_pt) + " to be " + to_string(values_found[0][i_pt]) + ". Ok up to vector of index " + to_string(i_vect) + " with value " + to_string(values_found[i_vect][i_pt]));
            }
        }
    }
}