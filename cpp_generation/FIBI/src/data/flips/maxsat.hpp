#pragma once

#include "abstract.hpp"
#include "../../algorithm/swaps/maxsat.hpp"

quality_delta_t fn_test_flip(MAXSATSwap& test_swap, const map<const var_id_t, vector<clause_id_t>> var_in_clauses, const vector<clause_t>& clauses, vector<bool>& variables_val, const vector<double>& weights, bool* sat_clauses) {
    double objective_cost_delta = 0;
    if (var_in_clauses.find(test_swap.var_id) == var_in_clauses.end())
    {
        return 0.;
    }
    const vector<clause_id_t>& var_clauses = var_in_clauses.at(test_swap.var_id);
    for (const clause_id_t& clause_id : var_clauses) {
        const clause_t& clause = clauses.at(clause_id);
        bool sat = false;

        for (const clause_elem_t& clause_elem : clause) {
            const bool variable_val = (clause_elem.first == test_swap.var_id)
                ? !variables_val[clause_elem.first]
                : variables_val[clause_elem.first];

            if (clause_elem.second == variable_val) {
                sat = true;
                break;
            }
        }

        if (sat != sat_clauses[clause_id]) {
            objective_cost_delta += (sat) ? weights[clause_id] : -weights[clause_id];
        }
    }

    return objective_cost_delta;
};
void fn_flip(const MAXSATSwap& test_swap, map<const var_id_t, vector<clause_id_t>> var_in_clauses, const vector<clause_t>& clauses, vector<bool>& variables_val, const vector<double>& weights, bool* sat_clauses)  {
    // update variable value
    variables_val[test_swap.var_id] = !variables_val[test_swap.var_id];

    auto& clauses_with_var = var_in_clauses[test_swap.var_id];
    if (clauses_with_var.empty()) {
        return;
    }

    // Update sat clauses:
    // Example 1: var_id = 3, clauses_with_var = {1, 3, 5}
    for (const auto& clause_id : clauses_with_var) {
        const auto& clause = clauses[clause_id];
        // check if clause contains the variable to flip:
        // Example, Clause 1: [(var_1, 0), (var_2, 1), (var_3, 1)] contains variable var_3
        //          Clause 3: [(var_3, 1), (var_4, 0)] contains variable var_3
        //          Clause 5: [(var_3, 0), (var_5, 1)] contains variable var_3
        // check if clause is satisfied
        bool sat = false;
        // Example 1: clause = [(var_3, 0), (var_5, 1)] -> clause_elem: (var_3, 0)
        for (const auto& clause_elem : clause) {
            bool variable_val = variables_val[clause_elem.first];
            if (clause_elem.first == test_swap.var_id) {
                // invert the variable value if is the selected one to flip
                variable_val = !variable_val;
            }
            if (clause_elem.second == variable_val) {
                sat = true;
                break;
            }
        }
        sat_clauses[clause_id] = sat;
    }
};