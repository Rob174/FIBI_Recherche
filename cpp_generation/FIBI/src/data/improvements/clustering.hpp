#pragma once
#include <vector>
#include <random>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <random>
using namespace std;



/** @brief  Assign each point to the nearest cluster }
 * @param[out] centroids: centroids of the clusters
 * @param[in] points: points to be assigned to a cluster
 * @param[out] cluster_assign: vector of size num_points containing the cluster to which each point is assigned
 * @param[out] n_pts_per_clust: vector of size num_clust containing the number of points in each cluster
 * @param num_dims: dimension of the points
 * @return true if the assignment changed, false otherwise

*/
bool affectToNearestCluster(vector<double>& centroids, const vector<double>& points, vector<int>& cluster_assign, vector<int>& n_pts_per_clust, const int num_dims)
{
	const int num_clust = centroids.size() / num_dims;
	const int num_points = points.size() / num_dims;
	for (int i = 0; i < num_clust; i++)
	{
		n_pts_per_clust[i] = 0;
	}
	// Update affectations and count of points per cluster
	for (int i = 0; i < num_points; i++)
	{
		const double* c_points = &(points.data()[i * num_dims]);
		double minDist = 1e100;
		int chosenClust = -1;
		for (int c = 0; c < num_clust; c++)
		{
			const double* c_coords = &(centroids.data()[c * num_dims]);
			const double d = dist(c_coords, c_points, num_dims);
			if (d < minDist)
			{
				minDist = d;
				chosenClust = c;
			}
		}
		cluster_assign[i] = chosenClust;
		n_pts_per_clust[chosenClust]++;
	}
	double* cpyCentroids = new double[num_clust * num_dims];
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		cpyCentroids[i] = centroids[i];
		centroids[i] = 0;
	}
	// Compute center of gravities (clusters centers)
	for (int i = 0; i < num_points * num_dims; i++)
	{
		const double point_coord = points[i];
		const int cluster = cluster_assign[i / num_dims];
		centroids[cluster * num_dims + i % num_dims] += point_coord;
	}
	bool oneCentHasChanged = false;
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] /= n_pts_per_clust[i / num_dims];
		if (abs(centroids[i] - cpyCentroids[i]) > EPSILON)
		{
			oneCentHasChanged = true;
		}
	}
	delete[] cpyCentroids;
	return oneCentHasChanged;
}

