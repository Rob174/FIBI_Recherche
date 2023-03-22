#pragma once

#include <vector>
#include "./abstract.hpp"
#include "../../algorithm/swaps/clustering.hpp"
#include "../../utils.h"

using namespace std;

template <bool debug = false, const double EPSILON = 1e-5>
class ClusteringSolutionContainer : public AbstractSolutionContainer<ClusteringSwap>
{
public:
	const vector<double>& p_c; // points coor
	vector<int>& c_a;          // cluster assignements
	vector<int> n_p_p_c;      // num_pts_per_clust
	vector<double> c_c;       // cluster_centroids
	const ClusteringConfig& conf;

public:
	ClusteringSolutionContainer(const vector<double>& points, vector<int>& assignements, const ClusteringConfig& conf) :
		p_c(points),
		c_c(vector<double>(conf.NUM_CLUST.get()* conf.NUM_DIM.get(), 0)),
		n_p_p_c(vector<int>(conf.NUM_CLUST.get(), 0)),
		c_a(assignements),
		conf(conf),
		AbstractSolutionContainer() {
		for (int i = 0; i < conf.NUM_POINTS.get(); i++)
		{
			n_p_p_c[assignements[i]]++;
			for (int d = 0; d < conf.NUM_DIM.get(); d++)
			{
				const double v = points[i * conf.NUM_DIM.get() + d];
				c_c[assignements[i] * conf.NUM_DIM.get() + d] += v;
			}
		}
		for (int a = 0; a < conf.NUM_CLUST.get() * conf.NUM_DIM.get(); a++)
		{
			c_c[a] /= n_p_p_c[a / conf.NUM_DIM.get()];
		}
	};
	ClusteringSolutionContainer(ClusteringSolutionContainer& other) : AbstractSolutionContainer(other), p_c(other.p_c), c_a(other.c_a), n_p_p_c(other.n_p_p_c), c_c(other.c_c), conf(other.conf) {}

	// Flips
	quality_delta_t test_flip(const ClusteringSwap& test_swap) const {
		// f: source cluster
		// t: destination cluster
		const int n_f = n_p_p_c[test_swap.cluster_src];
		const int n_t = n_p_p_c[test_swap.cluster_dst];
		const int f = test_swap.cluster_src;
		const int t = test_swap.cluster_dst;
		const Slice<> point(p_c, test_swap.point_id * conf.NUM_DIM.get());
		const Slice<> centroid_t(c_c, t * conf.NUM_DIM.get());
		const Slice<> centroid_f(c_c, f * conf.NUM_DIM.get());
		const double part1 = (double)(n_t) / (double)(n_t + 1)
			* dist_squared(centroid_t, point, conf.NUM_DIM.get());
		double part2 = 0;
		if (n_f > 1)
		{
			part2 = (double)(n_f) / (double)(n_f - 1) * dist_squared(centroid_f, point, conf.NUM_DIM.get());
		}
		return part1 - part2;
	};
	void flip(const ClusteringSwap& swap, const quality_delta_t delta) {
		if constexpr (debug) {
			for (int i = 0; i < conf.NUM_POINTS.get(); i++)
			{
				cout << c_a[i] << ",";
			}
			cout << endl;
			const int past_cost = compute_quality_metric();
		}
		// f: source cluster
		// t: destination cluster
		// update centroids
		const Slice<> point = Slice<>(p_c, swap.point_id * conf.NUM_DIM.get());
		for (int dim = 0; dim < conf.NUM_DIM.get(); dim++)
		{
			if (n_p_p_c[swap.cluster_src] <= 1) // If we had one point
				c_c[swap.cluster_src * conf.NUM_DIM.get() + dim] = 0.;
			else
				c_c[swap.cluster_src * conf.NUM_DIM.get() + dim] = (double)(n_p_p_c[swap.cluster_src] * c_c[swap.cluster_src * conf.NUM_DIM.get() + dim] - point[dim]) /
				(double)(n_p_p_c[swap.cluster_src] - 1);
			c_c[swap.cluster_dst * conf.NUM_DIM.get() + dim] = (double)(n_p_p_c[swap.cluster_dst] * c_c[swap.cluster_dst * conf.NUM_DIM.get() + dim] + point[dim]) /
				(double)(n_p_p_c[swap.cluster_dst] + 1);
		}
		// update num_pts_per_clust
		n_p_p_c[swap.cluster_dst] = n_p_p_c[swap.cluster_dst] + 1;
		n_p_p_c[swap.cluster_src] = n_p_p_c[swap.cluster_src] - 1;
		c_a[swap.point_id] = swap.cluster_dst;

		if constexpr (debug) {
			const double real_cost = compute_quality_metric();
			const double delta_modif = test_flip(swap);
			const double orig_cost = get_quality_metric();
			update_with_delta_cost(delta);
			const double error = abs(get_quality_metric() - real_cost);
			if (error > EPSILON)
			{
				test_flip(swap);
				compute_quality_metric();
				for (int i = 0; i < conf.NUM_POINTS.get() * conf.NUM_DIM.get(); i++)
				{
					cout << p_c[i] << ",";
				}
				cout << endl;
				cout << "After" << endl;
				for (int i = 0; i < conf.NUM_POINTS.get(); i++)
				{
					// If we are at the point_id change color
					if (i == swap.point_id)
					{
						cout << "\033[1;31m" << c_a[i] << "\033[0m,";
					}
					else
					{
						cout << c_a[i] << ",";
					}
				}
				cout << endl;
				throw exception("Non matching costs");
			}
		}
		else {
			update_with_delta_cost(delta);
		}

		last_choice = swap;
	};
	// Objective
	double compute_quality_metric() {
		// compute cost
		double cost = 0;
		for (int i = 0; i < conf.NUM_POINTS.get(); i++)
		{
			const Slice<> point(p_c, i * conf.NUM_DIM.get());
			const Slice<> centroid(c_c, c_a[i] * conf.NUM_DIM.get());
			const double d = dist_squared<Slice<>>(point, centroid, conf.NUM_DIM.get());
			cost += d;
		}
		return cost;
	};
	// Utility
	void print()
	{
		cout << "----------------------------------------" << endl;
		cout << "\tPoints: " << endl;
		for (int i = 0; i < conf.NUM_POINTS.get(); i++)
		{
			for (int j = 0; j < conf.NUM_DIM.get(); j++)
			{
				cout << p_c[i * conf.NUM_DIM.get() + j] << ",";
			}
			cout << endl;
		}
		cout << "\tCentroids: " << endl;
		for (int i = 0; i < conf.NUM_CLUST.get(); i++)
		{
			for (int j = 0; j < conf.NUM_DIM.get(); j++)
			{
				cout << c_c[i * conf.NUM_DIM.get() + j] << ",";
			}
			cout << endl;
		}
		cout << "\tAssignements: " << endl;
		for (int i = 0; i < conf.NUM_POINTS.get(); i++)
		{
			cout << c_a[i] << ",";
		}
		cout << endl;
		cout << "\tCost: " << quality_metric << endl;
		cout << "----------------------------------------" << endl;
	};
};
