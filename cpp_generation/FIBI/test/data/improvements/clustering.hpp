#pragma once
#include <random>
#include <vector>
#include "../../../test/constants.hpp"
#include "../../../src/data/improvements/clustering.hpp"

using namespace std;

/** @brief Test the general coherence of the result of the kmeansPlusPlus function}
* - n_pts_per_clust <-> checked with cluster_assign
* - centroids <-> checked with cluster_assign + points
*/
void test_kmeansPlusPlus() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis_ints(1, 100);
	const int num_points = dis_ints(gen);
	// num_clusters less than num_points
	const int num_clusters = (uniform_int_distribution<> { 2, num_points - 1 })(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = (uniform_int_distribution<> { 2, 256 })(gen);
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> points(uniform_points(num_points, num_dims, seed));
	vector<double> centroids(num_clusters * num_dims, -1);// -1 to detect errors
	vector<int> clusters_assign(num_points, -1);// -1 to detect errors
	vector<int> num_pts_per_clust(num_clusters, -1);// -1 to detect errors
	kmeansPlusPlus(centroids, *points, clusters_assign, num_pts_per_clust, num_dims, seed);
	// Check1: centroid[i] must be equal to the sum of points assigned to cluster i (points)
	// Check2: n_pts_per_clust[i] = how many points are assigned to cluster i (clusters_assign)

	// Check1: check for each cluster, dimension by dimension that the sum of the points coordinate is equal to the centroid
	for (int i = 0; i < num_clusters; i++) {
		for (int j = 0; j < num_dims; j++) {
			double sum = 0;
			vector<double> values;
			for (int k = 0; k < num_points; k++) {
				if (clusters_assign[k] == i) {
					const double v = points->at(num_dims * k + j);
					sum += v;
					values.push_back(v);
				}
			}
			if (abs(centroids[num_dims * i + j] - (sum / num_pts_per_clust[i])) >= EPSILON) {
				string msg = "The centroids " + to_string(i) + " dimension " + to_string(j) + " is not correct: expected " + to_string(sum) + " but got " + to_string(centroids[num_dims * i + j]);
				msg += "\n values: \n";
				int i = 0;
				for (const double v : values) {
					msg += to_string(v);
					if (i < (values.size() - 1)) {
						msg += " + ";
					}
					i++;
				}
				msg += " = " + to_string(sum) + " / " + to_string(num_pts_per_clust[i]) + " = " + to_string(sum / num_pts_per_clust[i]);
				msg += " != " + to_string(centroids[num_dims * i + j]);
				throw runtime_error(msg);
			}
		}
	}

	for (int i = 0; i < num_points; i++) {
		// Check2: remove the point occurence from the cluster
		num_pts_per_clust[clusters_assign[i]]--;
	}
	// Check2: no points should be left
	for (int i = 0; i < num_clusters; i++) {
		if (num_pts_per_clust[i] != 0) {
			throw runtime_error("The number of points per cluster is not correct: expected 0 but got " + to_string(num_pts_per_clust[i]));
		}
	}
}

/** @brief Test the general coherence of the result of the kmeansPlusPlusSuperGlutton function}
* - n_pts_per_clust <-> checked with cluster_assign
* - centroids <-> checked with cluster_assign + points
*/
void test_kmeansPlusPlusSuperGlutton() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis_ints(1, 100);
	const int num_points = (uniform_real_distribution<>{ 3, 100 })(gen);
	// num_clusters less than num_points
	const int num_clusters = (uniform_int_distribution<> { 2, num_points - 1 })(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = (uniform_int_distribution<> { 2, 256 })(gen);
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> points(uniform_points(num_points, num_dims, seed));
	vector<double> centroids(num_clusters * num_dims, -1);// -1 to detect errors
	vector<int> clusters_assign(num_points, -1);// -1 to detect errors
	vector<int> num_pts_per_clust(num_clusters, -1);// -1 to detect errors
	kmeansPlusPlusSuperGlutton(centroids, *points, clusters_assign, num_pts_per_clust, num_dims, seed);
	// Check1: centroid[i] must be equal to the sum of points assigned to cluster i (points)
	// Check2: n_pts_per_clust[i] = how many points are assigned to cluster i (clusters_assign)

	// Check1: check for each cluster, dimension by dimension that the sum of the points coordinate is equal to the centroid
	for (int i = 0; i < num_clusters; i++) {
		for (int j = 0; j < num_dims; j++) {
			double sum = 0;
			vector<double> values;
			for (int k = 0; k < num_points; k++) {
				if (clusters_assign[k] == i) {
					const double v = points->at(num_dims * k + j);
					sum += v;
					values.push_back(v);
				}
			}

			if (abs(centroids[num_dims * i + j] - (sum / num_pts_per_clust[i])) >= EPSILON) {
				string msg = "The centroids " + to_string(i) + " dimension " + to_string(j) + " is not correct: expected " + to_string(sum) + " but got " + to_string(centroids[num_dims * i + j]);
				msg += "\n values: \n";
				int i = 0;
				for (const double v : values) {
					msg += to_string(v);
					if (i < (values.size() - 1)) {
						msg += " + ";
					}
					i++;
				}
				msg += " = " + to_string(sum) + " / " + to_string(num_pts_per_clust[i]) + " = " + to_string(sum / num_pts_per_clust[i]);
				msg += " != " + to_string(centroids[num_dims * i + j]);
				throw runtime_error(msg);
			}
		}
	}

	for (int i = 0; i < num_points; i++) {
		// Check2: remove the point occurence from the cluster
		num_pts_per_clust[clusters_assign[i]]--;
	}
	// Check2: no points should be left
	for (int i = 0; i < num_clusters; i++) {
		if (num_pts_per_clust[i] != 0) {
			throw runtime_error("The number of points per cluster is not correct: expected 0 but got " + to_string(num_pts_per_clust[i]));
		}
	}
}