/** @brief  Compute the kmeans centroids }
* * @param[out] centroids: centroids of the clusters
* * @param[in] points: points to be assigned to a cluster
* * @param[out] cluster_assign: vector of size num_points containing the cluster to which each point is assigned
* * @param[out] n_pts_per_clust: vector of size num_clust containing the number of points in each cluster
* * @param num_dims: dimension of the points
*/
void compKmeansCentroids(vector<double>& centroids, const vector<double>& points, vector<int>& cluster_assign, vector<int>& n_pts_per_clust, const int num_dims)
{
	bool changed = true;
	while (changed)
	{
		changed = affectToNearestCluster(centroids, points, cluster_assign, n_pts_per_clust, num_dims);
	}
}
/** @brief  Compute the distance between the centroids (up to num_centroids) and the points (up to num_points) and store it in the distance matrix
* * @param[out] dist_matrix, the distance matrix to update
* * @param[in] centroids, the centroids of the clusters
* * @param[in] points, the points to be assigned to a cluster
* */
void update_dist_matrix(vector<double>& dist_matrix, const vector<double>& centroids, const vector<double>& points, const int num_centroids, const int num_dims, const int num_points) {
	// As the previous centroids are fixed, we only need to compute the distance with the new centroid
	for (int i = num_centroids - 1; i < num_centroids; i++)
	{
		for (int j = 0; j < num_points; j++)
		{
			dist_matrix[i * num_points + j] = dist(
				&(centroids.data()[i * num_dims]),
				&(points.data()[j * num_dims]),
				num_dims
			);
		}
	}
}
/** @brief  Compute the centroids and the number of points per cluster given points coordinates and cluster assignments }
* * @param[in] points: points to be assigned to a cluster
* * @param[in] cluster_assign: vector of size num_points containing the cluster to which each point is assigned
* * @param[in] num_dim: dimension of the points
* * @param[in] num_clust: number of clusters
* * @return a the centroids and the number of points per cluster a vectors pointers
* */
pair < vector<double>*, vector<int>*> cmpt_centroids_and_npts_per_clust(const vector<double>& points, const vector<int>& cluster_assign, const int num_dim, const int num_clust) {
	vector<double>* centroids = new vector<double>(num_clust * num_dim, 0);
	unordered_map<int, int> n_pts_per_clust_set;
	for (int i = 0; i < cluster_assign.size(); i++) {
		const int clust = cluster_assign[i];
		if (n_pts_per_clust_set.find(clust) == n_pts_per_clust_set.end()) {
			n_pts_per_clust_set[clust] = 0;
		}
		n_pts_per_clust_set[clust]++;
		for (int j = 0; j < num_dim; j++) {
			(*centroids)[clust * num_dim + j] += points[i * num_dim + j];
		}
	}
	vector<int>* n_pts_per_clust = new vector<int>(num_clust, -1);
	// Fill the n_pts_per_clust vector with the number of points per cluster of the set
	for (auto it = n_pts_per_clust_set.begin(); it != n_pts_per_clust_set.end(); it++) {
		(*n_pts_per_clust)[it->first] = it->second;
	}
	return make_pair(centroids, n_pts_per_clust);
}
/** @brief *  This function implements the kmeans++ algorithm to find an initial solution for kmeans.
 *  The algorithm is described in the paper:
 * https://theory.stanford.edu/~sergei/papers/kMeansPP-soda.pdf
 *  Arthur, D., & Vassilvitskii, S. (2007). k-means++: The advantages of careful seeding. Proceedings of the eighteenth annual ACM-SIAM symposium on Discrete algorithms, 1027-1035.
 * }
 * @param centroids[out], the centroids found by the algorithm
 * @param points[in], the points that are closest to each centroid
 * @param cluster_assign[out], the cluster assignment of each point
 * @param n_pts_per_clust[out], the number of points in each cluster
 * @param num_dims, the number of dimensions of the point
 * @param seed, the seed for the random number generator
 *
*/
void init_clustering_greedy_randomized(vector<double>& centroids, const vector<double>& points, vector<int>& cluster_assign, vector<int>& n_pts_per_clust, const int num_dims, const int seed)
{
	const double GRID_MAX = 100;
	const int num_clust = centroids.size() / num_dims;
	const int num_points = points.size() / num_dims;
	mt19937 gen_dist(seed);
	uniform_real_distribution<> dis_choice(0, GRID_MAX);
	// "D(x) denote the shortest distance from a data point to the closest center we have already chosen."
	//" Take one center c1, chosen uniformly at random from X"
	for (int i = 0; i < num_dims; i++)
	{
		centroids[i] = dis_choice(gen_dist);
	}
	int num_centroids = 1;
	vector<double> dist_matrix(num_clust * num_points);
	for (int i = 0; i < num_clust; i++)
	{
		for (int j = 0; j < num_points; j++)
		{
			dist_matrix[i * num_points + j] = 1e100;
		}
	}

	while (num_centroids < num_clust)
	{
		// Update distance matrix
		update_dist_matrix(dist_matrix, centroids, points, num_centroids, num_dims, num_points);
		// Assign point to closest centroid
		// At the same time, 1. Create weights for random discrete distribution according to formula:
		vector<double> weights;
		for (int i = 0; i < num_points; i++)
		{
			// find closest centroid of point
			double closest_dist = 1e100;
			int closest_clust = -1;
			for (int j = 0; j < num_clust; j++)
			{
				if (dist_matrix[j * num_points + i] < closest_dist)
				{
					closest_dist = dist_matrix[j * num_points + i];
					closest_clust = j;
				}
			}
			cluster_assign[i] = closest_clust;
			weights.push_back(closest_dist);
		}
		// Find new centroid
		// 2. Find new centroid using random discrete distribution
		discrete_distribution<int> distribution(weights.begin(), weights.end());
		const int chosen_point = distribution(gen_dist);
		const double* point = &(points.data()[chosen_point * num_dims]);
		const int start = num_centroids * num_dims;
		for (int i = 0; i < num_dims; i++)
		{
			centroids[start + i] = point[i];
		}
		num_centroids++;
	}

	compKmeansCentroids(centroids, points, cluster_assign, n_pts_per_clust, num_dims);

	// Manage case where there are empty clusters
	// Affect the farthest points to the empty clusters

	// 1. Find then umber of required points num_pts_to_fill
	vector<int> empty_clusters;

	for (int i = 0; i < num_clust; i++)
	{
		if (n_pts_per_clust[i] == 0)
		{
			empty_clusters.push_back(i);
		}
	}
	const int num_pts_to_fill = empty_clusters.size();
	// 2. Find the num_pts_to_fill farthest away points
	// 2.1 Compute dist to centroids
	vector<pair<double, int>> d_to_clust;
	for (int i = 0; i < num_points; i++)
	{
		const double* point = &(points.data()[i * num_dims]);
		const int cluster = cluster_assign[i];
		const double* clust_coords = &(centroids.data()[cluster * num_dims]);
		d_to_clust.push_back(make_pair(dist(point, clust_coords, num_dims), i));
	}
	partial_sort(d_to_clust.rbegin(), d_to_clust.rbegin() + num_pts_to_fill, d_to_clust.rend(), greater{});
	for (int i = 0; i < num_pts_to_fill; i++)
	{
		auto& [d, idx_pt] = d_to_clust[i];
		const int origClusterPt = cluster_assign[idx_pt];
		const int clustToFill = empty_clusters[i];
		cluster_assign[idx_pt] = clustToFill;
		n_pts_per_clust[origClusterPt]--;
		n_pts_per_clust[clustToFill]++;
	}
	// Recompute centers of gravity
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] = 0;
	}
	for (int i = 0; i < num_points * num_dims; i++)
	{
		const double point_coord = points[i];
		const int cluster = cluster_assign[i / num_dims];
		centroids[cluster * num_dims + i % num_dims] += point_coord;
	}
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] /= n_pts_per_clust[i / num_dims];
	}
}


