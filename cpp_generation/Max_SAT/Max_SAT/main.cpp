#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <random>
#include <stdio.h>
#include <map>
#include <memory>

#include "test/tests.hpp"

/*
#include "MAXSATgenerator.h"
#include "Algorithm.h"
#include "AlgorithmObservable.h"
#include "AlgorithmObserver.h"
#include "result.h"
#include "Metrics.h"
#include "factories.h"
#include "progress.h"
#include "argParser.hpp"
*/

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <signal.h>
#endif
/*
#define M_insert(key, value) (*args)[stringify(key)] = value

int currBackup;
int stopBackup;

class ProductIterator
{
public:
	int current = 0;
	int maxIndex = 0;
	std::map<std::string, std::vector<int>> params_poss;
	std::vector<std::string> params_order;
	std::vector<int> index;
	ProductIterator() : params_poss(std::map<std::string, std::vector<int>>{}), current(0){};
	ProductIterator(std::map<std::string, std::vector<int>> params_poss, std::vector<std::string> params_order) : params_poss(params_poss), current(0)
	{
		std::reverse(params_order.begin(), params_order.end());
		this->params_order = params_order;
		this->index = std::vector<int>(params_order.size(), 0);
		maxIndex = 1;
		for (auto param_name : params_order)
		{
			maxIndex *= params_poss[param_name].size();
		}
		maxIndex--; // Max index not number of possibilites
	};

	std::map<std::string, int> next()
	{
		int res = current;
		if (current > maxIndex)
		{
			return std::map<std::string, int>{};
		}
		std::map<std::string, int> values_poss;
		int k = 0;
		for (int idx : index)
		{
			values_poss[params_order[k]] = params_poss[params_order[k]][idx];
			k++;
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
		return values_poss;
	}
};
class RunnerObserver
{
public:
	RunnerObserver(){};
	virtual std::map<std::string, int> *update(std::map<std::string, int> &args) = 0;
};
class LoggerScheduler : public RunnerObserver
{
private:
	int num_logs;
	int current_log;
	int i;
	int int_rep;

public:
	LoggerScheduler(int num_logs, const int int_rep = 1000) : num_logs(num_logs), current_log(0), i(0), int_rep(int_rep){};
	std::map<std::string, int> *update(std::map<std::string, int> &args)
	{
		if (i % int_rep == 0)
		{
			current_log = 0;
		}
		i++;
		current_log++;
		if (current_log < num_logs)
		{
			return new std::map<std::string, int>{{"log", 1}};
		}
		return new std::map<std::string, int>{{"log", 0}};
	}
};

class SeedLoggerScheduler : public RunnerObserver
{
private:
	std::vector<int> seeds_to_log;

public:
	SeedLoggerScheduler(std::vector<int> seeds_to_log) : seeds_to_log(seeds_to_log){};
	std::map<std::string, int> *update(std::map<std::string, int> &args)
	{
		if (std::find(seeds_to_log.begin(), seeds_to_log.end(), args["SEED_GLOB"]) != seeds_to_log.end())
		{
			return new std::map<std::string, int>{{"log", 1}};
		}
		return new std::map<std::string, int>{{"log", 0}};
	}
};

static std::vector<RunnerObserver *> _dummy_setter = std::vector<RunnerObserver *>{};
template <typename Config_T, typename Factory_T, typename FactoryObs_T>
class Runner
{
private:
	ProductIterator *it;
	std::map<std::string, int> constants;
	std::vector<RunnerObserver *> fill_args;

public:
	Runner(std::map<std::string, std::vector<int>> params_poss, std::vector<std::string> params_order, std::map<std::string, int> constants,
		   std::vector<RunnerObserver *> fill_args = _dummy_setter) : constants(constants), fill_args(fill_args)
	{
		this->it = new ProductIterator(params_poss, params_order);
	}
	~Runner()
	{
		delete it;
	}
	void run(const int start, const int stop)
	{
		if (start < 0 || stop < 0)
		{
			std::cout << "Max seed: " << this->it->maxIndex << std::endl;
			THROW_EXC1("End of program because negative seed (check for max_seed)", -5);
		}
		stopBackup = stop;
		int seed_glob = 0;
		Progress p(start, stop);
		std::map<std::string, int> args;
		while (!(args = this->it->next()).empty())
		{
			currBackup = seed_glob;
			args.insert(std::pair<std::string, int>(stringify(SEED_GLOB), seed_glob));
			args.insert(constants.begin(), constants.end());
			bool log = false;
			for (RunnerObserver *f : fill_args)
			{
				std::map<std::string, int> *m = f->update(args);
				if (!log)
				{
					// Check if "log" in m
					auto it = m->find("log");
					if (it != m->end())
					{
						log = it->second;
					}
				}
				delete m;
			}
			if (seed_glob >= start && seed_glob <= stop)
			{
				Config_T conf(args);
				Factory_T f(std::vector<FactoryObs_T *>{});
				f.run(&conf, seed_glob == 0, log);
				std::cout << "done seed_glob: " << seed_glob << " ; ";
				p.print(seed_glob);
				std::cout << std::endl;
			}
			seed_glob++;
		};
		std::cout << "seed_glob end " << seed_glob - 1 << std::endl;
	};
};
#define P_SETUP                                          \
	std::map<std::string, std::vector<int>> params_poss; \
	std::vector<std::string> params_order;
#define P_REGISTER_PARAMS(param_name, param_values) \
	params_poss[param_name] = param_values;         \
	params_order.push_back(param_name);
#define P_FILL_RANGE0(table, end) \
	for (int i = 0; i < end; i++) \
	{                             \
		table.push_back(i);       \
	}
#define P_FILL_RANGE1(table, start, end, step) \
	for (int i = start; i < end; i += step)    \
	{                                          \
		table.push_back(i);                    \
	}
class SetSeedProblem : public RunnerObserver
{
public:
	SetSeedProblem() : RunnerObserver(){};
	std::map<std::string, int> *update(std::map<std::string, int> &args)
	{
		(args)[stringify(SEED_PROBLEM)] = (args)[stringify(SEED_ASSIGN)];
		return new std::map<std::string, int>{};
	}
};
class SetSeedAssign : public RunnerObserver
{
public:
	SetSeedAssign() : RunnerObserver(){};
	std::map<std::string, int> *update(std::map<std::string, int> &args)
	{
		(args)[stringify(SEED_ASSIGN)] = (args)[stringify(SEED_PROBLEM)];
		return new std::map<std::string, int>{};
	}
};
#define P_DONT_LOG ;
#define P_LOG_10 \
	if (l)       \
		callback.push_back(l);
void maxsat_random(int start, int end, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 0}};

	std::vector<int> num_variables;
	P_FILL_RANGE1(num_variables, 1000, 5001, 1000)
	std::vector<int> num_clauses;
	P_FILL_RANGE1(num_clauses, 1000, 5001, 1000)
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1};
	std::vector<int> seed_assign;
	P_FILL_RANGE0(seed_assign, 1000)

	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	SetSeedProblem s;
	std::vector<RunnerObserver *> callback{&s};
	P_LOG_10
	Runner<MAXSATConfig, MAXSATFactory, AbstractMAXSATFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, end);
}
void maxsat_evaluation_benchmark(int start, int stop, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 1}};
	const int NUM_INST = 29;
	const int NUM_REP = 1000;
	std::vector<int> seed_problem;
	P_FILL_RANGE0(seed_problem, NUM_INST)
	std::vector<int> seed_assign;
	P_FILL_RANGE0(seed_assign, NUM_REP)
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1};
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	std::vector<RunnerObserver *> callback{};
	P_LOG_10
	Runner<MAXSATConfig, MAXSATFactory, AbstractMAXSATFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, stop);
}
void clustering_quad(int start, int end, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 0}, {stringify(NUM_DIM), 2}};

	const int NUM_REP = 1000;
	std::vector<int> num_pts{20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
	std::vector<int> clust{2, 4, 8, 16, 32, 64, 128, 256};
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1, 2};
	std::vector<int> seed_problem;
	P_FILL_RANGE0(seed_problem, 1000)

	P_REGISTER_PARAMS(stringify(NUM_POINTS), num_pts);
	P_REGISTER_PARAMS(stringify(NUM_CLUST), clust);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);

	SetSeedAssign s;
	std::vector<RunnerObserver *> callback{&s};
	P_LOG_10
	Runner<ClusteringConfig, CluteringFactory, AbstractClusteringFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, end);
}
void clustering_norm(int start, int end, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 3}, {stringify(NUM_DIM), 2}};

	const int NUM_REP = 1000;
	std::vector<int> num_pts{20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
	std::vector<int> clust{2, 4, 8, 16, 32, 64, 128, 256};
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1, 2};
	std::vector<int> seed_problem;
	P_FILL_RANGE0(seed_problem, 1000)

	P_REGISTER_PARAMS(stringify(NUM_POINTS), num_pts);
	P_REGISTER_PARAMS(stringify(NUM_CLUST), clust);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);

	SetSeedAssign s;
	std::vector<RunnerObserver *> callback{&s};
	P_LOG_10
	Runner<ClusteringConfig, CluteringFactory, AbstractClusteringFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, end);
}
void clustering_aloise(int start, int stop, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 2}, {stringify(NUM_DIM), 2}};
	const int NUM_INST = 25;
	const int NUM_REP = 1000;
	std::vector<int> clust{2, 4, 8, 16, 32, 64, 128, 256};
	std::vector<int> seed_problem;
	P_FILL_RANGE0(seed_problem, NUM_INST)
	std::vector<int> seed_assign;
	P_FILL_RANGE0(seed_assign, NUM_REP)
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1};
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(NUM_CLUST), clust);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	std::vector<RunnerObserver *> callback{};
	P_LOG_10
	Runner<ClusteringConfig, CluteringFactory, AbstractClusteringFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, stop);
}

void clustering_franti(int start, int stop, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 1}};
	const int NUM_REP = 1000;
	std::vector<int> seed_problem{0, 1, 2, 8, 9, 10, 11, 12, 13, 124, 125, 126, 127, 128};
	std::vector<int> seed_assign;
	P_FILL_RANGE0(seed_assign, NUM_REP)
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1};
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	std::vector<RunnerObserver *> callback{};
	P_LOG_10
	Runner<ClusteringConfig, CluteringFactory, AbstractClusteringFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, stop);
}
void tsp_quad(const int start, const int stop, RunnerObserver *l)
{
	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 0}, {stringify(NUM_DIM), 2}};

	const int NUM_REP = 1000;
	std::vector<int> num_pts{20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1, 2, 3, 4};
	std::vector<int> seed_problem;
	P_FILL_RANGE0(seed_problem, 1000)

	P_REGISTER_PARAMS(stringify(NUM_TOWNS), num_pts);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);

	SetSeedAssign s;
	std::vector<RunnerObserver *> callback{&s};
	P_LOG_10
	Runner<TSPConfig, TSPFactory, AbstractTSPFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, stop);
}
void tsp_tsplib_all(const int start, const int stop, RunnerObserver *l)
{

	P_SETUP
	std::map<std::string, int> constants{{stringify(DATASET), 1}, {stringify(NUM_DIM), 2}};
	const int NUM_INST = 49;
	const int NUM_REP = 1000;
	std::vector<int> seed_problem;
	P_FILL_RANGE0(seed_problem, NUM_INST)
	std::vector<int> seed_assign;
	P_FILL_RANGE0(seed_assign, NUM_REP)
	std::vector<int> fi_bi{0, 1};
	std::vector<int> impr{0, 1, 2, 3, 4};
	P_REGISTER_PARAMS(stringify(SEED_PROBLEM), seed_problem);
	P_REGISTER_PARAMS(stringify(FI_BI), fi_bi);
	P_REGISTER_PARAMS(stringify(IMPR), impr);
	P_REGISTER_PARAMS(stringify(SEED_ASSIGN), seed_assign);

	std::vector<RunnerObserver *> callback{};
	P_LOG_10
	Runner<TSPConfig, TSPFactory, AbstractTSPFactoryObserver> runner(params_poss, params_order, constants, callback);
	runner.run(start, stop);
}
RunnerObserver *parse_logger(const int argc, char **argv)
{
	if (cmdOptionExists(argv, argv + argc, "-logSeed"))
	{
		return new LoggerScheduler(10, 1000);
	}
	if (cmdOptionExists(argv, argv + argc, "-logSeedSpec"))
	{
		char *seedsSelected = getCmdOption(argv, argv + argc, "-logSeedSpec");
		// seedsSelected could be
		// 1,2,3,4,5 -> vector should contain 1,2,3,4,5
		// 1-5 -> vector should contain 1,2,3,4,5
		// 1-5,7,9-10 -> vector should contain 1,2,3,4,5,7,9,10
		std::vector<int> seeds;
		std::string seedsSelectedStr(seedsSelected);
		std::stringstream ss(seedsSelectedStr);
		std::string token;
		while (std::getline(ss, token, ','))
		{
			if (token.find('-') != std::string::npos)
			{
				std::stringstream ss2(token);
				std::string token2;
				std::getline(ss2, token2, '-');
				int start = std::stoi(token2);
				std::getline(ss2, token2, '-');
				int stop = std::stoi(token2);
				for (int i = start; i <= stop; i++)
				{
					seeds.push_back(i);
				}
			}
			else
			{
				seeds.push_back(std::stoi(token));
			}
		}
		// Make an optional object and build the SeedLoggerScheduler
		return new SeedLoggerScheduler(seeds);
	}

	return {};
}
#define P_SAFE_EXECUTION
void print_argv(int argc, char **argv)
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i];
		if (i != argc - 1)
		{
			std::cout << " ";
		}
	}
}
#define INVALID_ARGS(route, possible)                                                   \
	std::cout << "Invalid/Missing argument. Provided: ";                                \
	print_argv(argc, argv);                                                             \
	std::cout << std::endl                                                              \
			  << "Possible arguments for `" << route << "`: " << possible << std::endl; \
	return 1;
#ifdef _WIN32
BOOL WINAPI consoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT)
	{
		std::cout << "Args to restart: -s " << currBackup << " -e " << stopBackup << std::endl;
		exit(1);
	}
	return TRUE;
}
#elif __linux__
void consoleHandler(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "Args to restart: -s " << currBackup << " -e " << stopBackup << std::endl;
		exit(1);
	}
}
#endif
*/
int main(int argc, char **argv)
{
	/*
#ifdef _WIN32
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
	{
		std::cout << "ERROR: Could not set control handler" << std::endl;
		return 1;
	};
#elif __linux__
	signal(SIGINT, consoleHandler);
#endif
	if (P_FLAG_SEEN("-test"))
	{
		Tests test = Tests();
		test.run();
	}
	else if (P_FLAG_SEEN("-help"))
	{
		std::cout << "Possible options: " << std::endl;
		std::cout << "-help" << std::endl;
		std::cout << "-test" << std::endl;
		std::cout << "-tsp 		-s <int> -e <int> 	-quad" << std::endl;
		std::cout << "                     			-tsplib 	-rand" << std::endl;
		std::cout << "                     			-tsplib 	-greedy" << std::endl;
		std::cout << "-clust 	-s <int> -e <int> 	-quad" << std::endl;
		std::cout << "                         		-aloise" << std::endl;
		std::cout << "                         		-franti" << std::endl;
		std::cout << "-maxsat 	-s <int> -e <int> 	-rand_gen" << std::endl;
		std::cout << "                         		-bench" << std::endl;
		std::cout << "Additionnal flag: " << std::endl;
		std::cout << "-logSeed for 10 logs every 1000 seeds" << std::endl;
		std::cout << "-logSeedSpec <int>-<int>,<int>-<int>,... for ranges of seeds" << std::endl;
	}
	else
	{
#ifdef P_SAFE_EXECUTION
		try
		{
#endif
			const int start = std::atoi(P_GET_VAL("-s"));
			const int end = std::atoi(P_GET_VAL("-e"));
			std::unique_ptr<RunnerObserver> observer(parse_logger(argc, argv));
			if (P_FLAG_SEEN("-tsp"))
			{
				if (P_FLAG_SEEN("-quad"))
				{
					std::cout << "TSPquad" << std::endl;
					tsp_quad(start, end, observer.get());
					return 0;
				}
				else if (P_FLAG_SEEN("-tsplib"))
				{
					std::cout << "TSPtsplibrand" << std::endl;
					tsp_tsplib_all(start, end, observer.get());
					return 0;
				}
			}
			else if (P_FLAG_SEEN("-clust"))
			{
				if (P_FLAG_SEEN("-quad"))
				{
					std::cout << "Clusteringquad" << std::endl;
					clustering_quad(start, end, observer.get());
					return 0;
				}
				else if (P_FLAG_SEEN("-quadNorm"))
				{
					std::cout << "Clustering quad normal distribution" << std::endl;
					clustering_norm(start, end, observer.get());
					return 0;
				}
				else if (P_FLAG_SEEN("-aloise"))
				{
					std::cout << "Clusteringaloise" << std::endl;
					clustering_aloise(start, end, observer.get());
					return 0;
				}
				else if (P_FLAG_SEEN("-franti"))
				{
					std::cout << "Clusteringfranti" << std::endl;
					clustering_franti(start, end, observer.get());
					return 0;
				}
				else
				{
					INVALID_ARGS("-clust", "-quad, -quadNorm, -aloise, -franti");
				}
			}
			else if (P_FLAG_SEEN("-maxsat"))
			{
				if (P_FLAG_SEEN("-rand_gen"))
				{
					std::cout << "Maxsatrand_gen" << std::endl;
					maxsat_random(start, end, observer.get());
					return 0;
				}
				else if (P_FLAG_SEEN("-bench"))
				{
					std::cout << "Maxsatbench" << std::endl;
					maxsat_evaluation_benchmark(start, end, observer.get());
					return 0;
				}
				else
				{
					INVALID_ARGS("-maxsat", "-rand_gen, -bench");
				}
			}
			std::vector<std::string> flags(argv, argv + argc);
			flags.insert(flags.begin(), "Unrecognized flags: ");
			throw GenericException(flags);
#ifdef P_SAFE_EXECUTION
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
#endif
	}*/
	run_tests();
	return 0;
}