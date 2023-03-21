#pragma once
#include <random>
#include <math.h>
#include "H5Cpp.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <random>
#include <cmath>
#include <numeric>
#include "./open_hdf5.hpp"
#include "../types.h"
#include "../utils.h"
#include "./constants/clustering.hpp"
#include "./constants/tsp.hpp"
#include "./constants/maxsat.hpp"

using namespace H5;
using namespace std;


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
/** @brief Generate a vector of random points
* *
* * @param num_points Number of points
* * @param num_dims Number of dimensions
* * @param seed Seed for the random number generator
* * @param minV Minimum value for the random number generator X coordinate
* * @param maxV Maximum value for the random number generator Y coordinate
* * @return const vector<double>& Vector of random points 1 dimensional
* */
const vector<double>* normal_points(const unsigned long num_points, const unsigned long num_dims, const unsigned int seed, const double minV = DEFAULT_MIN_V, const double maxV = DEFAULT_MAX_V)
{
	normal_distribution<> dis(minV, maxV);
	return fill_vector_with<double>(dis, num_points * num_dims, seed);
}
/** @brief Generate a vector of random clusters
* * @param num_clust Number of clusters possible
* * @param num_points Number of points to assign a clusters
* * @param seed Seed for the random number generator
* * @return const vector<int>& Vector of random clusters (no guarantee that all clusters are present)
*/
vector<int>* random_clust(const unsigned long num_clust, const unsigned long num_points, const unsigned int seed)
{
	uniform_int_distribution<> distr(0, num_clust - 1);
	return fill_vector_with<int>(distr, num_points, seed);
}

/** @brief Generate a vector of random clusters
 * * @param num_clust Number of clusters possible
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


/** @brief Open the clustering town positions from the hdf5 file, set the number of points and dimensions. If load_num_clust is true, it will also load the number of clusters
* * @param instance Instance number
* * @param filename File name
* * @param conf Clustering configuration that will be modified with the number of points and dimensions (and number of clusters if load_num_clust is true)
* * @param load_num_clust If true, it will load the number of clusters
* * @return const vector<double>* Vector of points
* */
const vector<double>* open_clustering(const int instance, string filename, ClusteringConfig* conf, bool load_num_clust)
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
/** @brief Open the maxsat benchmark from the hdf5 file
* * @param instance Instance number
* * @param filename File name
* * @param conf MAXSAT configuration that will be modified with the number of points and dimensions
* * @return const vector<double>& Vector of points
* */
const vector<double>* open_maxsat_benchmark(const int instance, string filename, MAXSATConfig* conf)
{
	auto process_file = [conf, instance](H5File& f) -> vector<double>*
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
/** @brief Parse data from the maxsat benchmark hdf5 files into clauses, weights and compute the number of variables (with the observed variables in clauses)
* * @param data Data from the hdf5 file
* * @return tuple<vector<clause_t>, vector<weight_t>, n_vars_t> Clauses, weights and number of variables
*/
tuple<vector<clause_t>, vector<weight_t>, n_vars_t> parse_maxsat(const vector<double>& data)
{
	// parse data
	vector<double> weights;
	vector<clause_t> clauses;
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
			clauses.push_back(clause_t());
		}
		else if (skipNext)
		{
			taille--;
			skipNext = false;
			weights.push_back(data.at(i));
		}
		else
		{
			const int variable = (int)abs(data.at(i)) - 1;
			variables.insert(variable);
			clauses.at(clauses.size() - 1).push_back(make_pair(variable, sgn<double>(data.at(i)) == 1));
			taille--;
		}
	}
	return make_tuple(clauses, weights, variables.size());
}
