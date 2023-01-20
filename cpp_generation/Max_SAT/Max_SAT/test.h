#ifndef TEST_H
#define TEST_H
#include <vector>

#include "utils.h"
#include "constants.h"
#include <string>
#include "exceptions.h"
#include "SolutionSaver.h"
#include "Container.h"
#include <random>
#include <algorithm>

#include "MAXSATgenerator.h"
#include "Algorithm.h"
#include "AlgorithmObservable.h"
#include "AlgorithmObserver.h"
#include "result.h"
#include "Metrics.h"
#include "H5Cpp.h"
#include "factories.h"
#include "MAXSATgenerator.h"
#include "ClusteringGenerator.h"
#include "constants.h"
#include "TSPGenerator.h"

void test_clustering();
void test_tsp();
void test_tsp_50towns_rand();
void test_tsp_50towns_impr();
void test_clustering_50points();
void test_maxsat_50clauses_50vars();
class Tests
{
public:
	Tests() {};
	void run();
};
#endif