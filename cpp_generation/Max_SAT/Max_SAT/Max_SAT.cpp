#include <iostream>
#include "test.h"
#include "MAXSATgenerator.h"
#include "Algorithm.h"
#include "AlgorithmObservable.h"
#include "AlgorithmObserver.h"
#include "result.h"
#include "Metrics.h"
#include "H5Cpp.h"
#include "factories.h"
#include "MAXSATgenerator.h"
#include <cstdlib>
#include <string>
#include "progress.h"
#define M_insert(key, value) (*args)[stringify(key)] = value

class ProductIterator
{
public:
	int current = 0;
	int maxIndex = 0;
	std::map<std::string, std::vector<int>> params_poss;
	std::vector<std::string> params_order;
	std::vector<int> index;
	ProductIterator() : params_poss(std::map<std::string, std::vector<int>>{}), current(0){};
	ProductIterator(std::map<std::string, std::vector<int>> params_poss, std::vector<std::string> params_order) : params_order(params_order), params_poss(params_poss), current(0)
	{
		std::vector<int> index(params_order.size(), 0);
		maxIndex = 1;
		for (auto param_name : params_order)
		{
			maxIndex *= params_poss[param_name].size();
		}
	};
	std::map<std::string, int> next()
	{
		int res = current;
		if (current > maxIndex)
		{
			return std::map<std::string, int>{};
		}
		// Increment index
		for (int i = 0; i < index.size(); i++)
		{
			if (index[i] < params_poss[params_order[i]].size() - 1)
			{
				index[i]++;
				break;
			}
			else
			{
				index[i] = 0;
			}
		}
		current++;
		std::map<std::string, int> values_poss;
		for (int idx : index)
		{
			values_poss[params_order[idx]] = params_poss[params_order[idx]][idx];
		}
		return values_poss;
	}
};
template <class Config_T, class Factory_T, class FactoryObs_T>
class Runner
{
private:
	ProductIterator it;
	std::map<std::string, int> constants;

public:
	Runner(std::vector<std::vector<int>> params_poss, std::vector<std::string> params_order, std::map<std::string, int> constants,
		std::vector<void (std::map<string,int>&)> fill_args = std::vector<void (std::map<string,int>&)>{}
	) : constants(constants)
	{
		this->it = ProductIterator(params_poss, params_order);
	};
	void run(const int start, const int stop)
	{
		const int NUM_REP = 1000;
		int seed_glob = 0;
		Progress p(start, stop);
		std::map<std::string, int> args;
		while (!(args = this->it.next()).empty())
		{
			args.insert(std::pair<std::string, int>(stringify(SEED_GLOB), seed_glob));
			args.insert(constants.begin(), constants.end());
			for (auto f : fill_args)
			{
				f(args);
			}
			if (seed_glob >= start && seed_glob <= stop)
			{
				Config_T conf(&args);
				Factory_T f(std::vector<FactoryObs_T *>{});
				f.run(&conf, seed_glob == 0);
				std::cout << "seed_glob: " << seed_glob << " ; ";
				p.print(seed_glob);
				std::cout << std::endl;
			}
			seed_glob++;
		};
		std::cout << "seed_glob end " << seed_glob - 1 << std::endl;
	};
};
#define P_SETUP std::map<std::string, int> params_poss;std::vector<std::string> params_order;
#define P_REGISTER_PARAMS(param_name, param_values) \
	params_poss[param_name] = param_values;								  \
	params_order.push_back(param_name);
#define P_FILL_RANGE(table, end) \
	for (int i = start; i < end; i+=step) \
	{							  \
		table.push_back(i);		  \
	}
#define P_FILL_RANGE(table, start, end, step) \
	for (int i = start; i < end; i+=step) \
	{							  \
		table.push_back(i);		  \
	}
