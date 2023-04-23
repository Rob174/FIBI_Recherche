#include "factories.h"

void MAXSATFactory::run(MAXSATConfig *conf, bool dump_mapping, bool debug, const int min_var_per_clause, const int max_var_per_clause)
{
	// setup
	MAXSATContainer *cont;
	if (conf->DATASET.get() == 0)
	{
		/* Generated dataset */
		std::pair<std::vector<clause_t> *, double *> clauses = generate_random_clauses(
			conf->NUM_VARIABLES.get(),
			conf->NUM_CLAUSES.get(),
			conf->SEED_PROBLEM.get(),
			min_var_per_clause, max_var_per_clause);
		bool *init_assign = generate_random_var_assign(conf->NUM_VARIABLES.get(), conf->SEED_ASSIGN.get());
		cont = new MAXSATContainer(clauses.second, conf->NUM_VARIABLES.get(), init_assign, *clauses.first, conf);
		delete[] init_assign;
		delete[] clauses.second;
		delete clauses.first;
	}
	else if (conf->DATASET.get() == 1)
	{
		double *data = load_points<double, false>("maxsat_benchmark.hdf5", conf->SEED_PROBLEM.get());
		// std::tuple<std::vector<clause_t>*, weight_t*, n_vars_t>
		auto values = parse_maxsat<double>(data);
		delete[] data;
		conf->NUM_VARIABLES.set(std::get<2>(values));
		conf->NUM_CLAUSES.set((*std::get<0>(values)).size());
		bool *init_assign = generate_random_var_assign(conf->NUM_VARIABLES.get(), conf->SEED_ASSIGN.get());
		cont = new MAXSATContainer(std::get<1>(values), conf->NUM_VARIABLES.get(), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else if (conf->DATASET.get() == 2)
	{
		double *data = load_fixed_clauses();
		auto values = parse_maxsat<double>(data);
		delete[] data;
		bool *init_assign = load_fixed_assign();
		conf->NUM_VARIABLES.set(std::get<2>(values));
		conf->NUM_CLAUSES.set((*std::get<0>(values)).size());
		cont = new MAXSATContainer(std::get<1>(values), conf->NUM_VARIABLES.get(), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else if (conf->DATASET.get() == 3)
	{
		int *data = load_points<int, true>("maxsat_benchmark2.hdf5", conf->SEED_PROBLEM.get());
		// std::tuple<std::vector<clause_t>*, weight_t*, n_vars_t>
		auto values = parse_maxsat<int>(data, true);
		delete[] data;
		conf->NUM_VARIABLES.set(std::get<2>(values));
		conf->NUM_CLAUSES.set((*std::get<0>(values)).size());
		bool *init_assign = generate_random_var_assign(conf->NUM_VARIABLES.get(), conf->SEED_ASSIGN.get());
		cont = new MAXSATContainer(std::get<1>(values), conf->NUM_VARIABLES.get(), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else if (conf->DATASET.get() == 4)
	{
		int *data = load_points<int, true>("maxsat_benchmark3.hdf5", conf->SEED_PROBLEM.get());
		// std::tuple<std::vector<clause_t>*, weight_t*, n_vars_t>
		auto values = parse_maxsat(data, true);
		delete[] data;
		conf->NUM_VARIABLES.set(std::get<2>(values));
		conf->NUM_CLAUSES.set((*std::get<0>(values)).size());
		bool *init_assign = generate_random_var_assign(conf->NUM_VARIABLES.get(), conf->SEED_ASSIGN.get());
		cont = new MAXSATContainer(std::get<1>(values), conf->NUM_VARIABLES.get(), init_assign, *std::get<0>(values), conf);
		delete[] std::get<1>(values);
		delete[] init_assign;
		delete std::get<0>(values);
	}
	else
	{
		THROW_EXC1("Invalid DATASET argument, Valid values are 0,1,2,3,4", 8);
	}
	conf->print();
	// Choose if make improvement
	if (conf->IMPR.get() == 1)
	{
		improve(cont, conf->SEED_ASSIGN.get());
	}
	else if(conf->IMPR.get() > 1)
	{
		THROW_EXC1("Invalid CLAUSE_IMPR argument, Valid values are 0->1", 9);
	}
	cont->on_improvement_done();
	Metrics *metrics = new Metrics();
	MAXSATResult *res = new MAXSATResult(conf, metrics);
	this->on_setup_end(cont, res);
	// algorithms execution
	std::vector<AlgorithmObserver<MAXSATContainer,MAXSATSwap> *> observers;
	observers.push_back(res);
	MAXSATAlgorithm *alg = new MAXSATAlgorithm(observers);
	alg->run(cont, conf, (bool)conf->FI_BI.get());

	JSONStorage storage;
	MAXSATLogger logger(&storage, true);
	if (debug)
	{
		observers.push_back(&logger);
	}
	// writing the results
	if (dump_mapping)
	{
		res->save_mapping();
	}
	create_dataset<MAXSATResult>(res);
	if (debug) {
		storage.save_json("actions.json");
	}
	// cleaning
	const int seed = conf->SEED_GLOB.get();
	delete metrics;
	delete res;
	delete cont;
	delete alg;
}

void CluteringFactory::run(ClusteringConfig *conf, bool dump_mapping, bool debug)
{
	// setup
	ClusteringContainer *cont;
	if (conf->DATASET.get() == 0)
	{
		/* Uniformly distrbuted points */
		cont = new ClusteringContainer(
			uniform_points(conf->NUM_POINTS.get(),
						   conf->NUM_DIM.get(),
						   conf->SEED_PROBLEM.get()),
			random_clust(conf), conf);
	}
	else if (conf->DATASET.get() == 1)
	{
		/* Franti et al benchmark */
		// WARNING: works because of here: do not touch ! Need to set the constant NUM_POINTS first
		const double* p = open_points_1(conf->SEED_PROBLEM.get(), "franti_benchmark.hdf5", conf, true);
		cont = new ClusteringContainer(
			p,
			random_clust(conf),
			conf);
	}
	else if (conf->DATASET.get() == 2)
	{
		/* Aloise et al benchmark */
		// WARNING: works because of here: do not touch ! Need to set the constant NUM_POINTS first
		const double* p = open_points_1(conf->SEED_PROBLEM.get(), "aloise_benchmark.hdf5", conf, false);
		cont = new ClusteringContainer(
			p,
			random_clust(conf),
			conf);
	}
	else if(conf->DATASET.get() == 3) {
		/* Normally distributed points */
		cont = new ClusteringContainer(
			normal_points(conf->NUM_POINTS.get(),
				conf->NUM_DIM.get(),
				conf->SEED_PROBLEM.get()),
			random_clust(conf), conf);
	}
	else
	{
		THROW_EXC1("Invalid DATASET argument, Valid values are 0->2", 10);
	}
	if (conf->NUM_CLUST.get() >= conf->NUM_POINTS.get()) {
		std::cout << "Stopping because of num_clust>=num_points with NUM_CLUST:" << conf->NUM_CLUST.get() << " and NUM_POINTS:" << conf->NUM_POINTS.get() << std::endl;
		return;
	}
	conf->print();
	if (conf->IMPR.get() == 1)
	{
		kmeansPlusPlus(cont, conf);
	}
	else if (conf->IMPR.get() == 2) {
		kmeansPlusPlusSuperGlutton(cont, conf);
	}
	else if (conf->IMPR.get() > 1)
	{
		THROW_EXC1("Invalid IMPR argument, Valid values are 0->1", 11);
	}
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
	JSONStorage storage;
	ClusteringLogger logger(&storage, true);
	if (debug)
	{
		observers.push_back(&logger);
	}
	ClusteringAlgorithm *alg = new ClusteringAlgorithm(observers,1000);
	alg->run(cont, conf, conf->FI_BI.get());
	// writing the results
	create_dataset<ClusteringResult>(res);
	if (debug) {
		storage.save_json("actions.json");
	}
	// cleaning
	delete alg;
	delete metrics;
	delete res;
	delete cont;
}

void TSPFactory::run(TSPConfig *conf, bool dump_mapping, bool debug)
{
	using namespace std;
	// setup
	vector<double> towns_pos;
	if (conf->DATASET.get() == 0)
	{
		/* Uniformly distrbuted points */
		towns_pos = uniform_points(conf->NUM_TOWNS.get(),
								   conf->NUM_DIM.get(),
								   conf->SEED_PROBLEM.get());
	}
	else if (conf->DATASET.get() == 1)
	{
		/* TSPLIB */
		towns_pos = open_file("tsplib.hdf5", conf->SEED_PROBLEM.get(), conf);
	}
	else if(conf->DATASET.get() == 2) {
		/* Normally distributed points */
		towns_pos = normal_points(conf->NUM_TOWNS.get(),
			conf->NUM_DIM.get(),
			conf->SEED_PROBLEM.get());
	}
	else
	{
		THROW_EXC1("Invalid DATASET argument, Valid values are 0->3", 10);
	}
	conf->print();
	DistanceMatrix m(conf->NUM_TOWNS.get(), conf->NUM_DIM.get(), towns_pos);
	int *tour;
	if (conf->IMPR.get() == 0)
	{
		tour = random_tour(conf->NUM_TOWNS.get(), conf->SEED_ASSIGN.get());
	}
	else if (conf->IMPR.get() == 1)
	{
		tour = improved_tour(conf->NUM_TOWNS.get(), conf->SEED_ASSIGN.get(), &m);
	}
	else if (conf->IMPR.get() == 2)
	{
		tour = improved_rand_tour(conf->NUM_TOWNS.get(), conf->SEED_ASSIGN.get(), &m, 3);
	}
	else if (conf->IMPR.get() == 3)
	{
		tour = improved_rand_tour(conf->NUM_TOWNS.get(), conf->SEED_ASSIGN.get(), &m, 4);
	}
	else if (conf->IMPR.get() == 4)
	{
		tour = improved_rand_tour(conf->NUM_TOWNS.get(), conf->SEED_ASSIGN.get(), &m, 5);
	}
	else
	{
		THROW_EXC1("Invalid IMPR argument, Valid values are 0->4", 11);
	}
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
	TSPLogger logger(&storage, true);
	if (debug)
	{
		observers.push_back(&logger);
	}
	TSPAlgorithm *alg = new TSPAlgorithm(observers);
	alg->run(&cont, conf, conf->FI_BI.get());
	// writing the results
	create_dataset<TSPResult>(res);
	if (debug) {
		storage.save_json("actions.json");
	}
	// cleaning
	delete alg;
	delete metrics;
	delete res;
}