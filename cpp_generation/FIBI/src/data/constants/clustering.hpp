#pragma once

#include "abstract.hpp"
using namespace std;
class ClusteringConfig : public AbstractConfig
{
public:
	Uninitialized<int> SEED_GLOB,
		SEED_PROBLEM,
		SEED_ASSIGN,
		DATASET,
		FI_BI,
		IMPR,
		NUM_POINTS,
		NUM_DIM,
		NUM_CLUST;

public:
	ClusteringConfig(map<string, int> args)
	{
		addCst(SEED_GLOB);
		addCst(SEED_PROBLEM);
		addCst(SEED_ASSIGN);
		addCst(DATASET);
		addCst(FI_BI);
		addCst(IMPR);
		addCst(NUM_POINTS);
		addCst(NUM_DIM);
		addCst(NUM_CLUST);
	}
	const int num_choices() const
	{
		return this->NUM_POINTS.get();
	};
	vector<pair<string, double>> get_json()  const override {
		return vector<pair<string, double>>{
			jMk(SEED_GLOB),
			jMk(SEED_PROBLEM),
			jMk(SEED_ASSIGN),
			jMk(DATASET),
			jMk(FI_BI),
			jMk(IMPR),
			jMk(NUM_POINTS),
			jMk(NUM_DIM),
			jMk(NUM_CLUST)
		};
	}
};