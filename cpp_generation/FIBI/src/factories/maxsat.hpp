#pragma once
#include "abstract.hpp"
#include "../data/get_data.hpp"
#include "../data/solution_container/maxsat.hpp"
#include "../data/improvements/maxsat.hpp"
#include "../algorithm/stats/metrics.hpp"
#include "../algorithm/factories.hpp"
#include "../utils.h"
#include "../out/abstract.hpp"

using namespace std;

class MAXSATFactory : public AbstractFactory<MAXSATConfig> {
public:
	vector<pair<string, double>>* run(MAXSATConfig& cf, string root_data = "./") override
	{
		// setup
		vector<clause_t>* clauses_ptr = nullptr;
		vector<bool>* assignements_ptr = nullptr;
		vector<double>* weights_ptr = nullptr;
		int n_vars = 0;
		string dataset_name;
		if (cf.DATASET.get() == 0)
		{
			/* Uniformly distributed points */
			clauses_ptr = random_clauses(cf.NUM_VARIABLES.get(), cf.NUM_CLAUSES.get(), cf.SEED_PROBLEM.get());
			weights_ptr = random_weights(cf.NUM_CLAUSES.get(), cf.SEED_PROBLEM.get());
		}
		else if (cf.DATASET.get() == 1) {
			dataset_name = "maxsat_benchmark/";
			unique_ptr<const vector<double>> data(open_maxsat_benchmark(cf.SEED_PROBLEM.get(), root_data + dataset_name));
			auto [clauses_ptr_tmp, weights_ptr_tmp, n_vars] = parse_maxsat(*data);
			cf.NUM_VARIABLES.set(n_vars);
			cf.NUM_CLAUSES.set(clauses_ptr_tmp->size());
			clauses_ptr = clauses_ptr_tmp;
			weights_ptr = weights_ptr_tmp;
		}
		else {
			cout << "Invalid DATASET argument, Valid values are 0->1" << endl;
			throw invalid_argument("Invalid DATASET argument, Valid values are 0->1");
		}
		assignements_ptr = random_assignements(cf.NUM_VARIABLES.get(), cf.SEED_ASSIGN.get());
		unique_ptr<const vector<clause_t>> clauses(clauses_ptr);
		unique_ptr<vector<bool>> assignements(assignements_ptr);
		unique_ptr<const vector<double>> weights(weights_ptr);
		unique_ptr< const map<const var_id_t, vector<clause_id_t>>> var_to_clauses(get_var_to_clauses(*clauses));

		if (cf.IMPR.get() >= 1)
		{
			/* Slack improvement algorithm for MAXSAT */
			// Override the assignements with the improved ones
			AbstractContainerMAXSAT *c;
			if(cf.IMPR.get() == 2) {
				c = new GREEDYContainer(); 
			} else if(cf.IMPR.get() == 3) {
				c = new TOPKContainer<3>(cf.SEED_ASSIGN.get());
			} else if(cf.IMPR.get() == 4) {
				c = new TOPKContainer<4>(cf.SEED_ASSIGN.get());
			} else if(cf.IMPR.get() == 5) {
				c = new TOPKContainer<5>(cf.SEED_ASSIGN.get());
			} else if(cf.IMPR.get() == 1) {
				c = new RandomizedContainer(cf.SEED_ASSIGN.get());
			}
			assignements.reset(improve_maxsat(*var_to_clauses, *clauses, *weights, cf.NUM_VARIABLES.get(), cf.SEED_ASSIGN.get(),*c));
			delete c;
		}
		else if (cf.IMPR.get() > 1) {
			cout << "Invalid IMPR argument, Valid values are 0->1" << endl;
			throw invalid_argument("Invalid IMPR argument, Valid values are 0->1");
		}
		MAXSATSolutionContainer co(*weights, *assignements, *clauses);
		Metrics<MAXSATSolutionContainer> metrics;
		vector<AlgorithmObserver<MAXSATSolutionContainer> *> obs;
		obs.push_back(&metrics);
		AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>o(obs);
		MAXSATNeighbourhoodExplorer n(o, (bool)cf.FI_BI.get()); 
		FlipMAXSAT f;
		LocalSearch<MAXSATSwap,MAXSATSolutionContainer,MAXSATNeighbourhoodExplorer,MAXSATConfig,FlipMAXSAT> ls(n, o);
		ls.run(co,cf,f);
		vector<pair<string, double>>* res = get_results<MAXSATSolutionContainer>(&metrics, &cf);
		return res;
	}
};