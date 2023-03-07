#include "InstanceLoader.h"

double *load_fixed_clauses()
{
    const int size = 8;
    double points[size] = {
        // size_data, (n_vars_in_clause2,weight_clause1 , var1, -var2), (n_vars_in_clause2,weight_clause2 , var1)
        7,
        3, 1, 1, -2,
        2, 1, 1};
    double *points_out = new double[size];
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