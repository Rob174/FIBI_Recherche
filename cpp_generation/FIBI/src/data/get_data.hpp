#pragma once
#include <random>
#include <math.h>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <random>
#include <cmath>
#include <numeric>
#include <algorithm>
#include "./open_hdf5.hpp"
#include "../types.h"
#include "../utils.h"
#include "./constants/clustering.hpp"
#include "./constants/tsp.hpp"
#include "./constants/maxsat.hpp"

using namespace std;
#ifdef HDF5save
#include "H5Cpp.h"
using namespace H5;
#endif
/** @brief Fill a vector with random numbers
* * @tparam T_return Type of the vector
* * @tparam T_distr Type of the distribution
* * @param distr Distribution to use
* * @param size Size of the vector
* * @param seed Seed for the random number generator
* * @return const vector<T_return>& Vector of random numbers
* */
template <typename T_return, typename T_distr>
vector<T_return>* fill_vector_with(T_distr distr, const unsigned long size, const unsigned int seed)
{
	mt19937 gen(seed);
	vector<T_return>* vec = new vector<T_return>(size);
	for (int i = 0; i < size; i++)
	{
		(*vec)[i] = distr(gen);
	}
	return vec;
}
const double DEFAULT_MIN_V = 0;
const double DEFAULT_MAX_V = 100;
/** @brief Generate a vector of random points
* *
* * @param num_points Number of points
* * @param num_dims Number of dimensions
* * @param seed Seed for the random number generator
* * @param minV Minimum value for the random number generator
* * @param maxV Maximum value for the random number generator
* * @return const vector<double>& Vector of random points 1 dimensional
*/
const vector<double>* uniform_points(const unsigned long num_points, const unsigned long num_dims, const unsigned int seed, const double minV = DEFAULT_MIN_V, const double maxV = DEFAULT_MAX_V)
{
	uniform_real_distribution<> dis(minV, maxV);
	return fill_vector_with<double>(dis, num_points * num_dims, seed);
}
/** @brief Generate a vector of random points. For the points are equally distributed into clusters. 
* * For each cluster a normal distribution is used with a random center in the range [minV, maxV] and a random standard deviation in the range [(maxV - minV) / 10, (maxV - minV) / 2]
* * Can be a different number of points per cluster of more than one because we need to assign the points to the clusters at the same time as we create them
* * @param num_points Number of points
* * @param num_dims Number of dimensions
* * @param num_clust Number of clusters
* * @param seed Seed for the random number generator
* * @param minV Minimum value for the random number generator X coordinate
* * @param maxV Maximum value for the random number generator Y coordinate
* * @return const tuple<vector<double>*,vector<int>*,vector<double>*, vector<double>*> Vector of 
* *		- random points, 
* *     - cluster assignments 
* *     - cluster centers (in the format num_dims * num_centers: flattened)
* *     - standard deviations
* */
const tuple<vector<double>*,vector<int>*, vector<double>*, vector<double>*> normal_points(const unsigned long num_points, const unsigned long num_dims, const unsigned int num_clust, const unsigned int seed, const double minV = DEFAULT_MIN_V, const double maxV = DEFAULT_MAX_V)
{
	const int num_points_per_clust = num_points / num_clust;
	const double clust_range = (maxV - minV) / num_clust;
	const double clust_std = (maxV - minV) / 10;
	const double clust_std_max = (maxV - minV) / 2;
	vector<double>* points = new vector<double>(num_points * num_dims);
	vector<int>* clust = new vector<int>(num_points); // Cluster assignment
	vector<double>* clust_centers = new vector<double>(num_clust*num_dims);
	vector<double>* clust_stds = new vector<double>(num_clust);
	mt19937 gen(seed);
	uniform_real_distribution<> dis(minV, maxV);
	uniform_real_distribution<> dis_std(clust_std, clust_std_max);
	int offset_pt = 0;
	// Number of points per cluster is num_points / num_clust plus 1 
	// if the number of points is not divisible by the number of clusters 
	// and if the cluster index is less than the remainder
	for (int i = 0; i < num_clust; i++)
	{
		// Generate a random standard deviation for the cluster
		const double std = dis_std(gen);
		(*clust_stds)[i] = (double)std;
		// Generate a normal distribution for the cluster
		normal_distribution<> dis_clust(0, std);
		const int added = (i < (num_points % num_clust) ? 1 : 0);
		const int number_of_pts = num_points_per_clust + added;
		// Fill the points with the normal distribution
		for (int k = 0; k < num_dims; k++) // dimensions first to sample the mean on the fly
		{
			const double mean = dis(gen);
			(*clust_centers)[i * num_dims + k] = (double)mean;
			// Move the decided mean
			for (int j = offset_pt; j < offset_pt+ number_of_pts; j++)
			{
				(*points)[j * num_dims + k] = dis_clust(gen) + mean;
				(*clust)[j] = i;
			}
		}
		offset_pt += number_of_pts;
	}
	return make_tuple(points, clust, clust_centers, clust_stds);
}
/** @brief Generate a permutation of the numbers 0 to num_towns - 1
* @param num_towns Number of towns
* @seed Seed for the random number generator
*/
vector<int>* random_tour(const long num_towns, const long seed)
{
	// Make a vector of all towns and shuffle it
#ifdef P_COMPACT
	vector<int>* tour = new vector<int>(num_towns);
	iota(tour->begin(), tour->end(), 0);
	shuffle(tour->begin(), tour->end(), mt19937(seed));
#else
	std::mt19937 gen_dist(seed);
	vector<int>* tour = new vector<int>(num_towns);
	for (int i = 0; i < num_towns; i++)
		(*tour)[i] = i;
	for (int i = 0; i < num_towns - 2; i++)
	{
		std::uniform_int_distribution<> dis_choice(i, num_towns - 1);
		int r = dis_choice(gen_dist);
		int tmp = (*tour)[i];
		(*tour)[i] = (*tour)[r];
		(*tour)[r] = tmp;
	}
#endif
	return tour;
}
/** @brief Generate a vector of random clusters
* * @param num_clust Number of clusters possible
* * @param num_points Number of points to assign a clusters
* * @param seed Seed for the random number generator
* * @return vector<int>* Vector of random clusters (no guarantee that all clusters are present)
*/
vector<int>* random_clust(const unsigned long num_clust, const unsigned long num_points, const unsigned int seed)
{
	uniform_int_distribution<> distr(0, num_clust - 1);
	return fill_vector_with<int>(distr, num_points, seed);
}
/** @brief Generate a vector of random weights
* * @param num_clauses Number of clauses to assign a weight to
* * @param seed Seed for the random number generator
* * @return vector of random weights in the range [0, 1000]
*/
vector<double>* random_weights(const unsigned long num_clauses, const unsigned int seed)
{
	uniform_real_distribution<> distr(0, 1000);
	return fill_vector_with<double>(distr, num_clauses, seed);
}

