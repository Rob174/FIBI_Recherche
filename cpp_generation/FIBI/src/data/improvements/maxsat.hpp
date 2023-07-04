#pragma once
#include <random>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include "../../types.h"
#include "../../utils.h"

using namespace std;

struct stats
{
	double SAT;
	double UNSAT;
	double B;
};
struct best_assignement
{
	double increase_bound;
	var_id_t variable;
	bool assignement;
	bool found;
};

void assign_variable(const best_assignement &b, stats &former_stats, vector<bool> *assignement)
{
}
/** @brief A simple randomized 3/4 approximation algorithm for MAXSAT
 *   Taken from paper Bounds on Greedy Algorithms for MAX SAT https://link.springer.com/content/pdf/10.1007/978-3-642-23719-5.pdf
 *   with explanations of https://diglib.tugraz.at/download.php?id=5b6d299d67215&location=browse
 *   main source https://people.orie.cornell.edu/mup3/Poloczek%20Schnitger%20Williamson%20van%20Zuylen%20-%20Greedy%20Algorithms%20for%20MAX%20SAT__SICOMP_2017.pdf p1038 and 1058-59
 * * @param var_in_clauses: map of variable to clauses where it appears
 * * @param clauses: vector of clauses
 * * @param weights: vector of weights
 * * @param num_variables: number of variables
 * * @param seed: seed for random number generator
 * * @return vector of bools representing the assignments for the variables
 */
vector<bool> *improve_maxsat(const map<const var_id_t, vector<clause_id_t>> &var_in_clauses, const vector<clause_t> &clauses, const vector<double> &weights, const unsigned int num_variables, const unsigned int seed)
{
	vector<bool> *assignement = new vector<bool>(num_variables, false);
	mt19937 gen{seed};
	vector<vector<set<clause_id_t>>> undecided_clauses_per_var = {};
	for (var_id_t var_id = 0; var_id < num_variables; var_id++)
	{
		vector<set<clause_id_t>> undecided = {set<clause_id_t>{}, set<clause_id_t>{}};
		undecided_clauses_per_var.push_back(undecided);
	}
	set<var_id_t> undecided_variables = {};
	for (var_id_t var_id = 0; var_id < num_variables; var_id++)
	{
		undecided_variables.insert(var_id);
	}
	vector<int> number_unfixed_literals;
	for (clause_id_t i = 0; i < clauses.size(); i++)
	{
		auto &clause = clauses.at(i);
		number_unfixed_literals.push_back(clause.size());
		for (auto &e : clause)
		{
			undecided_clauses_per_var.at(e.first).at((int)(e.second)).insert(i);
		}
	}
	double weight_satisfied = 0;
	double total_weights = 0;
	for (const auto &w : weights)
	{
		total_weights += w;
	}
	stats formerStats = {.SAT = 0, .UNSAT = 0, .B = total_weights / 2.};
	auto cmptB = [total_weights](stats &s)
	{ return 0.5 * (double)(s.SAT + (total_weights - s.UNSAT)); };
	while (undecided_variables.size() > 0)
	{
		set<var_id_t> cpy_undecided_variables = undecided_variables;
		best_assignement b = {
			-1e16,
			0,
			false,
			false};
		for (var_id_t var_id : cpy_undecided_variables)
		{
			// ti: if we set xi to true, all clauses with only one variable left
			stats currStatsTrue = {.SAT = formerStats.SAT, .UNSAT = formerStats.UNSAT, .B = formerStats.B};
			// and with a negation will be set to false not true if previously a true assignement (disjonction of literals)
			for (int clause_id : undecided_clauses_per_var.at(var_id).at(0))
			{
				if (number_unfixed_literals.at(clause_id) == 1)
				{
					currStatsTrue.UNSAT += weights[clause_id];
				}
			}
			// and without a negation will be set to true: because **at least** one true required
			for (int clause_id : undecided_clauses_per_var.at(var_id).at(1))
			{
				currStatsTrue.SAT += weights[clause_id];
			}
			currStatsTrue.B = cmptB(currStatsTrue);
			const double ti = currStatsTrue.B - formerStats.B;

			// fi: if we set xi to false
			stats currStatsFalse = {.SAT = formerStats.SAT, .UNSAT = formerStats.UNSAT, .B = formerStats.B};
			for (int clause_id : undecided_clauses_per_var.at(var_id).at(1))
			{
				if (number_unfixed_literals.at(clause_id) == 1)
				{
					currStatsFalse.UNSAT += weights[clause_id];
				}
			}
			for (int clause_id : undecided_clauses_per_var.at(var_id).at(0))
			{
				currStatsFalse.SAT += weights[clause_id];
			}
			currStatsFalse.B = cmptB(currStatsFalse);
			const double fi = currStatsFalse.B - formerStats.B;
			// We want to maximize the bound (weights of satisfied clauses or potentially satisfyable clauses)
			if (ti > b.increase_bound)
			{
				b.increase_bound = ti;
				b.variable = var_id;
				b.assignement = true;
				b.found = true;
			}
			if (fi > b.increase_bound)
			{
				b.increase_bound = fi;
				b.variable = var_id;
				b.assignement = false;
				b.found = true;
			}
		}
		if (!b.found)
		{
			// If no improvement found choose a random variable
			std::uniform_int_distribution<> dis_var(0, undecided_variables.size() - 1);
			auto it = undecided_variables.begin();
    		std::advance(it, dis_var(gen));
			b.variable = *it;
			// And a random assignement
			std::uniform_int_distribution<> dis_affect(0, 1);
			b.assignement = dis_affect(gen);
			// Compute the variation of bound
			b.increase_bound = 0;
			stats currStats = {.SAT = formerStats.SAT, .UNSAT = formerStats.UNSAT, .B = formerStats.B};
			for (int clause_id : undecided_clauses_per_var.at(b.variable).at((int)b.assignement))
			{
				if (number_unfixed_literals.at(clause_id) == 1)
				{
					currStats.UNSAT += weights[clause_id];
				}
			}
			// and without a negation will be set to true: because **at least** one true required
			for (int clause_id : undecided_clauses_per_var.at(b.variable).at(1))
			{
				currStats.SAT += weights[clause_id];
			}
			currStats.B = cmptB(currStats);
			b.increase_bound = currStats.B - formerStats.B;
		}
		// Remove the variable from the ones to decide
		undecided_variables.erase(b.variable);
		(*assignement)[b.variable] = b.assignement;
		// Remove satisfied clauses and update stats
		// if false Lxi_bar if true Lxi to iterate
		const int idx_neg_sat = (int)(b.assignement);
		auto undecided = undecided_clauses_per_var.at(b.variable).at(idx_neg_sat);

		for (int clause_id : vector<int>(undecided.begin(), undecided.end()))
		{
			undecided_clauses_per_var.at(b.variable).at(idx_neg_sat).erase(clause_id);
			formerStats.SAT += weights[clause_id];
		}
		// Decrement unfixed literals in other clauses
		const int idx_neg_unsat = (int)(!b.assignement);
		undecided = undecided_clauses_per_var.at(b.variable).at(idx_neg_unsat);
		for (int clause_id : vector<int>(undecided.begin(), undecided.end()))
		{
			number_unfixed_literals.at(clause_id) -= 1;
			if (number_unfixed_literals.at(clause_id) == 0)
			{
				undecided_clauses_per_var.at(b.variable).at(idx_neg_unsat).erase(clause_id);
				formerStats.UNSAT += weights[clause_id];
			}
		}
		formerStats.B = cmptB(formerStats);
	}

	return assignement;
}