#ifndef FACTORIES_H
#define FACTORIES_H
#include <vector>
#include "constants.h"
#include "Container.h"
#include "MAXSATgenerator.h"
#include "exceptions.h"
#include "result.h"
#include "Algorithm.h"
#include "dataset.h"
#include "InstanceLoader.h"
#include "factory_observer.h"
#include "ClusteringGenerator.h"
#include "kmeans.h"
#include "maxsat_improvement.h"
#include "TSPGenerator.h"
#include <filesystem>

class AbstractMAXSATFactory : public AbstractMAXSATFactoryObservable
{
public:
	AbstractMAXSATFactory(std::vector<AbstractMAXSATFactoryObserver *> obs) : AbstractMAXSATFactoryObservable(obs){};
};
class MAXSATFactory : public AbstractMAXSATFactory
{
public:
	MAXSATFactory(std::vector<AbstractMAXSATFactoryObserver *> obs) : AbstractMAXSATFactory(obs){};
	void run(MAXSATConfig *conf, bool dump_mapping = false, const int min_var_per_clause = -1, const int max_var_per_clause = -1);
};
class CluteringFactory : public AbstractClusteringFactoryObservable
{
public:
	CluteringFactory(std::vector<AbstractClusteringFactoryObserver *> obs) : AbstractClusteringFactoryObservable(obs){};
	void run(ClusteringConfig *conf, bool dump_mapping = false);
};

class AbsstractClusteringFactory : public AbstractClusteringFactoryObservable
{
public:
	AbsstractClusteringFactory(std::vector<AbstractClusteringFactoryObserver *> obs) : AbstractClusteringFactoryObservable(obs){};
};

class AbstractTSPFactory : public AbstractTSPFactoryObservable
{
public:
	AbstractTSPFactory(std::vector<AbstractTSPFactoryObserver *> obs) : AbstractTSPFactoryObservable(obs){};
};
class TSPFactory : public AbstractTSPFactory
{
public:
	TSPFactory(std::vector<AbstractTSPFactoryObserver *> obs) : AbstractTSPFactory(obs){};
	void run(TSPConfig *conf, bool dump_mapping = false, bool debug = false);
};

#endif