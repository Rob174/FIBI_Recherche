#include "InstanceLoader.h"

int *load_fixed_clauses()
{
    const int size = 8;
    double points[size] = {
        // size_data, (n_vars_in_clause2,weight_clause1 , var1, -var2), (n_vars_in_clause2,weight_clause2 , var1)
        7,
        3, 1, 1, -2,
        2, 1, 1};
    int *points_out = new int[size];
    for (int i = 0; i < size; i++)
    {
        points_out[i] = points[i];
    }
    return points_out;
}
bool *load_fixed_assign()
{
    bool *assign = new bool[2];
    assign[0] = false;
    assign[1] = false;
    // with that we have 1st clause to true (1 -2 -> FALSE V TRUE) and second to false (1 -> FALSE).  One improvement possibility expected
    return assign;
}
std::tuple<std::vector<clause_t>*,weight_t *,n_vars_t>  parse_maxsat(int *data, bool round_vars)
{
    // parse data
    std::vector<double> weights;
    std::vector<clause_t> *clauses = new std::vector<clause_t>;
    int i_clause = 0;
    int clause_size = 0; // to avoid 0%0 error

    const int size = (int)(data[0]) + 1;
    std::set<int> variables;
    int taille = 0;
    bool skipNext = false; // because first is weight
    for (int i = 1; i < size; i++)
    { //
        if (taille == 0)
        {
            taille = data[i];
            skipNext = true;
            clauses->push_back(clause_t());
        }
        else if (skipNext)
        {
            taille--;
            skipNext = false;
            weights.push_back(data[i]);
        }
        else
        {
            const int variable = (int)abs(data[i]) - 1;
            variables.insert(variable);
            clauses->at(clauses->size() - 1).push_back(std::make_pair(variable, sgn<double>(data[i]) == 1));
            taille--;
        }
    }
    // Si la dernière variable du set (sorted) n'est pas égale au nombre de variables-1
    /*
    if (variables.size() - 1 != *variables.rbegin())
    {
        THROW_EXC4_WARNING(
            "Unexpected number of variables. Expected ",
            std::to_string(variables.size() - 1),
            " found last variable ",
            std::to_string(*variables.rbegin()), 15);
    }
    */
    int num_vars = *variables.rbegin(); // max value (because std::set)
    if (round_vars) {
        double n_vars = std::ceil((double)(num_vars) / 1000.);
        num_vars = n_vars * 1000;
    }
#ifdef SAFE_EXEC
    for (auto& c : *clauses) {
        for (auto& [v, _] : c) {
            if (v >= num_vars) {
                THROW_EXC4("Variable ", std::to_string(v), " is outside of bound of num_vars ", std::to_string(num_vars), 526);
            }
        }
    }
#endif // SAFE_EXEC
    double *weights_p = new double[weights.size()];
    for (int i = 0; i < weights.size(); i++)
        weights_p[i] = weights.at(i);
    return std::make_tuple(clauses,weights_p, num_vars);
}