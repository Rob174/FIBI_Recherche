#pragma once

#include <vector>
#include "./abstract.hpp"
#include "../../algorithm/swaps/clustering.hpp"
#include "../../utils.h"

using namespace std;

class ClusteringSolutionContainer : public AbstractSolutionContainer
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