void run_instance_random_greedy_51_familles(int start, int stop)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 1}};
	const int NUM_INST = 24;
	const int NUM_REP = 1000;
	std::vector<int> seed_problem;
	P_FILL_RANGE(seed_problem, NUM_INST)
	std::vector<int> seed_assign;
	P_FILL_RANGE(seed_problem, NUM_REP)
	std::vector<int> fi_bi{0, 1 };
	std::vector<int> impr{0, 1 };
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI),  fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	Runner<MAXSATConfig, MAXSATFactory, AbstractMAXSATFactoryObserver> runner(params_poss, params_order, constants);
	runner.run(start, stop);
}
void test_generate_random(int start, int end)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 0}};

	std::vector<int> num_variables;
	P_FILL_RANGE(num_variables, 1000,5001,1000)
	std::vector<int> num_clauses;
	P_FILL_RANGE(num_clauses, 1000,5001,1000)
	std::vector<int> fi_bi{0, 1 };
	std::vector<int> impr{0, 1 };
	std::vector<int> num_clauses;
	P_FILL_RANGE(num_clauses, 1000)
	P_REGISTER_PARAMS(stringify(NUM_VARIABLES), seed_problem);
	P_REGISTER_PARAMS(stringify(NUM_CLAUSES), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI),  fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);
	
	auto set_seed_problem = [](std::map<std::string, int>& args)
	{
		args[stringify(SEED_PROBLEM)] = args[stringify(SEED_ASSIGN)];
	};
	Runner<MAXSATConfig, MAXSATFactory, AbstractMAXSATFactoryObserver> runner(params_poss, params_order, constants, set_seed_problem);
	runner.run(start, stop);
}
void test_generate_benchmark2(int start, int stop)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 3}};
	const int NUM_INST = 125;
	const int NUM_REP = 1000;
	std::vector<int> seed_problem;
	P_FILL_RANGE(seed_problem, NUM_INST)
	std::vector<int> seed_assign;
	P_FILL_RANGE(seed_problem, NUM_REP)
	std::vector<int> fi_bi{0, 1 };
	std::vector<int> impr{0, 1 };
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI),  fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	Runner<MAXSATConfig, MAXSATFactory, AbstractMAXSATFactoryObserver> runner(params_poss, params_order, constants);
	runner.run(start, stop);
}

void test_generate_benchmark3(int start, int stop)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 4}};
	const int NUM_INST = 8;
	const int NUM_REP = 1000;
	std::vector<int> seed_problem;
	P_FILL_RANGE(seed_problem, NUM_INST)
	std::vector<int> seed_assign;
	P_FILL_RANGE(seed_problem, NUM_REP)
	std::vector<int> fi_bi{0, 1 };
	std::vector<int> impr{0, 1 };
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI),  fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	Runner<MAXSATConfig, MAXSATFactory, AbstractMAXSATFactoryObserver> runner(params_poss, params_order, constants);
	runner.run(start, stop);
}
void test_clustering_quad(int start, int end)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 0},{stringify(NUM_DIM),2}};

	const int NUM_REP = 1000;
	std::vector<int> num_pts{20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
	std::vector<int> clust{2, 4, 8, 16, 32, 64, 128, 256};
	std::vector<int> fi_bi{0, 1 };
	std::vector<int> impr{0, 1 };
	std::vector<int> seed_problem;
	P_FILL_RANGE(seed_problem, 1000)

	P_REGISTER_PARAMS(stringify(NUM_POINTS), num_pts);
	P_REGISTER_PARAMS(stringify(NUM_CLUST), clust);
	P_REGISTER_PARAMS(stringify(FI_BI),  fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	
	auto set_seed_assign = [](std::map<std::string, int>& args)
	{
		args[stringify(SEED_ASSIGN)] = args[stringify(SEED_PROBLEM)];
	};
	Runner<ClusteringConfig, CluteringFactory, AbstractClusteringFactoryObserver> runner(params_poss, params_order, constants, set_seed_assign);
	runner.run(start, stop);
}
void test_tsp_quad(const int start, const int stop)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 0},{stringify(NUM_DIM),2}};

	const int NUM_REP = 1000;
	std::vector<int> num_pts{20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
	std::vector<int> fi_bi{0, 1 };
	std::vector<int> impr{0, 1 };
	std::vector<int> seed_problem;
	P_FILL_RANGE(seed_problem, 1000)

	P_REGISTER_PARAMS(stringify(NUM_TOWNS), num_pts);
	P_REGISTER_PARAMS(stringify(FI_BI),  fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	
	auto set_seed_assign = [](std::map<std::string, int>& args)
	{
		args[stringify(SEED_ASSIGN)] = args[stringify(SEED_PROBLEM)];
	};
	Runner<ClusteringConfig, CluteringFactory, AbstractClusteringFactoryObserver> runner(params_poss, params_order, constants, set_seed_assign);
	runner.run(start, stop);
}
std::pair<int, int> parse_interval(int argc, char **argv)
{
	if (argc == 3)
	{
		return std::make_pair(std::atoi(argv[1]), std::atoi(argv[2]));
	}
	THROW_EXC2("Invalid number of parameters: expected 3 received ", std::to_string(argc), 2000);
}
#define TEST
int main(int argc, char **argv)
{
#ifndef TEST
	try
	{
		std::pair<int, int> start_stop = parse_interval(argc, argv);
		test_clustering_quad(start_stop.first, start_stop.second);
	}
	catch (KeyNotFound &e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	catch (GenericException &e)
	{
		std::cout << e.what() << std::endl;
		return 2;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return 3;
	}
	catch (...)
	{
		std::cout << "other exception" << std::endl;
		return 4;
	}
	return 0;
#else
	Tests test = Tests();
	test.run();
	return 0;
#endif
}