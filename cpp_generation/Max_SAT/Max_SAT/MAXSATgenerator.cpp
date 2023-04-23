#include "MAXSATgenerator.h"
bool* generate_random_var_assign(const int num_var, const int seed)
{
	std::mt19937 mt{ (unsigned int)seed };
	std::uniform_int_distribution rd_val(0, 1);
	bool* initial_var_values = new bool[num_var];
	for (int i = 0; i < num_var; i++)
	{
		initial_var_values[i] = rd_val(mt) == 1;
	}
	return initial_var_values;
}
std::pair<std::vector<clause_t>*, double*> generate_random_clauses(
	const int num_variables, const int num_clauses, const int seed, const int min_num_var_per_clauses, const int max_num_var_per_clauses)
{
	// Problem: on 5000 variables, only ~300 used. Checked uniformly distributed in 0;4999 ok. More than 
	std::mt19937 mt{ (unsigned int)seed };
	std::uniform_real_distribution<double> rd_w(0, 1000);
	std::uniform_int_distribution<int> rd_var(0, num_variables - 1);
	std::uniform_int_distribution rd_bool(0, 1);
	std::set<int> vars_in_clauses;
	std::vector<clause_t>* clauses = new std::vector<clause_t>();
	// Prepare containers to manage mandatory variable per clause
	int mandatory_id = 0;
	const int num_mandatory_per_clause = ceil(num_variables / (double)num_clauses);
	std::vector<int> mandatory_variables;
	for (int i = 0; i < num_variables; i++) {
		mandatory_variables.push_back(i);
	}
	std::shuffle(mandatory_variables.begin(), mandatory_variables.end(), mt);
	// start of the main algorithm
	for (int i = 0; i < num_clauses; i++)
	{
		std::set<int> vars_in_curr_clause;
		clause_t clause;
		// default variable(s) to ensure that all the variables are at least in one clause
		int num_required = 0;
		if (num_mandatory_per_clause * mandatory_id < num_variables - 1) {
			const int min_var_id_to_insert = num_mandatory_per_clause * mandatory_id;
			const int max_var_id_to_insert = min<int>(num_mandatory_per_clause * (mandatory_id + 1)-1, num_variables-1);
			for (int vari = min_var_id_to_insert; vari <= max_var_id_to_insert; vari++) {
				vars_in_curr_clause.insert(mandatory_variables.at(vari));
				vars_in_clauses.insert(mandatory_variables.at(vari));
				bool negation = rd_bool(mt);
				clause.push_back(std::make_pair(mandatory_variables.at(vari), negation));
				num_required++;
			}
			mandatory_id++;
		}
		// Determine if the remaining interval of clause sizes possible
		const int min_added = min_num_var_per_clauses + num_required;
		const int max_added = max_num_var_per_clauses - num_required;
		int size;
		if (min_added > max_added) {
			THROW_EXC6("Impossible to satisfy the requirement min_num_var_per_clauses=", std::to_string(min_num_var_per_clauses), " max_num_var_per_clauses=", std::to_string(max_num_var_per_clauses), "with num_variables=", std::to_string(num_variables), 200);
		}
			
		std::uniform_int_distribution<int> rd_size_added(min_added, max_added);
		// And then choose the remaining variables
		int clause_added_size = rd_size_added(mt);
		for (int j = 0; j < clause_added_size; j++)
		{
			int variable = -1;
			while (variable == -1) {
				int tmpVar = rd_var(mt);
				if (vars_in_curr_clause.find(tmpVar) == vars_in_curr_clause.end()) {
					variable = tmpVar;
				}
			}
			bool negation = rd_bool(mt);
			clause.push_back(std::make_pair(variable, negation));
			vars_in_clauses.insert(variable);
			vars_in_curr_clause.insert(variable);
		}
		clauses->push_back(clause);
	}
	if (vars_in_clauses.size() < num_variables) {
		THROW_EXC6("Problem to satisfy the requirement min_num_var_per_clauses=", 
			std::to_string(min_num_var_per_clauses), 
			" max_num_var_per_clauses=", 
			std::to_string(max_num_var_per_clauses), 
			"with num_variables=", std::to_string(num_variables), 
			201);
	}

	double* weights = new double[num_clauses];
	for (int i = 0; i < num_clauses; i++)
	{
		weights[i] = rd_w(mt);
	}
	return std::make_pair(clauses, weights);
}

