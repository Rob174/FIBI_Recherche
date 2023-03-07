#ifndef CLUSTERINGGENERATOR_H
#define CLUSTERINGGENERATOR_H
#include <random>
#include <math.h>
#include <H5Cpp.h>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <random>
#include "constants.h"
#include "Container.h"
#include "open_hdf5.hpp"
#include "types.h"

using namespace H5;
using namespace std;
template <typename T_return, typename T_distr>
vector<T_return> fill_vector_with(T_distr distr, int size, const unsigned int seed)
{
    mt19937 gen(seed);
    vector<T> vec(size);
    for (int i = 0; i < size; i++)
    {
        vec[i] = distr(gen);
    }
    return vec;
}
const double DEFAULT_MIN_V = 0;
const double DEFAULT_MAX_V = 100;
vector<double> uniform_points(const unsigned int num_points, const unsigned int num_dims, const unsigned int seed, const double minV = DEFAULT_MIN_V, const double maxV = DEFAULT_MAX_V)
{
    uniform_real_distribution<> dis(minV, maxV);
    return fill_vector_with<double>(dis, num_points * num_dims, seed);
}

vector<double> normal_points(const unsigned int num_points, const unsigned int num_dims, const unsigned int seed, const double minV = DEFAULT_MIN_V, const double maxV = DEFAULT_MAX_V)
{
    normal_distribution<> dis(minV, maxV);
    return fill_vector_with<double>(dis, num_points * num_dims, seed);
}

vector<int> random_clust(const unsigned int num_clust, const unsigned int num_points, const unsigned int seed)
{
    uniform_int_distribution<> distr(0, num_clust - 1);
    return fill_vector_with<int>(distr, num_points, seed);
}
vector<double> test_points()
{
    vector<double> points = {
        78.383235, 31.307694,
        63.483371, 93.723736,
        24.904309, 4.286545,
        75.807586, 44.086720};

    return points;
}
vector<int> test_clusters()
{
    return vector<int>{
        1, 1, 0, 0};
}
vector<double> load_fixed_clauses()
{
    const int size = 8;
    double points[size] = {
        // size_data, (n_vars_in_clause2,weight_clause1 , var1, -var2), (n_vars_in_clause2,weight_clause2 , var1)
        7,
        3, 1, 1, -2,
        2, 1, 1};
    vector<double> points_out(size);
    for (int i = 0; i < size; i++)
    {
        points_out[i] = points[i];
    }
    return points_out;
}
vector<bool> load_fixed_assign()
{
    vector<bool> assign(2);
    assign[0] = false;
    assign[1] = false;
    // with that we have 1st clause to true (1 -2 -> FALSE V TRUE) and second to false (1 -> FALSE).  One improvement possibility expected
    return assign;
}
vector<double> open_clustering(const int instance, string filename, ClusteringConfig *conf, bool load_num_clust)
{

    auto process_file = [conf, load_num_clust, instance](H5File &f) -> vector<double>
    {
        H5std_string DATASET_NAME(to_string(instance));
        using namespace std;
        vector<double> dims = process_group("dims", DATASET_NAME, f);
        conf->NUM_DIM.set((int)dims[dims.size() == 1 ? 0 : 1]);
        vector<double> points = process_group("points", DATASET_NAME, f);
        conf->NUM_POINTS.set(points.size() / conf->NUM_DIM.get());
        if (load_num_clust)
        {
            vector<double> num_clust = process_group("num_clusters", DATASET_NAME, f);
            conf->NUM_CLUST.set(num_clust[0]);
        }
        return points;
    };
    return open_hdf5<vector<double>>(
        filename,
        process_file);
}
vector<double> open_tsplib(const int instance, string filename, TSPConfig *conf)
{

    auto process_file = [conf, instance](H5File &f) -> vector<double>
    {
        using namespace std;
        H5std_string DATASET_NAME(to_string(instance));
        vector<double> points = open_dataset<double>(DATASET_NAME, f);
        conf->NUM_TOWNS.set(points.size() / conf->NUM_DIM.get());
        return points;
    };
    return open_hdf5<vector<double>>(
        filename,
        process_file);
}

vector<double> open_maxsat_benchmark(const int instance, string filename, MAXSATConfig *conf)
{
    auto process_file = [conf, instance](H5File &f) -> vector<double>
    {
        using namespace std;
        H5std_string DATASET_NAME(to_string(instance));
        vector<double> data = open_dataset<double>(DATASET_NAME, f);
        return data;
    };
    return open_hdf5<vector<double>>(
        filename,
        process_file);
}

tuple<vector<clause_t>, vector<weight_t>, n_vars_t> parse_maxsat(vector<double> &data)
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
}
#endif