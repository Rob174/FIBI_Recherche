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
    const int num_points = 10004;
    const int num_dims = 2;
    const int num_clust = 5;
    const int seed = 0;
    auto [points, assignements, clust_centers, clust_stds] = normal_points(num_points, num_dims, num_clust, seed);
    // cout the centers and std in clust_center pair
    for (int i_clust = 0; i_clust < num_clust; i_clust++) {
        cout << "Cluster " << i_clust << " center: ";
        for (int i_dim = 0; i_dim < num_dims; i_dim++) {
            cout << (*clust_centers)[i_clust * num_dims + i_dim];
            if (i_dim < num_dims - 1) {
				cout << ", ";
			}
		}
        cout << " std: " << (*clust_stds)[i_clust] << endl;
	}
    // Check that the number of points is correct
    if (points->size() != num_points * num_dims) {
        throw runtime_error("The number of points is not correct: expected " + to_string(num_points * num_dims) + " but got " + to_string(points->size()));
    }
    // Check that the number of assignements is correct
    if (assignements->size() != num_points) {
		throw runtime_error("The number of assignements is not correct: expected " + to_string(num_points) + " but got " + to_string(assignements->size()));
	}
    // Check the number of points per cluster (equal repartion: num_points / num_clust with a tolerance of 1)
    map<int,int> assignements_count = {};
    for (int i_pt = 0; i_pt < num_points; i_pt++) {
        if (assignements_count.find(assignements->at(i_pt)) == assignements_count.end()) {
			assignements_count[assignements->at(i_pt)] = 0;
		}
		assignements_count[assignements->at(i_pt)]++;
	}
    // Check that the number of points per cluster is correct
    string error_txt = "";
    bool error = false;
    cout << "Total of " << num_points << " points in " << num_clust << " clusters so, " << num_points / num_clust << " points per cluster with remaining " << num_points % num_clust << " points" << endl;
    for (auto& [assign, assign_count] : assignements_count) {
        cout << "Observed " << assign_count << " points for cluster " << assign << " expected " << num_points / num_clust << " +/- 1" << endl;
        if (abs(assign_count - num_points / num_clust) != 0) {
            cout << "Difference of " << assign_count - num_points / num_clust << " from the expected number of points" << endl;
        }
        if (abs(assign_count - num_points / num_clust) > 1) {
            error_txt = "The number of points per cluster is not correct: expected " + to_string(num_points / num_clust) + " but got " + to_string(assign_count);
            error = true;
		}
	}
    if (error) {
		throw runtime_error(error_txt);
    }

    // Make a call to python, import plotly express and plot the points with their cluster assignements. Set a title with the number of points, dimensions and clusters
    stringstream ss;
    ss << "import plotly.express as px" << endl;
    ss << "import pandas as pd" << endl;
    ss << "df = pd.DataFrame({" << endl;
    for (int i_dim = 0; i_dim < num_dims; i_dim++) {
		ss << "'dim" << i_dim << "': [";
        for (int i_pt = 0; i_pt < num_points; i_pt++) {
			ss << points->at(i_pt * num_dims + i_dim);
            if (i_pt < num_points - 1) {
				ss << ", ";
			}
		}
		ss << "]";
        if (i_dim < num_dims - 1) {
			ss << ",";
		}
	}

    // Add assignements
    ss << ", 'assignements': [";
    for (int i_pt = 0; i_pt < num_points; i_pt++) {
        ss << "'" << assignements->at(i_pt) << "'";
        if (i_pt < num_points - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    ss << "})" << endl;
    // Make one plot per assignement value on the same figure
    ss << "fig = px.scatter(df, x='dim0', y='dim1', color='assignements', title='";
    ss << "num_points: " << num_points << ", num_dims: " << num_dims << ", num_clust: " << num_clust << "')" << endl;
    
    ss << "fig.show()" << endl;

	// Write the python script
	ofstream file;
	file.open("test_normal_points.py");
	file << ss.str();
	file.close();
	// Call python
	int r = system("pip install plotly&&python test_normal_points.py");
    if (r != 0) {
		throw runtime_error("Error calling python");
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
    const int num_clust = dis_ints(gen);
    const int seed = 0;
    vector<vector<double>> values_found;
    const int num_tests = 3;
    for (int i = 0; i < num_tests; i++) {
        const vector<double>* points = std::get<0>(normal_points(num_points, num_dims, num_clust, seed));
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