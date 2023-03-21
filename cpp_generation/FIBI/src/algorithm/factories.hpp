#pragma once

#include <vector>
#include "../types.h"
#include "./algorithms/localSearch.hpp"
#include "./algorithms/neighbourhoods/tsp.hpp"
#include "./algorithms/neighbourhoods/clustering.hpp"
#include "./algorithms/neighbourhoods/maxsat.hpp"
#include "../algorithm/observers/AlgorithmObserver.hpp"
#include "../data/solution_container/tsp.hpp"
#include "../data/solution_container/clustering.hpp"
#include "../data/solution_container/maxsat.hpp"
#include "../algorithm/swaps/tsp.hpp"
#include "../algorithm/swaps/clustering.hpp"
#include "../algorithm/swaps/maxsat.hpp"
#include "../data/constants/tsp.hpp"
#include "../data/constants/clustering.hpp"
#include "../data/constants/maxsat.hpp"

using tsp_ls_t = LocalSearch < TSPSwap, TSPSolutionContainer<>, TSPNeighbourhoodExplorer<>, TSPConfig>;
using clustering_ls_t = LocalSearch < ClusteringSwap, ClusteringSolutionContainer<>, ClusteringNeighbourhoodExplorer<>, ClusteringConfig>;
using maxsat_ls_t = LocalSearch < MAXSATSwap, MAXSATSolutionContainer, MAXSATNeighbourhoodExplorer<>, MAXSATConfig>;
using tsp_obs_t = AlgorithmObserver<TSPSwap, TSPSolutionContainer<>>;
using clustering_obs_t = AlgorithmObserver<ClusteringSwap, ClusteringSolutionContainer<>>;
using maxsat_obs_t = AlgorithmObserver<MAXSATSwap, MAXSATSolutionContainer>;
using namespace std;
tsp_ls_t* getTSPLocalSearch(vector < tsp_obs_t*>& obs) {
	AlgorithmObservable<TSPSwap, TSPSolutionContainer<>> *o = new AlgorithmObservable<TSPSwap, TSPSolutionContainer<>>(obs);
	return new tsp_ls_t(new TSPNeighbourhoodExplorer<>(o), o);
}

clustering_ls_t* getClusteringLocalSearch(vector < clustering_obs_t*>& obs) {
	AlgorithmObservable<ClusteringSwap, ClusteringSolutionContainer<>>* o = new AlgorithmObservable<ClusteringSwap, ClusteringSolutionContainer<>>(obs);
	return new clustering_ls_t(new ClusteringNeighbourhoodExplorer<>(o), o);
}

maxsat_ls_t* getMAXSATLocalSearch(vector < maxsat_obs_t*>& obs) {
	AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>* o = new AlgorithmObservable<MAXSATSwap, MAXSATSolutionContainer>(obs);
	return new maxsat_ls_t(new MAXSATNeighbourhoodExplorer<>(o), o);
}