/** @brief * Kmeans++ super glutton: instead of sampling the next centroid using a random distribution, the closest point is used
	* @param[in] points: vector of points to cluster
	* @param[in] num_dims: number of dimensions of points
	* @param[in] num_clust: number of clusters
	* @param[in] seed: seed for random number generator
	* @param[out] cluster_assign: vector of cluster assignments
	* @param[out] n_pts_per_clust: vector of number of points per cluster
	* @param[out] centroids: vector of centroids
	*
*/
void init_clustering_greedy(vector<double>& centroids, const vector<double>& points, vector<int>& cluster_assign, vector<int>& n_pts_per_clust, const int num_dims, const int seed)
{
	const double GRID_MAX = 100;
	const int num_clust = centroids.size() / num_dims;
	const int num_points = points.size() / num_dims;
	mt19937 gen_dist(seed);
	uniform_real_distribution<> dis_choice(0, GRID_MAX);
	// "D(x) denote the shortest distance from a data point to the closest center we have already chosen."
	//" Take one center c1, chosen uniformly at random from X"
	for (int i = 0; i < num_dims; i++)
	{
		centroids[i] = dis_choice(gen_dist);
	}
	int num_centroids = 1;

	vector<double> dist_matrix(num_clust * num_points);
	for (int i = 0; i < num_clust; i++)
	{
		for (int j = 0; j < num_points; j++)
		{
			dist_matrix[i * num_points + j] = 1e100;
		}
	}

	while (num_centroids < num_clust)
	{
		// Update distance matrix
		// Update distance matrix
		update_dist_matrix(dist_matrix, centroids, points, num_centroids, num_dims, num_points);
		// Assign point to closest centroid
		// At the same time, 1. Create weights for random discrete distribution according to formula:
		vector<double> weights;
		for (int i = 0; i < num_points; i++)
		{
			// find closest centroid of point
			double closest_dist = 1e100;
			int closest_clust = -1;
			for (int j = 0; j < num_clust; j++)
			{
				if (dist_matrix[j * num_points + i] < closest_dist)
				{
					closest_dist = dist_matrix[j * num_points + i];
					closest_clust = j;
				}
			}
			cluster_assign[i] = closest_clust;
			weights.push_back(closest_dist);
		}
		// Find new centroid
		// 2. New centroid is the farthest point from fixed clusters
		const int chosen_point = distance(weights.begin(), max_element(weights.begin(), weights.end()));
		const double* point = &(points.data()[chosen_point * num_dims]);
		const int start = num_centroids * num_dims;
		for (int i = 0; i < num_dims; i++)
		{
			centroids[start + i] = point[i];
		}
		num_centroids++;
	}

	compKmeansCentroids(centroids, points, cluster_assign, n_pts_per_clust, num_dims);

	// Manage case where there are empty clusters
	// Affect the farthest points to the empty clusters

	// 1. Find then umber of required points num_pts_to_fill
	vector<int> empty_clusters;

	for (int i = 0; i < num_clust; i++)
	{
		if (n_pts_per_clust[i] == 0)
		{
			empty_clusters.push_back(i);
		}
	}
	const int num_pts_to_fill = empty_clusters.size();
	// 2. Find the num_pts_to_fill farthest away points
	// 2.1 Compute dist to centroids
	vector<pair<double, int>> d_to_clust;
	for (int i = 0; i < num_points; i++)
	{
		const double* point = &(points.data()[i * num_dims]);
		const int cluster = cluster_assign[i];
		const double* clust_coords = &(centroids.data()[cluster * num_dims]);
		d_to_clust.push_back(make_pair(dist(point, clust_coords, num_dims), i));
	}
	partial_sort(d_to_clust.rbegin(), d_to_clust.rbegin() + num_pts_to_fill, d_to_clust.rend(), greater{});
	for (int i = 0; i < num_pts_to_fill; i++)
	{
		auto& [d, idx_pt] = d_to_clust.at(i);
		const int origClusterPt = cluster_assign[idx_pt];
		const int clustToFill = empty_clusters[i];
		cluster_assign[idx_pt] = clustToFill;
		n_pts_per_clust[origClusterPt]--;
		n_pts_per_clust[clustToFill]++;
	}
	// Recompute centers of gravity
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] = 0;
	}
	for (int i = 0; i < num_points * num_dims; i++)
	{
		const double point_coord = points[i];
		const int cluster = cluster_assign[i / num_dims];
		centroids[cluster * num_dims + i % num_dims] += point_coord;
	}
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] /= n_pts_per_clust[i / num_dims];
	}
}

