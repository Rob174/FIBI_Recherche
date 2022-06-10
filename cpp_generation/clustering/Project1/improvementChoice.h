#pragma once
#include <iostream>
#include <limits>
#include <vector>
#include <tuple>
#include "constants.h"
#include "results.h"
#include "operations.h"
#include "utils.h"

#define UNINITIALIZED -1
class ClusteringChoice {
public:
	int i;
	int l;
	int j;
	double vij;
	int counter_not_closest;
	ClusteringChoice() : i(UNINITIALIZED), l(UNINITIALIZED), j(UNINITIALIZED), vij(0), counter_not_closest(UNINITIALIZED){};
};
class ImprovementChoice {
public:
	ImprovementChoice(){};
	virtual void choose_solution(ClusteringChoice* choice, Clustering* clustering, double sugg_vij, int sugg_i, int sugg_l, int sugg_j);
	virtual bool stop_loop();
	virtual void initialize(Clustering*clustering) { return; };
	virtual void after_choice();
};
class BestImpr : public ImprovementChoice {
public:
	BestImpr() : ImprovementChoice() {};
	bool stop_loop();
};
class FirstImpr : public ImprovementChoice {
public:
	FirstImpr() : ImprovementChoice() {};
	bool stop_loop();
};