/** @brief Generate a vector of random variables
 * * @param num_variables Number of variables to assign a value to
 */
vector<bool>* random_assignements(const unsigned long num_variables, const unsigned int seed) {
	uniform_int_distribution<> distr(0, 1);
	return fill_vector_with<bool>(distr, num_variables, seed);
}

/** @brief Generate random clauses based on the given parameters
* * @param num_variables Number of variables
* * @param num_clauses Number of clauses
* * @param seed Seed for the random number generator
* * @param min_num_var_per_clauses Minimum number of variables per clause
* * @param max_num_var_per_clauses Maximum number of variables per clause
* * @return A pointer to a vector of clause_t representing the generated random clauses
*/
vector<clause_t>* random_clauses(
	const int num_variables, const int num_clauses, const int seed, const int min_num_var_per_clauses = 1, const int max_num_var_per_clauses = 25)
{
	mt19937 mt(seed);
	uniform_real_distribution<double> rd_w(0, 1000);
	uniform_int_distribution<int> rd_var(0, num_variables - 1);
	uniform_int_distribution<int> rd_bool(0, 1);

	vector<clause_t>* clauses = new vector<clause_t>();

	// Determine the number of mandatory variables per clause
	const int num_mandatory_per_clause = ceil(num_variables / (double)num_clauses);

	// Generate the mandatory variables
	vector<int> mandatory_variables(num_variables);
	iota(mandatory_variables.begin(), mandatory_variables.end(), 0);
	shuffle(mandatory_variables.begin(), mandatory_variables.end(), mt);
	set<int> vars_in_clauses;

	// Generate the clauses
	int mandatory_id = 0;
	for (int i = 0; i < num_clauses; i++)
	{
		// Generate the mandatory variables for this clause
		clause_t clause;
		set<int> vars_in_clause;
		for (int j = 0; j < num_mandatory_per_clause && mandatory_id < num_variables; j++, mandatory_id++)
		{
			clause.push_back(make_pair(mandatory_variables[mandatory_id], rd_bool(mt)));
			vars_in_clause.insert(mandatory_variables[mandatory_id]);
			vars_in_clauses.insert(mandatory_variables[mandatory_id]);
		}

		// Determine the remaining size of the clause
		const int remaining_size = max_num_var_per_clauses - clause.size();
		if (remaining_size < min_num_var_per_clauses) {
			throw runtime_error("Impossible to satisfy the requirement min_num_var_per_clauses=" + to_string(min_num_var_per_clauses) + " max_num_var_per_clauses=" + to_string(max_num_var_per_clauses) + "with num_variables=" + to_string(num_variables));
		}

		// Generate the remaining variables for this clause
		for (int j = 0; j < remaining_size; j++)
		{
			int variable;
			do {
				variable = rd_var(mt);
			} while (vars_in_clause.count(variable));
			clause.push_back(make_pair(variable, rd_bool(mt)));
			vars_in_clauses.insert(variable);
			vars_in_clause.insert(variable);
		}

		clauses->push_back(clause);
	}

	if (vars_in_clauses.size() < num_variables) {
		throw runtime_error("Problem to satisfy the requirement min_num_var_per_clauses=" + to_string(min_num_var_per_clauses) + " max_num_var_per_clauses=" + to_string(max_num_var_per_clauses) + "with num_variables=" + to_string(num_variables));
	}

	return clauses;
}


