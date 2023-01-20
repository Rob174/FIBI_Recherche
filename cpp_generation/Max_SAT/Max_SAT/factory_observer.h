#ifndef FACTORY_OBSERVER_H
#define FACTORY_OBSERVER_H
#include "Container.h"
#include "MAXSATgenerator.h"
#include "result.h"

class AbstractFactoryObserver
{
};
class AbstractMAXSATFactoryObserver : public AbstractFactoryObserver
{
public:
	virtual void on_setup_end(MAXSATContainer *cont, MAXSATResult *res) = 0;
};

class AbstractClusteringFactoryObserver : public AbstractFactoryObserver
{
public:
	virtual void on_setup_end(ClusteringContainer *cont, ClusteringResult*res) = 0;
};
class ClausesChecker : public AbstractMAXSATFactoryObserver
{
private:
	std::vector<clause_t> expected_clauses;

public:
	ClausesChecker(std::vector<clause_t> expected_clauses) : expected_clauses(expected_clauses){};
	void on_setup_end(MAXSATContainer *cont, MAXSATResult *res);
};
class AbstractFactoryObservable
{
};
class AbstractMAXSATFactoryObservable : public AbstractFactoryObservable
{
private:
	std::vector<AbstractMAXSATFactoryObserver *> obs;

public:
	AbstractMAXSATFactoryObservable(std::vector<AbstractMAXSATFactoryObserver *> obs) : obs(obs){};
	void on_setup_end(MAXSATContainer *cont, MAXSATResult *res);
};

class AbstractClusteringFactoryObservable : public AbstractFactoryObservable
{
private:
	std::vector<AbstractClusteringFactoryObserver *> obs;
public:
	AbstractClusteringFactoryObservable(std::vector<AbstractClusteringFactoryObserver *> obs) : obs(obs){};
	void on_setup_end(ClusteringContainer *cont, ClusteringResult *res);
};

class AbstractTSPFactoryObserver : public AbstractFactoryObserver
{
public:
	virtual void on_setup_end(TSPContainer *cont, TSPResult *res) = 0;
};

class AbstractTSPFactoryObservable : public AbstractFactoryObservable
{
private:
	std::vector<AbstractTSPFactoryObserver *> obs;
public:
	AbstractTSPFactoryObservable(std::vector<AbstractTSPFactoryObserver *> obs) : obs(obs){};
	void on_setup_end(TSPContainer *cont, TSPResult *res);
};

#endif