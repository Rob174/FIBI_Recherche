#ifndef INSTANCELOADER_H
#define INSTANCELOADER_H
#include "constants.h"
#include <H5Cpp.h>
#include "Container.h"
#include <stdlib.h>
#include <tuple>
#include <algorithm>
#define SAFE_EXEC
using namespace H5;
using weight_t = double;
using n_vars_t = int;
template <typename T>
std::tuple<std::vector<clause_t>*,weight_t *,n_vars_t>  parse_maxsat(T *data, bool round_vars=false)
{
    // parse data
    std::vector<double> weights;
    std::vector<clause_t>* clauses = new std::vector<clause_t>;
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
    int num_vars = *variables.rbegin()+1; // max value (because std::set)
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
    double* weights_p = new double[weights.size()];
    for (int i = 0; i < weights.size(); i++)
        weights_p[i] = weights.at(i);
    return std::make_tuple(clauses, weights_p, num_vars);
}
template <typename T,bool data_is_int>
T* load_points(std::string dataset_name, const int problemIdx) {
    T* points;
    int size = 0;
    try {
        Exception::dontPrint();
        H5std_string DATASET_NAME(std::to_string(problemIdx));
        H5File  file(dataset_name, H5F_ACC_RDWR);
        DataSet dataset = file.openDataSet(DATASET_NAME);
        DataSpace dataspace = dataset.getSpace();
        hsize_t rank;
        hsize_t dims[1];
        rank = dataspace.getSimpleExtentDims(dims);
        size = dims[0];
        points = new T[dims[0] + 1];
        points[0] = (T)size;
        if(data_is_int)
            dataset.read(&points[1], PredType::NATIVE_INT);
        else
            dataset.read(&points[1], PredType::NATIVE_DOUBLE);

    }
    catch (FileIException error) {
        error.printErrorStack();
        THROW_EXC1("FileIException", 13);
    }
    catch (DataSetIException error) {
        error.printErrorStack();
        THROW_EXC1("DataSetIException", 14);
    }
    return points;
}
double* load_fixed_clauses();
bool* load_fixed_assign();
#endif