#ifdef HDF5save
/** @brief Open the clustering town positions from the hdf5 file, set the number of points and dimensions. If load_num_clust is true, it will also load the number of clusters
* * @param instance Instance number
* * @param filename File name
* * @param conf Clustering configuration that will be modified with the number of points and dimensions (and number of clusters if load_num_clust is true)
* * @param load_num_clust If true, it will load the number of clusters
* * @return const vector<double>* Vector of points
* */
const vector<double>* open_clustering(const int instance, string filename, ClusteringConfig* conf, bool load_num_clust = false)
{

	auto process_file = [conf, load_num_clust, instance](H5File& f) -> vector<double>*
	{
		H5std_string DATASET_NAME(to_string(instance));
		using namespace std;
		vector<double>* dims = process_group("dims", DATASET_NAME, f);
		conf->NUM_DIM.set((int)(*dims)[dims->size() == 1 ? 0 : 1]);
		vector<double>* points = process_group("points", DATASET_NAME, f);
		conf->NUM_POINTS.set(points->size() / conf->NUM_DIM.get());
		if (load_num_clust)
		{
			vector<double>* num_clust = process_group("num_clusters", DATASET_NAME, f);
			conf->NUM_CLUST.set((*num_clust)[0]);
		}
		return points;
	};
	return open_hdf5<vector<double>*>(
		filename,
		process_file);
}
#else
/** @brief Open the clustering town positions from the hdf5 file, set the number of points and dimensions. If load_num_clust is true, it will also load the number of clusters
* * @param instance Instance number
* * @param foldername Folder name:
* *		@param -> under subfolder points the points ;
* *		@param -> under subfolder dims the dimensions ;
* *		@param -> under subfolder num_clusters the number of clusters (if load_num_clust is true)
* * @param conf Clustering configuration that will be modified with the number of points and dimensions (and number of clusters if load_num_clust is true)
* * @param load_num_clust If true, it will load the number of clusters
* * @return const vector<double>* Vector of points
* */
const vector<double>* open_clustering(const int instance, string foldername, ClusteringConfig* conf, bool load_num_clust = false) {
	vector<double>* points = read_file_txt_multiprocessing(instance, foldername + "/points/");
	unique_ptr<vector<double>> dims(read_file_txt_multiprocessing(instance, foldername + "/dims/"));
	conf->NUM_DIM.set(dims->at(dims->size() - 1));
	conf->NUM_POINTS.set(points->size() / conf->NUM_DIM.get());
	if (load_num_clust) {
		unique_ptr<vector<double>> num_clust(read_file_txt_multiprocessing(instance, foldername + "/num_clusters/"));
		conf->NUM_CLUST.set(num_clust->at(0));
	}
	return points;
}
#endif
#ifdef HDF5save
/** @brief Open the tsp, tsplib town positions from the hdf5 file, set the number of points and dimensions
* * @param instance Instance number
* * @param filename File name
* * @param conf TSP configuration that will be modified with the number of points and dimensions
* * @return const vector<double>& Vector of points
*/
const vector<double>* open_tsplib(const int instance, string filename, TSPConfig* conf)
{

	auto process_file = [conf, instance](H5File& f) -> vector<double>*
	{
		const H5std_string DATASET_NAME(to_string(instance));
		vector<double>* points = open_dataset<double>(DATASET_NAME, f);
		conf->NUM_TOWNS.set(points->size() / conf->NUM_DIM.get());
		return points;
	};
	return open_hdf5<vector<double>*>(
		filename,
		process_file);
}
#else
/** @brief Open the tsp, tsplib town positions from the files under foldername set the number of points and dimensions
* * @param instance Instance number
* * @param foldername Folder name: under foldername/instances we have one txt file for each instance ; the file name is the instance number ; the file contains the double to read separated by a space
* * @param conf TSP configuration that will be modified with the number of points and dimensions
* * @return const vector<double>* Vector of points
* */
const vector<double>* open_tsplib(const int instance, string foldername, TSPConfig* conf) {
	vector<double>* points = read_file_txt_multiprocessing(instance, foldername);
	conf->NUM_TOWNS.set(points->size() / conf->NUM_DIM.get());
	return points;
}
#endif
#ifdef HDF5save
/** @brief Open the maxsat benchmark from the hdf5 file
* * @param instance Instance number
* * @param filename File name
* * @return const vector<double>& Vector of points
* */
const vector<double>* open_maxsat_benchmark(const int instance, string filename)
{
	auto process_file = [instance](H5File& f) -> vector<double>*
	{
		using namespace std;
		H5std_string DATASET_NAME(to_string(instance));
		vector<double>* data = open_dataset<double>(DATASET_NAME, f);
		return data;
	};
	return open_hdf5<vector<double>*>(
		filename,
		process_file);
}
#else
/** @brief Open the maxsat benchmark from the hdf5 file
* * @param instance Instance number
* * @param foldername Folder name: under foldername/instances we have one hdf5 file for each instance ; the file name is the instance number ; the file contains the double to read separated by a space
* * @return const vector<double>& Vector of points
* */
const vector<double>* open_maxsat_benchmark(const int instance, string foldername)
{
	vector<double>* data = read_file_txt_multiprocessing(instance, foldername);
	return data;
}
#endif // HDF5save
/** @brief Compute for each variables in which clauses it appears
* * @param clauses List of clauses
* * @return map<const var_id_t, vector<clause_id_t>>* Map of variables to clauses where it appears
* */
map<const var_id_t, vector<clause_id_t>>* get_var_to_clauses(const vector<clause_t>& clauses)
{
	map<const var_id_t, vector<clause_id_t>>* var_to_clauses = new map<const var_id_t, vector<clause_id_t>>();
	for (int i = 0; i < clauses.size(); i++)
	{
		for (int j = 0; j < clauses[i].size(); j++)
		{
			var_id_t var = clauses[i][j].first;
			if (var_to_clauses->count(var) == 0)
			{
				(*var_to_clauses)[var] = vector<clause_id_t>();
			}
			(*var_to_clauses)[var].push_back(i);
		}
	}
	return var_to_clauses;
}
/** @brief Parse data from the maxsat benchmark hdf5 files into clauses, weights and compute the number of variables (with the observed variables in clauses)
* * @param data Data from the hdf5 file
* * @return tuple<vector<clause_t>, vector<weight_t>, n_vars_t> Clauses, weights and number of variables
*/
tuple<vector<clause_t>*, vector<weight_t>*, n_vars_t> parse_maxsat(const vector<double>& data)
{
	// parse data
	vector<double>* weights = new vector<double>();
	vector<clause_t>* clauses = new vector<clause_t>();
	int i_clause = 0;
	int clause_size = 0; // to avoid 0%0 error

	set<int> variables;
	int taille = 0;
	bool skipNext = false; // because first is weight
	for (int i = 0; i < data.size(); i++)
	{
		if (taille == 0)
		{
			taille = data.at(i);
			skipNext = true;
			clauses->push_back(clause_t());
		}
		else if (skipNext)
		{
			taille--;
			skipNext = false;
			weights->push_back(data.at(i));
		}
		else
		{
			const int variable = (int)abs(data.at(i)) - 1;
			variables.insert(variable);
			clauses->at(clauses->size() - 1).push_back(make_pair(variable, sgn<double>(data.at(i)) == 1));
			taille--;
		}
	}
	return make_tuple(clauses, weights, variables.size());
}