/** @brief * Kmeans++ super glutton: instead of sampling the next centroid using a random distribution, a random point is sampled among the k closest neighbours
	* @param[in] points: vector of points to cluster
	* @param[in] num_dims: number of dimensions of points
	* @param[in] num_clust: number of clusters
	* @param[in] seed: seed for random number generator
	* @param[out] cluster_assign: vector of cluster assignments
	* @param[out] n_pts_per_clust: vector of number of points per cluster
	* @param[out] centroids: vector of centroids
	*
*/
void init_clustering_greedy_topk(vector<double>& centroids, const vector<double>& points, vector<int>& cluster_assign, vector<int>& n_pts_per_clust, const int num_dims, const int seed, const int topk) {
	const int N = topk;
	const double GRID_MAX = 100;
	const int num_clust = centroids.size() / num_dims;
	const int num_points = points.size() / num_dims;
	mt19937 gen_dist(seed);
	uniform_real_distribution<> dis_choice(0, GRID_MAX);
	// "D(x) denote the shortest distance from a data point to the closest center we have already chosen."
	//" Take one center c1, chosen uniformly at random from X"
	for (int i = 0; i < num_dims; i++)
	{
		centroids[i] = dis_choice(gen_dist);
	}
	int num_centroids = 1;
	vector<double> dist_matrix(num_clust * num_points);
	for (int i = 0; i < num_clust; i++)
	{
		for (int j = 0; j < num_points; j++)
		{
			dist_matrix[i * num_points + j] = 1e100;
		}
	}

	while (num_centroids < num_clust)
	{
		// Update distance matrix
		update_dist_matrix(dist_matrix, centroids, points, num_centroids, num_dims, num_points);
		// Assign point to closest centroid
		// At the same time, 1. Create weights for random discrete distribution according to formula:
		// Prepare for the top N
		vector<pair<double, int>> top_x_data;
		for (int i = 0; i < num_points; i++)
		{
			// find closest centroid of point
			double closest_dist = 1e100;
			int closest_clust = -1;
			for (int j = 0; j < num_clust; j++)
			{
				if (dist_matrix[j * num_points + i] < closest_dist)
				{
					closest_dist = dist_matrix[j * num_points + i];
					closest_clust = j;
				}
			}
			cluster_assign[i] = closest_clust;
			top_x_data.push_back({ -closest_dist , i });
		}
		// Take top N
		const int size_top = N < (int)(top_x_data.size()) ? N : (int)(top_x_data.size());
		nth_element(top_x_data.begin(), top_x_data.begin() + size_top, top_x_data.end(),
			[](auto& l, auto& r)
			{ return l.first < r.first; });
		// Find new centroid
		// 2. Find new centroid using random discrete distribution
		uniform_int_distribution<> dis_top(0, size_top - 1);
		const int selected_of_top_id = dis_top(gen_dist);
		const int chosen_point = top_x_data.at(selected_of_top_id).second;
		const double* point = &(points.data()[chosen_point * num_dims]);
		const int start = num_centroids * num_dims;
		for (int i = 0; i < num_dims; i++)
		{
			centroids[start + i] = point[i];
		}
		num_centroids++;
	}

	compKmeansCentroids(centroids, points, cluster_assign, n_pts_per_clust, num_dims);

	// Manage case where there are empty clusters
	// Affect the farthest points to the empty clusters

	// 1. Find then umber of required points num_pts_to_fill
	vector<int> empty_clusters;

	for (int i = 0; i < num_clust; i++)
	{
		if (n_pts_per_clust[i] == 0)
		{
			empty_clusters.push_back(i);
		}
	}
	const int num_pts_to_fill = empty_clusters.size();
	// 2. Find the num_pts_to_fill farthest away points
	// 2.1 Compute dist to centroids
	vector<pair<double, int>> d_to_clust;
	for (int i = 0; i < num_points; i++)
	{
		const double* point = &(points.data()[i * num_dims]);
		const int cluster = cluster_assign[i];
		const double* clust_coords = &(centroids.data()[cluster * num_dims]);
		d_to_clust.push_back(make_pair(dist(point, clust_coords, num_dims), i));
	}
	partial_sort(d_to_clust.rbegin(), d_to_clust.rbegin() + num_pts_to_fill, d_to_clust.rend(), greater{});
	for (int i = 0; i < num_pts_to_fill; i++)
	{
		auto& [d, idx_pt] = d_to_clust[i];
		const int origClusterPt = cluster_assign[idx_pt];
		const int clustToFill = empty_clusters[i];
		cluster_assign[idx_pt] = clustToFill;
		n_pts_per_clust[origClusterPt]--;
		n_pts_per_clust[clustToFill]++;
	}
	// Recompute centers of gravity
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] = 0;
	}
	for (int i = 0; i < num_points * num_dims; i++)
	{
		const double point_coord = points[i];
		const int cluster = cluster_assign[i / num_dims];
		centroids[cluster * num_dims + i % num_dims] += point_coord;
	}
	for (int i = 0; i < num_clust * num_dims; i++)
	{
		centroids[i] /= n_pts_per_clust[i / num_dims];
	}
}