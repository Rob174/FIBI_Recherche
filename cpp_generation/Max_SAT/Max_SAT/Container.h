#ifndef CONTAINER_H
#define CONTAINER_H
#include <vector>
#include <map>
#include <set>
#include "utils.h"
#include "exceptions.h"
#include <string>
#include <iostream>
#include "constants.h"
#include "types.h"


template <typename T_Swap>
class AbstractContainer
{
protected:
    T_Swap last_choice;
    quality_t quality_metric;

public:
    AbstractContainer() {
    };
    // Setup
    void on_improvement_done() { this->quality_metric = this->compute_quality_metric(); };
    // Flips
    virtual quality_delta_t test_flip(const T_Swap & test_swap) = 0;
    virtual void flip(const T_Swap & swap, const quality_delta_t delta) = 0;
    // Objective
    virtual quality_t get_quality_metric() { return quality_metric; };
    void set_quality_metric(const quality_t q) { this->quality_metric = q; }
    virtual quality_t compute_quality_metric() = 0;
    void update_with_delta_cost(const quality_delta_t delta){ quality_metric += delta;};
    // Utility
    virtual void print() = 0;
    std::map<std::string, int> get_last_choice() { return last_choice->get_dict(); };
};
class MAXSATContainer : public AbstractContainer<MAXSATSwap>
{
private:
    const int num_variables;
    const int num_clauses;

    const double *weights;
    bool *variables_val;
    std::vector<clause_t> clauses;

    std::map<const var_id_t, std::vector<clause_id_t>> var_in_clauses;
    bool *sat_clauses;

public:
    MAXSATConfig *config;
    MAXSATContainer(const double *weights, const int num_variables, bool *init_variables_val, std::vector<clause_t> clauses, MAXSATConfig *config);
    MAXSATContainer(MAXSATContainer& other) : num_variables(other.get_num_variables()), num_clauses(other.get_clauses().size()) {
        this->weights = copy_array<const double,double>(other.get_weights(), other.get_clauses().size());
        this->variables_val = copy_array<const bool,bool>(other.get_variables_val(), other.get_num_variables());
        this->clauses = other.get_clauses();
        this->var_in_clauses = other.get_var_in_clauses();
        this->sat_clauses = copy_array<bool,bool>(other.get_sat_clause(),other.get_clauses().size());
        this->config = other.config;
    }
    ~MAXSATContainer()
    {
        delete[] sat_clauses;
        delete[] weights;
        delete[] variables_val;
    }
    // Flips
    quality_delta_t test_flip(const MAXSATSwap& test_swap);
    void flip(const MAXSATSwap& swap, const quality_delta_t delta);
    // Objective
    double compute_quality_metric();
    // Getters
    const int get_num_variables() { return num_variables; }
    bool* get_sat_clause() { return sat_clauses; }
    const double *get_weights();
    const std::vector<clause_t> get_clauses();
    const bool *get_variables_val() { return variables_val; }
    const double *get_pweights();
    std::map<const var_id_t, std::vector<clause_id_t>> get_var_in_clauses() { return this->var_in_clauses; }
    // Setters
    void set_var_value(const var_id_t var_id, bool value);
    // Utility
    void print();
    static std::string clauses_str(std::vector<clause_t> clauses);
};
class ClusteringContainer : public AbstractContainer<ClusteringSwap>
{
public:
    const double *p_c; // points coor
    int *c_a;          // cluster assignements
    int *n_p_p_c;      // num_pts_per_clust
    double *c_c;       // cluster_centroids
    double cost;
    ClusteringConfig *conf;

public:
    ClusteringContainer(const double *points, int *assignements, ClusteringConfig *conf);
    ClusteringContainer(ClusteringContainer& other) {
        this->p_c = copy_array<const double,double>(other.p_c, other.conf->NUM_POINTS.get() * other.conf->NUM_DIM.get());
        this->c_a = copy_array<int,int>(other.c_a, other.conf->NUM_POINTS.get());
        this->n_p_p_c = copy_array<int,int>(other.n_p_p_c, other.conf->NUM_CLUST.get());
        this->c_c = copy_array<double,double>(other.c_c, other.conf->NUM_CLUST.get()* other.conf->NUM_DIM.get());
        this->cost = other.cost;
        this->conf = other.conf;
    }
    ~ClusteringContainer()
    {
        delete[] c_a;
        delete[] n_p_p_c;
        delete[] c_c;
        delete[] p_c;
    }
    // Flips
    quality_delta_t test_flip(const ClusteringSwap& test_swap);
    void flip(const ClusteringSwap& swap, const quality_delta_t delta);
    // Objective
    double compute_quality_metric();
    // Utility
    void print();
};
class DistanceMatrix
{
private:
    const int num_towns;
    const double* towns_pos;
    double **distances;

public:
    DistanceMatrix(const int num_towns, const int num_dim, const double* towns_pos);
    ~DistanceMatrix();
    double get(const town_in_tour_id_t i, const town_in_tour_id_t j);
    const double *get_town_pos() { return towns_pos; }
};
class TSPContainer : public AbstractContainer<TSPSwap>
{
public:
    std::vector<town_in_tour_id_t> tour;
    TSPConfig *conf;
    DistanceMatrix *dist;

public:
    TSPContainer(int* tour, DistanceMatrix* m, TSPConfig* conf) : conf(conf), dist(m), AbstractContainer<TSPSwap>() {
        for (int i = 0; i < this->conf->NUM_TOWNS.get(); i++) {
            this->tour.push_back(tour[i]);
        }
    };
    TSPContainer(TSPContainer& other) {
        this->tour = other.tour;
        this->conf = other.conf;
        this->dist = other.dist;
    }
    ~TSPContainer()
    {
    }
    // Flips
    quality_delta_t test_flip(const TSPSwap& test_swap);
    void flip(const TSPSwap& swap, const quality_delta_t delta);
    // Objective
    double compute_quality_metric();
    const double compute_delta(const TSPSwap & test_swap);
    // Getters
    int *get_tour() { return this->tour.data(); }
    const double *get_towns_pos() { return this->dist->get_town_pos(); }
    // Utility
    town_in_tour_id_t cycle_id(const town_in_tour_id_t i)
    {
        const int num_towns = this->conf->NUM_TOWNS.get();
        town_in_tour_id_t a = i < 0 ? num_towns + i : i;
        return a % num_towns;
    }
    void print();
};
#endif