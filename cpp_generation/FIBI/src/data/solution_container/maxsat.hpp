#pragma once

#include <vector>
#include "./abstract.hpp"
#include "../../algorithm/swaps/maxsat.hpp"

using namespace std;


class MAXSATSolutionContainer : public AbstractSolutionContainer<MAXSATSwap>
{
public:
	const int num_variables;
	const int num_clauses;

	const vector<double>& weights;
	vector<bool>& variables_val;
	const vector<clause_t>& clauses;

	map<const var_id_t, vector<clause_id_t>> var_in_clauses;
	bool* sat_clauses;

	MAXSATSolutionContainer(const vector<double>& weights, vector<bool>& init_variables_val, const vector<clause_t>& clauses) :
		weights(weights),
		variables_val(init_variables_val),
		num_variables(init_variables_val.size()),
		clauses(clauses),
		num_clauses(clauses.size()),
		AbstractSolutionContainer()
	{
		sat_clauses = new bool[num_clauses];
		double obj_init = 0;
		for (clause_id_t i = 0; i < num_clauses; i++)
		{
			const auto& c = clauses[i];
			bool sat = false;
			for (const auto& c_elem : c)
			{
				const var_id_t var_id = c_elem.first;
				if (var_id >= num_variables)
				{
					throw runtime_error("var_id >= num_variables: " + to_string(var_id) + " >= " + to_string(num_variables));
				}
				// Add clause to var_in_clauses: var_id -> clause_id_1, clause_id_2, ...
				auto& clauses_ids = var_in_clauses.emplace(var_id, vector<clause_id_t>()).first->second;
				clauses_ids.push_back(i);
				if (c_elem.second == variables_val[var_id])
				{
					sat = true;
				}
			}
			sat_clauses[i] = sat;
			if (sat)
			{
				obj_init += weights[i];
			}
		}
		if (!var_in_clauses.empty())
		{
			vector<string> id_missing;
			for (int i = 0; i < num_variables; i++)
			{
				if (var_in_clauses.find(i) == var_in_clauses.end())
				{
					id_missing.push_back(to_string(i));
					var_in_clauses.emplace(i, vector<clause_id_t>());
				}
			}
		}
	};
	MAXSATSolutionContainer(const MAXSATSolutionContainer& other) :
		num_variables(other.get_num_variables()),
		num_clauses(other.get_clauses().size()),
		weights(other.get_weights()),
		variables_val(other.get_variables_val()),
		clauses(other.get_clauses()),
		var_in_clauses(other.get_var_in_clauses()),
		sat_clauses(other.get_sat_clause()),
		AbstractSolutionContainer()
	{
	}
	[[nodiscard]] const vector<double> get_weights() const noexcept { return weights; }
	[[nodiscard]] vector<bool>& get_variables_val() const noexcept { return variables_val; }
	[[nodiscard]] const vector<clause_t> get_clauses() const noexcept { return clauses; }
	[[nodiscard]] const map<const var_id_t, vector<clause_id_t>> get_var_in_clauses() const noexcept { return var_in_clauses; }
	[[nodiscard]] bool* get_sat_clause() const { return sat_clauses; }
	[[nodiscard]] int get_num_variables() const noexcept { return num_variables; }
	[[nodiscard]] int get_num_clauses() const noexcept { return num_clauses; }
	[[nodiscard]] double get_objective_cost() const {
		double obj = 0;
		for (clause_id_t i = 0; i < num_clauses; i++)
		{
			if (sat_clauses[i])
			{
				obj += weights[i];
			}
		}
		return obj;
	}
	~MAXSATSolutionContainer()
	{
		delete[] sat_clauses;
	}
	// Flips
	quality_delta_t test_flip(MAXSATSwap& test_swap) const {
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
	void flip(const MAXSATSwap& test_swap, const quality_delta_t delta) {
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

		update_with_delta_cost(delta);
		last_choice = test_swap;
	};
	// Objective
	double compute_quality_metric() {
		double obj = 0;
		for (clause_id_t i = 0; i < num_clauses; i++)
		{
			const clause_t c = clauses.at(i);
			bool sat = false;
			for (int j = 0; j < c.size(); j++)
			{
				const clause_elem_t c_elem = c.at(j);
				const var_id_t var_id = c_elem.first;
				if (c_elem.second == variables_val[var_id])
				{
					sat = true;
				}
			}
			if (sat)
			{
				obj += weights[i];
			}
		}
		return obj;
	};
	// Setters
	void set_var_value(const var_id_t var_id, bool value) {
		variables_val[var_id] = value;
		for (clause_id_t i : var_in_clauses.at(var_id))
		{
			bool sat = false;
			for (auto& lit_p : clauses.at(i))
			{
				if (variables_val[lit_p.first] == lit_p.second)
				{
					sat = true;
					break;
				}
			}
			sat_clauses[i] = sat;
		}
	};
	// Utility
	void print() {
		cout << "Weights: ";
		for (int i = 0; i < num_clauses; i++)
		{
			const double v = weights[i];
			if (i != 0)
			{
				cout << ", ";
			}
			cout << v;
		}
		cout << endl;

		cout << "Variable values: ";
		for (int i = 0; i < num_variables; i++)
		{
			const bool v = variables_val[i];
			if (i != 0)
			{
				cout << ", ";
			}
			if (v)
			{
				cout << "\x1B[33mtrue\033[0m ";
			}
			else
			{
				cout << "\x1B[33mfalse\033[0m ";
			}
		}
		cout << endl;
		cout << MAXSATSolutionContainer::clauses_str(clauses);
		cout << "Clauses with values of variables: ";
		bool first_clause = true;
		for (clause_t c : clauses)
		{
			if (!first_clause)
			{
				cout << "\x1B[32m AND \033[0m ";
			}
			cout << "[ ";
			bool first = true;
			for (clause_elem_t l : c)
			{
				if (!first)
				{
					cout << "\x1B[32m OR \033[0m ";
				}
				if (!l.second)
				{
					cout << "NOT(";
				}
				const bool v = variables_val[l.first];
				if (v)
				{
					cout << "\x1B[33mtrue\033[0m ";
				}
				else
				{
					cout << "\x1B[33mfalse\033[0m ";
				}
				if (!l.second)
				{
					cout << ")";
				}
				first = false;
			}
			cout << "] ";
			first_clause = false;
		}
		cout << endl;
		cout << "Saturated clauses: ";
		for (int i = 0; i < num_clauses; i++)
		{
			const bool v = sat_clauses[i];
			if (i != 0)
			{
				cout << ", ";
			}
			if (v)
			{
				cout << "\x1B[33mtrue\033[0m ";
			}
			else
			{
				cout << "\x1B[33mfalse\033[0m ";
			}
		}
		cout << endl;
		cout << "Objective (weighted num of clauses satisfied): " << quality_metric;
		cout << endl;
	};
	static string clauses_str(vector<clause_t> clauses) {
		string msg = "\033[0m Clauses formula: ";
		bool first_clause = true;
		for (clause_t c : clauses)
		{
			if (!first_clause)
			{
				msg.append("\x1B[32m AND \033[0m ");
			}
			msg.append("[");
			bool first = true;
			for (clause_elem_t l : c)
			{
				if (!first)
				{
					msg.append("\x1B[32m OR \033[0m ");
				}
				if (!l.second)
				{
					msg.append("NOT(");
				}
				msg.append("\x1B[33mvar");
				msg.append(to_string(l.first));
				msg.append("\033[0m ");
				if (!l.second)
				{
					msg.append(")");
				}
				first = false;
			}
			msg.append("] ");
			first_clause = false;
		}
		return msg;
	};
};