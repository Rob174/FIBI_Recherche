#include "factories.h"

void MAXSATFactory::run(MAXSATConfig *conf, bool dump_mapping, const int min_var_per_clause, const int max_var_per_clause)
{
	// setup
	conf->check_keys_before_load_instance();
	MAXSATContainer *cont;
	if (M_cst_frc(DATASET) == 0)
	{
		/* Generated dataset */
		std::pair<std::vector<clause_t> *, double *> clauses = generate_random_clauses(
			M_cst_frc(NUM_VARIABLES),
			M_cst_frc(NUM_CLAUSES),
			M_cst_frc(SEED_PROBLEM),
			min_var_per_clause, max_var_per_clause);
		bool *init_assign = generate_random_var_assign(M_cst_frc(NUM_VARIABLES), M_cst_frc(SEED_ASSIGN));
		cont = new MAXSATContainer(clauses.second, M_cst_frc(NUM_VARIABLES), init_assign, *clauses.first, conf);
		delete[] init_assign;
		delete[] clauses.second;
		delete clauses.first;
	}
	else if (M_cst_frc(DATASET) == 1)
	{
		int *data = load_points<int, true>("maxsat_benchmarks.hdf5", M_cst_frc(SEED_PROBLEM));
		// std::tuple<std::vector<clause_t>*, weight_t*, n_vars_t>
		auto values = parse_maxsat(data);
		delete[] data;
		conf->update_keys(std::map<std::string, int>{{"NUM_VARIABLES", std::get<2>(values)}, {"NUM_CLAUSES", (*std::get<0>(values)).size()}});
		bool *init_assign = generate_random_var_assign(M_cst_frc(NUM_VARIABLES), M_cst_frc(SEED_ASSIGN));
		cont = new MAXSATContainer(std::get<1>(values), M_cst_frc(NUM_VARIABLES), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else if (M_cst_frc(DATASET) == 2)
	{
		int *data = load_fixed_clauses();
		auto values = parse_maxsat(data);
		delete[] data;
		bool *init_assign = load_fixed_assign();
		conf->update_keys(std::map<std::string, int>{{"NUM_VARIABLES", std::get<2>(values)}, {"NUM_CLAUSES", (*std::get<0>(values)).size()}});
		cont = new MAXSATContainer(std::get<1>(values), M_cst_frc(NUM_VARIABLES), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else if (M_cst_frc(DATASET) == 3)
	{
		int *data = load_points<int, true>("maxsat_benchmark2.hdf5", M_cst_frc(SEED_PROBLEM));
		// std::tuple<std::vector<clause_t>*, weight_t*, n_vars_t>
		auto values = parse_maxsat(data, true);
		delete[] data;
		conf->update_keys(std::map<std::string, int>{{"NUM_VARIABLES", std::get<2>(values)}, {"NUM_CLAUSES", (*std::get<0>(values)).size()}});
		bool *init_assign = generate_random_var_assign(M_cst_frc(NUM_VARIABLES), M_cst_frc(SEED_ASSIGN));
		cont = new MAXSATContainer(std::get<1>(values), M_cst_frc(NUM_VARIABLES), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else if (M_cst_frc(DATASET) == 4)
	{
		int *data = load_points<int, true>("maxsat_benchmark3.hdf5", M_cst_frc(SEED_PROBLEM));
		// std::tuple<std::vector<clause_t>*, weight_t*, n_vars_t>
		auto values = parse_maxsat(data, true);
		delete[] data;
		conf->update_keys(std::map<std::string, int>{{"NUM_VARIABLES", std::get<2>(values)}, {"NUM_CLAUSES", (*std::get<0>(values)).size()}});
		bool *init_assign = generate_random_var_assign(M_cst_frc(NUM_VARIABLES), M_cst_frc(SEED_ASSIGN));
		cont = new MAXSATContainer(std::get<1>(values), M_cst_frc(NUM_VARIABLES), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else
	{
		THROW_EXC1("Invalid DATASET argument, Valid values are 0,1,2,3", 8);
	}
	// Choose if make improvement
	if (M_cst_frc(IMPR) == 1)
	{
		improve(cont, M_cst_frc(SEED_ASSIGN));
	}
	else if (M_cst_frc(IMPR) > 1)
	{
		THROW_EXC1("Invalid CLAUSE_IMPR argument, Valid values are 0->1", 9);
	}
	conf->check_and_fill_keys_after_load_instance();
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	MAXSATResult *res = new MAXSATResult(conf, metrics);
	this->on_setup_end(cont, res);
	// algorithms execution
	std::vector<AlgorithmObserver<MAXSATContainer,MAXSATSwap> *> observers;
	observers.push_back(res);
	MAXSATAlgorithm *alg = new MAXSATAlgorithm(observers);
	alg->run(cont, conf, (bool)M_cst(FI_BI));

	// writing the results
	if (dump_mapping)
	{
		res->save_mapping();
	}
	create_dataset<MAXSATResult>(res);

	// cleaning
	const int seed = conf->get(stringify(SEED_GLOB));
	delete metrics;
	delete res;
	delete cont;
	delete alg;
}

void CluteringFactory::run(ClusteringConfig *conf, bool dump_mapping)
{
	// setup
	conf->check_keys_before_load_instance();
	ClusteringContainer *cont;
	if (M_cst_frc(DATASET) == 0)
	{
		/* Uniformly distrbuted points */
		cont = new ClusteringContainer(
			uniform_points(M_cst_frc(NUM_POINTS),
						   M_cst_frc(NUM_DIM),
						   M_cst_frc(SEED_PROBLEM)),
			random_clust(conf), conf);
	}
	else if (M_cst_frc(DATASET) == 1)
	{
		/* Franti et al benchmark */
		cont = new ClusteringContainer(
			open_points_1(M_cst_frc(SEED_PROBLEM), "franti_benchmark.hdf5", conf, true),
			random_clust(conf),
			conf);
	}
	else if (M_cst_frc(DATASET) == 2)
	{
		/* Aloise et al benchmark */
		cont = new ClusteringContainer(
			open_points_1(M_cst_frc(SEED_PROBLEM), "aloise_benchmark.hdf5", conf, false),
			random_clust(conf),
			conf);
	}
	else
	{
		THROW_EXC1("Invalid DATASET argument, Valid values are 0->2", 10);
	}
	if (M_cst_frc(IMPR) == 1)
	{
		kmeansPlusPlus(cont, conf);
	}
	else if (M_cst_frc(IMPR) > 1)
	{
		THROW_EXC1("Invalid IMPR argument, Valid values are 0->1", 11);
	}
	conf->check_and_fill_keys_after_load_instance();
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	ClusteringResult *res = new ClusteringResult(conf, metrics);
	this->on_setup_end(cont, res);
	// algorithms execution
	std::vector<AlgorithmObserver<ClusteringContainer, ClusteringSwap> *> observers;
	observers.push_back(res);
	if (dump_mapping)
	{
		res->save_mapping();
	}
	ClusteringAlgorithm *alg = new ClusteringAlgorithm(observers);
	alg->run(cont, conf, conf->get("FI_BI"));
	// writing the results
	create_dataset<ClusteringResult>(res);
	// cleaning
	delete alg;
	delete metrics;
	delete res;
	delete cont;
}

void TSPFactory::run(TSPConfig *conf, bool dump_mapping, bool debug)
{
	// setup
	conf->check_keys_before_load_instance();
	const double *towns_pos;
	if (M_cst_frc(DATASET) == 0)
	{
		/* Uniformly distrbuted points */
		towns_pos = uniform_points(M_cst_frc(NUM_TOWNS),
								   M_cst_frc(NUM_DIM),
								   M_cst_frc(SEED_PROBLEM));
	}
	else if (M_cst_frc(DATASET) == 1)
	{
		/* TSPLIB */
		towns_pos = open_file("tsplib.hdf5", M_cst_frc(SEED_PROBLEM));
	}
	else
	{
		THROW_EXC1("Invalid DATASET argument, Valid values are 0->2", 10);
	}
	DistanceMatrix m(M_cst_frc(NUM_TOWNS), M_cst_frc(NUM_DIM), towns_pos);
	int *tour;
	if (M_cst_frc(IMPR) == 0)
	{
		tour = random_tour(M_cst_frc(NUM_TOWNS), M_cst_frc(NUM_DIM));
	}
	else if (M_cst_frc(IMPR) == 1)
	{
		tour = improved_tour(M_cst_frc(NUM_TOWNS), M_cst_frc(NUM_DIM), &m);
	}
	else if (M_cst_frc(IMPR) > 1)
	{
		THROW_EXC1("Invalid IMPR argument, Valid values are 0->1", 11);
	}
	conf->check_and_fill_keys_after_load_instance();
	TSPContainer cont(tour, &m, conf);
	cont.on_improvement_done();
	Metrics *metrics = new Metrics();
	TSPResult *res = new TSPResult(conf, metrics);
	this->on_setup_end(&cont, res);
	// algorithms execution
	std::vector<AlgorithmObserver<TSPContainer, TSPSwap>*> observers;
	observers.push_back(res);
	if (dump_mapping)
	{
		res->save_mapping();
	}
	JSONStorage storage;
	TSPLogger logger(&storage);
	if (debug)
	{
		observers.push_back(&logger);
	}
	TSPAlgorithm *alg = new TSPAlgorithm(observers);
	alg->run(&cont, conf, conf->get("FI_BI"));
	// writing the results
	create_dataset<TSPResult>(res);
	storage.save_json("actions.json");
	// cleaning
	delete alg;
	delete metrics;
	delete res;
}