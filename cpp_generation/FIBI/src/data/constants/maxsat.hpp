#pragma once

#include "abstract.hpp"
using namespace std;

class MAXSATConfig : public AbstractConfig
{
public:
	Uninitialized<int> SEED_GLOB,
		SEED_PROBLEM,
		SEED_ASSIGN,
		DATASET,
		FI_BI,
		IMPR,
		NUM_VARIABLES,
		NUM_CLAUSES;
	MAXSATConfig(map<string, int> args)
	{
		addCst(SEED_GLOB);
		addCst(SEED_PROBLEM);
		addCst(SEED_ASSIGN);
		addCst(DATASET);
		addCst(FI_BI);
		addCst(IMPR);
		addCst(NUM_VARIABLES);
		addCst(NUM_CLAUSES);
	}
	vector<pair<string, double>> get_json() const override {
		return vector<pair<string, double>>{
			jMk(SEED_PROBLEM),
				jMk(SEED_ASSIGN),
				jMk(DATASET),
				jMk(FI_BI),
				jMk(IMPR),
				jMk(NUM_VARIABLES),
				jMk(NUM_CLAUSES)
		};
	}
	const int num_choices() const
	{
		return this->NUM_VARIABLES.get();
	}
};