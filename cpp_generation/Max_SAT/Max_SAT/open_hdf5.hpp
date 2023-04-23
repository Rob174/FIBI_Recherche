#pragma once
#include <iostream>
#include <vector>
#include "H5Cpp.h"
using namespace H5;
template <typename T, typename F>
T open_hdf5(std::string filename, F func)
{
    const H5std_string FILE_NAME(filename);
#if DEBUG
    // Check if the file exists
    if (!H5Fis_hdf5(FILE_NAME.c_str()))
    {
        throw std::exception("The file is not a HDF5 file");
    }
#endif
    H5File file(FILE_NAME, H5F_ACC_RDWR);
    return func(file);
}
template <typename T_data, typename T_src>
std::vector<T_data> open_dataset(H5std_string name, T_src src)
{
    DataSet dataset = src.openDataSet(name);
    DataSpace space = dataset.getSpace();
    hsize_t rank;
    hsize_t dims[1];
    rank = dataspace_dims.getSimpleExtentDims(dims);
    std::vector<T_data> data(dims[0]);
    dataset_dims.read(data.data(), T_data(), space);
    return data;
}
std::vector<double> process_group(std::string g, std::string dataset, H5File &f)
{
    using namespace std;
    Group group_points(f.openGroup(g));
    vector<double> data = open_dataset<double>(dataset, group_points);
    return data;
}