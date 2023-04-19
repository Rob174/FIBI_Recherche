#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include "../../../src/data/solution_container/clustering.hpp"
#include "../../constants.hpp"
#include "../../../src/data/get_data.hpp"

using namespace std;

/**
* @brief Test the compute_quality_metric method of ClusteringSolutionContainer:
* - Toy example
		B (0,3)
		|                         (20,1)
		|3                       A
	 5  |   5                 5  |1 5
  B-----O-----B     (15,0) A-----O-----A (25,0)
(-5,0)  |   (5,0)                |1
		|3                       A
		|                         (20,-1)
		B (0,-3)
* - The quality metric is the sum of the squared distances between each points and its centroid.
* - Here it is: 3² + 3² + 5² + 5² + 5² + 5² + 1² + 1² = 9 x 2 + 25 x 4 + 1 x 2 = 18 + 100 + 2 = 120
*/
void test_ClusteringSolutionContainer_compute_quality_metric_1() {
	const int num_clusters = 2;
	const int num_dims = 2;
	const int num_points = 8;
	vector<double> points = {
		0, 3, // B
		0, -3, // B
		-5, 0, // B
		5, 0, // B
		15, 0, // A
		25, 0, // A
		20, 1, // A
		20, -1 // A
	};
	vector<int> cluster_ids = {
		0, 0, 0, 0, 1, 1, 1, 1
	};
	ClusteringConfig conf({ {"NUM_CLUST", num_clusters}, {"NUM_DIM", num_dims}, {"NUM_POINTS",num_points} });
	ClusteringSolutionContainer c(points, cluster_ids, conf);
	const double quality = c.compute_quality_metric();
	if (abs(quality - 120) >= EPSILON) {
		throw runtime_error("The quality metric is not correct: expected 120 but got " + to_string(quality));
	}
}

/**
* @brief Test the test_flip method of ClusteringSolutionContainer:
* - Toy example: target point is Ac initially in cluster B but will be moved to cluster A
		B (0,3)
		|                         (20,1)
		|3                       A
	 5  |   5                 5  |1 5
  B-----O-----B     (15,0)Ac-----O-----A (25,0)
(-5,0)  |   (5,0)                |1
		|3                       A
		|                         (20,-1)
		B (0,-3)
* - The quality metric is the sum of the squared distances between each points and its centroid.
* - Here it is: 3² + 3² + 5² + 5² + 5² + 5² + 1² + 1² = 9 x 2 + 25 x 4 + 1 x 2 = 18 + 100 + 2 = 120
* - Changing Ac from cluster B to A will lead to a cost of 120
*/
void test_ClusteringSolutionContainer_test_flip() {
	const int num_clusters = 2;
	const int num_dims = 2;
	const int num_points = 8;
	vector<double> points = {
		0, 3, // B
		0, -3, // B
		-5, 0, // B
		5, 0, // B
		15, 0, // Ac
		25, 0, // A
		20, 1, // A
		20, -1 // A
	};
	vector<int> cluster_ids = {
		0, 0, 0, 0, 0, 1, 1, 1
	};
	ClusteringConfig conf({ {"NUM_CLUST", num_clusters}, {"NUM_DIM", num_dims}, {"NUM_POINTS",num_points} });
	ClusteringSolutionContainer c(points, cluster_ids, conf);
	const double quality = c.compute_quality_metric();
	ClusteringSwap s(4, 0, 1); // 0: B ; 1: A
	const double delta = c.test_flip(s);
	const double new_quality = delta + quality;
	c.flip(s, delta);
	const double cmpt_quality = c.compute_quality_metric();
	if (abs(new_quality - 120) >= EPSILON) {
		throw runtime_error("The quality metric is not correct: expected 120 but got " + to_string(new_quality) + " with computed new quality " + to_string(cmpt_quality));
	}

}

/**
* @brief Test the flip method of ClusteringSolutionContainer:
* - With any solution, flipping two times the same point out and in a cluster must lead to the same original cost
*/
void test_ClusteringSolutionContainer_compute_quality_metric_2() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis_ints(1, max_int);
	const int num_points = dis_ints(gen);
	const int num_clust = (uniform_int_distribution<>{ 1, 256 })(gen);
	// Limit the tests to 2D for now (only usage in the codebase)
	const int num_dims = (uniform_int_distribution<>{ 2, 1024 })(gen);
	const int seed = dis_ints(gen) - 1;
	unique_ptr<const vector<double>> points(uniform_points(num_points, num_dims, seed));
	unique_ptr<vector<int>> cluster_ids(random_clust(num_clust, num_points, seed));
	ClusteringConfig conf({ {"NUM_CLUST", num_clust}, {"NUM_DIM", num_dims}, {"NUM_POINTS",num_points} });
	ClusteringSolutionContainer<false> c(*points, *cluster_ids, conf);
	const double quality = c.compute_quality_metric();
	for (int i = 0; i < 100; i++) {
		const int point_id = (uniform_int_distribution<>{ 0, num_points - 1 })(gen);
		const int cluster_src = (uniform_int_distribution<>{ 0, num_clust - 1 })(gen);
		const int cluster_dst = (uniform_int_distribution<>{ 0, num_clust - 1 })(gen);
		ClusteringSwap s1(point_id, cluster_src, cluster_dst);
		const double delta1 = c.test_flip(s1);
		c.flip(s1, delta1);
		ClusteringSwap s2(point_id, cluster_dst, cluster_src);
		const double delta2 = c.test_flip(s2);
		c.flip(s2, delta2);
		if ((quality - c.compute_quality_metric()) >= EPSILON) {
			throw runtime_error("The quality metric is not correct: expected " + to_string(quality) + " but got " + to_string(c.compute_quality_metric()));
		}
	}
}