#include "test.h"

#define TEST_MACRO(name) \
	void name()          \
	{
#define REGISTER(fn)                 \
	functions_to_run.push_back(&fn); \
	functions_names.push_back(#fn);
void Tests::run()
{
	std::vector<void (*)()> functions_to_run;
	std::vector<std::string> functions_names;
	REGISTER(test_clustering);
	REGISTER(test_tsp);
	REGISTER(test_tsp_50towns_rand);
	REGISTER(test_tsp_50towns_impr);
	REGISTER(test_clustering_50points);
	REGISTER(test_maxsat_50clauses_50vars);

	for (int i = 0; i < functions_names.size(); i++)
	{
		try
		{
			functions_to_run.at(i)();
			std::cout << "\x1B[32m " << functions_names.at(i) << ":OK \033[0m " << std::endl;
		}
		catch (GenericException &e)
		{
			std::cout << "\x1B[31m " << functions_names.at(i) << ":Error " << e.what() << " \033[0m " << std::endl;
		}
		catch (KeyNotFound &e)
		{
			std::cout << "\x1B[31m " << functions_names.at(i) << ":Error " << e.what() << " \033[0m " << std::endl;
		}
		catch (TooLittleElements &e)
		{
			std::cout << "\x1B[31m " << functions_names.at(i) << ":Error " << e.what() << " \033[0m " << std::endl;
		}
		catch (TooManyElements &e)
		{
			std::cout << "\x1B[31m " << functions_names.at(i) << ":Error " << e.what() << " \033[0m " << std::endl;
		}
	}
}

#define M_insert(key, value) (*args)[stringify(key)] = value
void test_clustering()
{
	std::vector<double> cost_found;
	JSONStorage storage;
	for (int FI = 0; FI < 2; FI++)
	{
		// Print in color the algorithm (0:BI, 1:FI) with the name and not the number
		std::cout << "\x1B[34m ******************************************" << (FI == 0 ? "BI" : "FI") << "****************************************** \033[0m " << std::endl;
		std::map<std::string, int> *args = new std::map<std::string, int>();
		M_insert(DATASET, 0);
		M_insert(FI_BI, FI);
		M_insert(NUM_DIM, 2);
		M_insert(IMPR, 0);
		M_insert(SEED_GLOB, 0);
		M_insert(SEED_ASSIGN, 0);
		M_insert(SEED_PROBLEM, 0);
		ClusteringConfig *conf = new ClusteringConfig(*args);
		conf->NUM_POINTS.set(4);
		conf->NUM_CLUST.set(2);
		conf->NUM_DIM.set(2);
		double *p = test_points();
		int *c = test_clusters();
		ClusteringContainer *cont = new ClusteringContainer(
			p,
			c,
			conf);
		cont->on_improvement_done();
		Metrics *metrics = new Metrics();
		ClusteringResult *res = new ClusteringResult(conf, metrics);
		std::vector<AlgorithmObserver<ClusteringContainer, ClusteringSwap> *> observers;
		observers.push_back(res);
		ClusteringLogger logger(&storage);
		observers.push_back(&logger);
		ClusteringAlgorithm *alg = new ClusteringAlgorithm(observers);
		alg->run(cont, conf, conf->FI_BI.get());
		cost_found.push_back(metrics->final_cost);
		delete alg;
		delete metrics;
		delete res;
		delete cont;
	}
	storage.save_json("test_clustering.json");
	if (abs(cost_found[0] - 2301.1417898981035) > 1e-3 || abs(cost_found[1] - 2645.091033340276) > 1e-3)
	{
		THROW_EXC9("Clustering cost not correct (BI=", std::to_string(cost_found[0]), ",FI=", std::to_string(cost_found[1]), ") vs expected (BI=", std::to_string(2398.4242169551007), ",FI=", std::to_string(3461.417895607281), ")", 1024);
	}
}
void test_clustering_50points()
{
	JSONStorage storage;
	for (int impr = 0; impr < 2; impr++)
	{
		for (int FI = 0; FI < 2; FI++)
		{
			// Print in color the algorithm (0:BI, 1:FI) with the name and not the number
			std::cout << "\x1B[34m ******************************************" << (FI == 0 ? "BI" : "FI") << "****************************************** \033[0m " << std::endl;
			std::map<std::string, int> *args = new std::map<std::string, int>();
			M_insert(DATASET, 0);
			M_insert(FI_BI, FI);
			M_insert(NUM_DIM, 2);
			M_insert(IMPR, impr);
			M_insert(SEED_GLOB, 0);
			M_insert(SEED_ASSIGN, 0);
			M_insert(SEED_PROBLEM, 0);
			ClusteringConfig *conf = new ClusteringConfig(*args);
			conf->NUM_POINTS.set(50);
			conf->NUM_CLUST.set(4);
			conf->NUM_DIM.set(2);
			ClusteringContainer *cont;
			/* Uniformly distrbuted points */
			cont = new ClusteringContainer(
			uniform_points(conf->NUM_POINTS.get(),
						   conf->NUM_DIM.get(),
						   conf->SEED_PROBLEM.get()),
			random_clust(conf), conf);
			if (conf->IMPR.get() == 1)
			{
				kmeansPlusPlus(cont, conf);
			}
			else if (conf->IMPR.get() > 1)
			{
				THROW_EXC1("Invalid IMPR argument, Valid values are 0->1", 11);
			}
			cont->on_improvement_done();
			Metrics *metrics = new Metrics();
			ClusteringResult *res = new ClusteringResult(conf, metrics);
			// algorithms execution
			std::vector<AlgorithmObserver<ClusteringContainer, ClusteringSwap>*> observers;
			observers.push_back(res);
			ClusteringLogger logger(&storage);
			observers.push_back(&logger);
			ClusteringAlgorithm *alg = new ClusteringAlgorithm(observers);
			alg->run(cont,conf,conf->FI_BI.get());
			// cleaning
			delete alg;
			delete metrics;
			delete res;
			delete cont;
		}
	}
	storage.save_json("test_clustering_50points.json");
}
void test_tsp()
{
	// Print in color the algorithm (0:BI, 1:FI) with the name and not the number
	std::map<std::string, int> *args = new std::map<std::string, int>();
	M_insert(DATASET, 3);
	M_insert(FI_BI, 0);
	M_insert(NUM_DIM, 2);
	M_insert(NUM_TOWNS, 7);
	M_insert(IMPR, 0);
	M_insert(SEED_GLOB, 0);
	M_insert(SEED_ASSIGN, 0);
	M_insert(SEED_PROBLEM, 0);
	TSPConfig *conf = new TSPConfig(*args);
	const double *p = test_towns();
	int *c = test_tour();
	DistanceMatrix m(conf->NUM_TOWNS.get(), conf->NUM_DIM.get(), p);
	TSPContainer *cont = new TSPContainer(
		c, &m, conf);
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	TSPResult *res = new TSPResult(conf, metrics);
	std::vector<AlgorithmObserver<TSPContainer,TSPSwap>*> observers;
	observers.push_back(res);
	JSONStorage storage;
	TSPLogger logger(&storage);
	observers.push_back(&logger);

	TSPAlgorithm *alg = new TSPAlgorithm(observers, 20);
	alg->run(cont, conf, conf->FI_BI.get());
	storage.save_json("test_tsp.json");
	delete alg;
	delete metrics;
	delete res;
	delete cont;
}
void test_tsp_50towns_rand()
{
	// Print in color the algorithm (0:BI, 1:FI) with the name and not the number
	std::map<std::string, int> *args = new std::map<std::string, int>();
	M_insert(DATASET, 0);
	M_insert(FI_BI, 0);
	M_insert(NUM_DIM, 2);
	M_insert(NUM_TOWNS, 50);
	M_insert(IMPR, 0);
	M_insert(SEED_GLOB, 0);
	M_insert(SEED_ASSIGN, 0);
	M_insert(SEED_PROBLEM, 0);
	TSPConfig *conf = new TSPConfig(*args);
	const double *p = uniform_points(conf->NUM_TOWNS.get(),
									 conf->NUM_DIM.get(),
									 conf->SEED_PROBLEM.get());
	int *c = random_tour(conf->NUM_TOWNS.get(), conf->NUM_DIM.get());
	DistanceMatrix m(conf->NUM_TOWNS.get(), conf->NUM_DIM.get(), p);
	TSPContainer *cont = new TSPContainer(
		c, &m, conf);
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	TSPResult *res = new TSPResult(conf, metrics);
	std::vector<AlgorithmObserver<TSPContainer, TSPSwap>*> observers;
	observers.push_back(res);
	JSONStorage storage;
	TSPLogger logger(&storage);
	observers.push_back(&logger);

	TSPAlgorithm *alg = new TSPAlgorithm(observers, 100);
	alg->run(cont, conf, conf->FI_BI.get());
	storage.save_json("test_tsp_50towns_rand.json");
	delete alg;
	delete metrics;
	delete res;
	delete cont;
}
void test_tsp_50towns_impr()
{
	// Print in color the algorithm (0:BI, 1:FI) with the name and not the number
	std::map<std::string, int> *args = new std::map<std::string, int>();
	M_insert(DATASET, 0);
	M_insert(FI_BI, 0);
	M_insert(NUM_DIM, 2);
	M_insert(NUM_TOWNS, 50);
	M_insert(IMPR, 0);
	M_insert(SEED_GLOB, 0);
	M_insert(SEED_ASSIGN, 0);
	M_insert(SEED_PROBLEM, 0);
	TSPConfig *conf = new TSPConfig(*args);
	const double *p = uniform_points(conf->NUM_TOWNS.get(),
									 conf->NUM_DIM.get(),
									 conf->SEED_PROBLEM.get());
	DistanceMatrix m(conf->NUM_TOWNS.get(), conf->NUM_DIM.get(), p);
	int *c = improved_tour(conf->NUM_TOWNS.get(), conf->NUM_DIM.get(), &m);
	TSPContainer *cont = new TSPContainer(
		c, &m, conf);
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	TSPResult *res = new TSPResult(conf, metrics);
	std::vector<AlgorithmObserver<TSPContainer, TSPSwap>*> observers;
	observers.push_back(res);
	JSONStorage storage;
	TSPLogger logger(&storage);
	observers.push_back(&logger);

	TSPAlgorithm *alg = new TSPAlgorithm(observers, 100);
	alg->run(cont, conf, conf->FI_BI.get());
	delete alg;
	delete metrics;
	delete res;
	delete cont;
	storage.save_json("test_tsp_50towns_impr.json");
}
void test_maxsat_50clauses_50vars(){
	
	// Print in color the algorithm (0:BI, 1:FI) with the name and not the number
	std::map<std::string, int> *args = new std::map<std::string, int>();
	M_insert(DATASET, 0);
	M_insert(NUM_VARIABLES, 50);
	M_insert(NUM_CLAUSES, 50);
	M_insert(FI_BI, 0);
	M_insert(IMPR, 0);
	M_insert(SEED_GLOB, 0);
	M_insert(SEED_ASSIGN, 0);
	M_insert(SEED_PROBLEM, 0);
	MAXSATConfig *conf = new MAXSATConfig(*args);
	MAXSATContainer *cont;
	const int min_num_var_per_clause = 1;
	const int max_num_var_per_clause = 25;
	std::pair<std::vector<clause_t> *, double *> clauses = generate_random_clauses(
		conf->NUM_VARIABLES.get(),
		conf->NUM_CLAUSES.get(),
		conf->SEED_PROBLEM.get(),
		min_num_var_per_clause, max_num_var_per_clause);
	bool *init_assign = generate_random_var_assign(conf->NUM_VARIABLES.get(), conf->SEED_ASSIGN.get());
	cont = new MAXSATContainer(clauses.second, conf->NUM_VARIABLES.get(), init_assign, *clauses.first, conf);
	delete[] init_assign;
	delete[] clauses.second;
	delete clauses.first;
	if (conf->IMPR.get() == 1)
	{
		improve(cont, conf->SEED_ASSIGN.get());
	}
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	MAXSATResult *res = new MAXSATResult(conf, metrics);
	// algorithms execution
	std::vector<AlgorithmObserver<MAXSATContainer,MAXSATSwap> *> observers;
	observers.push_back(res);
	JSONStorage storage;
	MAXSATLogger logger(&storage, true);
	observers.push_back(&logger);
	MAXSATAlgorithm *alg = new MAXSATAlgorithm(observers);
	alg->run(cont, conf, (bool)conf->FI_BI.get());
	delete alg;
	delete metrics;
	delete res;
	delete cont;
	storage.save_json("test_maxsat.json");
}