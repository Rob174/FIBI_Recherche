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
std::tuple<std::vector<clause_t>*,weight_t *,n_vars_t>  parse_maxsat(int *data, bool round_vars=false);
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
int* load_fixed_clauses();
bool* load_fixed_assign();
#endif