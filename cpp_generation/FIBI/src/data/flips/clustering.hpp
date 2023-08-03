#pragma once
#include "abstract.hpp"
#include "../../algorithm/swaps/clustering.hpp"

quality_delta_t fn_test_flip(ClusteringSwap& test_swap, const vector<int> &n_p_p_c, const vector<double> &p_c, const vector<double>& c_c, const int num_dim) {
    // f: source cluster
    // t: destination cluster
    const int n_f = n_p_p_c[test_swap.cluster_src];
    const int n_t = n_p_p_c[test_swap.cluster_dst];
    const int f = test_swap.cluster_src;
    const int t = test_swap.cluster_dst;
    const Slice<> point(p_c, test_swap.point_id * num_dim);
    const Slice<> centroid_t(c_c, t * num_dim);
    const Slice<> centroid_f(c_c, f * num_dim);
    const double part1 = (double)(n_t) / (double)(n_t + 1)
        * dist_squared(centroid_t, point, num_dim);
    double part2 = 0;
    if (n_f > 1)
    {
        part2 = (double)(n_f) / (double)(n_f - 1) * dist_squared(centroid_f, point, num_dim);
    }
    return part1 - part2;
}
template <bool debug = false>
void fn_flip(const ClusteringSwap& swap, vector<int> &n_p_p_c, const vector<double> &p_c, vector<int>& c_a, vector<double>& c_c, const int num_dim) {
    // f: source cluster
    // t: destination cluster
    // update centroids
    const Slice<> point = Slice<>(p_c, swap.point_id * num_dim);
    for (int dim = 0; dim < num_dim; dim++)
    {
        if (n_p_p_c[swap.cluster_src] <= 1) // If we had one point
            c_c[swap.cluster_src * num_dim + dim] = 0.;
        else
            c_c[swap.cluster_src * num_dim + dim] = (double)(n_p_p_c[swap.cluster_src] * c_c[swap.cluster_src * num_dim + dim] - point[dim]) /
            (double)(n_p_p_c[swap.cluster_src] - 1);
        c_c[swap.cluster_dst * num_dim + dim] = (double)(n_p_p_c[swap.cluster_dst] * c_c[swap.cluster_dst * num_dim + dim] + point[dim]) /
            (double)(n_p_p_c[swap.cluster_dst] + 1);
    }
    // update num_pts_per_clust
    n_p_p_c[swap.cluster_dst] = n_p_p_c[swap.cluster_dst] + 1;
    n_p_p_c[swap.cluster_src] = n_p_p_c[swap.cluster_src] - 1;
    c_a[swap.point_id] = swap.cluster_dst;
};