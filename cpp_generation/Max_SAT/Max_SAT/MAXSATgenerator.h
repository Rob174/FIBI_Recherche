#ifndef MAXSATGENERATOR_H
#define MAXSATGENERATOR_H
#include <random>
#include "Container.h"
#include <iostream>
#include <set>
#include "exceptions.h"
#include "utils.h"
#include <cmath>
std::pair<std::vector<clause_t> *, double *> generate_random_clauses(
	const int num_variables, const int num_clauses, const int seed, const int min_num_var_per_clauses, const int max_num_var_per_clauses);
bool *generate_random_var_assign(const int num_var, const int seed);
#endif