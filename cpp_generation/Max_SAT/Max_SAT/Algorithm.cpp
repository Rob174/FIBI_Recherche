#include "Algorithm.h"
#define EPSILON 1e-5
#define DEBUG_LV1
// #define DEBUG_LV2
bool MAXSATAlgorithm::explore_flips(MAXSATContainer *container, MAXSATConfig *config, bool FI)
{
    double delta = 0;
    MAXSATSwap chosen_swap(-1);
    for (var_id_t i_elem = 0; i_elem < config->num_choices(); i_elem++)
    {
        double delta_ij = container->test_flip(MAXSATSwap(i_elem));
        if (delta_ij > EPSILON)
        {
            delta = delta_ij;
            chosen_swap.var_id = i_elem;
            if (FI)
            {
                this->on_glob_iter_end(container);
                this->on_iter_end(container,chosen_swap);
                container->flip(chosen_swap, delta);
                return false;
            }
        }
        this->on_glob_iter_end(container);
    }
    if (delta < EPSILON)
    {
        this->on_iter_end(container,chosen_swap);
        return true;
    }
    container->flip(chosen_swap, delta);
    this->on_iter_end(container,chosen_swap);
    return false;
}

bool ClusteringAlgorithm::explore_flips(ClusteringContainer *container, ClusteringConfig *conf, bool FI)
{
    double delta = 0;
    ClusteringSwap chosen_swap(-1, -1, -1);
    for (point_id_t point_moving_id = 0; point_moving_id < M_cst(NUM_POINTS); point_moving_id++)
    {
        cluster_id_t from_cluster_id = container->c_a[point_moving_id];
        for (int i = from_cluster_id + 1; i < from_cluster_id + M_cst(NUM_CLUST); i++)
        {
            const cluster_id_t to_clust_id = i % M_cst(NUM_CLUST);
            ClusteringSwap tmp_swap(point_moving_id, from_cluster_id, to_clust_id);
            double delta_ij = container->test_flip(
                tmp_swap);
            if (delta_ij < -EPSILON)
            {
                delta = delta_ij;
                chosen_swap = tmp_swap;
                this->on_glob_iter_end(container);
                if (FI)
                {
                    container->flip(chosen_swap, delta);
                    this->on_iter_end(container,chosen_swap);
                    return false;
                }
            }
        }
    }
    if (abs(delta) < EPSILON)
    {
        this->on_iter_end(container, chosen_swap);
        return true;
    }
    // Update cluster assignements
    container->flip(chosen_swap, delta);
    this->on_iter_end(container, chosen_swap);
    return false;
}

bool TSPAlgorithm::explore_flips(TSPContainer *container, TSPConfig *config, bool FI)
{
    double delta = 0;
    TSPSwap chosen_swap(-1, -1);
    // WARNING: here we use <= in the loops instead of < because we want to test the last flip
    for (town_in_tour_id_t i = 0; i <= config->get("NUM_TOWNS") - 2; i++)
    {
        int n_prime = i == 0 ? config->get("NUM_TOWNS") - 1 : config->get("NUM_TOWNS");
        for (town_in_tour_id_t j = i + 2; j <= n_prime; j++)
        {
            TSPSwap tmp_swap(container->cycle_id(i), container->cycle_id(j));
            double delta_ij = container->test_flip(tmp_swap);
            if (delta_ij < -EPSILON)
            {
                delta = delta_ij;
                chosen_swap = tmp_swap;
                this->on_glob_iter_end(container);
                if (FI)
                {
                    container->flip(chosen_swap, delta);
                    this->on_iter_end(container, chosen_swap);
                    return false;
                }
            }
        }
    }
    if (abs(delta) < EPSILON)
    {
        this->on_iter_end(container, chosen_swap);
        return true;
    }
    container->flip(chosen_swap, delta);
    this->on_iter_end(container, chosen_swap);
    return false;
}