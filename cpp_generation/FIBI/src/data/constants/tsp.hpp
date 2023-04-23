#pragma once

#include "abstract.hpp"
using namespace std;
class TSPConfig : public AbstractConfig
{
public:
	Uninitialized<long> SEED_GLOB,
		SEED_PROBLEM,
		SEED_ASSIGN,
		DATASET,
		FI_BI,
		IMPR,
		NUM_TOWNS,
		NUM_DIM;

public:
	TSPConfig(map<string,long>* args)
	{
		addCst(SEED_GLOB);
		addCst(SEED_PROBLEM);
		addCst(SEED_ASSIGN);
		addCst(DATASET);
		addCst(FI_BI);
		addCst(IMPR);
		addCst(NUM_TOWNS);
		addCst(NUM_DIM);
	}
	vector<pair<string, double>> get_json() const override {
		return vector<pair<string, double>>{
			jMk(SEED_GLOB),
			jMk(SEED_PROBLEM),
				jMk(SEED_ASSIGN),
				jMk(DATASET),
				jMk(FI_BI),
				jMk(IMPR),
				jMk(NUM_TOWNS),
				jMk(NUM_DIM)
		};
	}
	const int num_choices() const
	{
		return this->NUM_TOWNS.get();
	}
};