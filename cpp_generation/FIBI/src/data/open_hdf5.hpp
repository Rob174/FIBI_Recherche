#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <exception>
#include "H5Cpp.h"

using namespace H5;
using namespace std;
template <typename T, typename F>
T open_hdf5(std::string filename, F func)
{
    string abs_path = std::filesystem::absolute(filename).string();
    // Check if path exists
    if (!std::filesystem::exists(abs_path))
    {
		throw runtime_error("Could not open the hdf5 file");
	}
    try {
        // load the file using the C API of hdf5
        H5File file(filename.c_str(), H5F_ACC_RDONLY);
        return func(file);
    }
    catch ( FileIException &error ) {
        // Print current folder and target file path
        std::cerr << "Current folder: " << std::filesystem::current_path() << std::endl;
        std::cerr << "Target file: " << filename << std::endl;
        std::cerr << "Absolute path: " << std::filesystem::absolute(filename) << std::endl;
        string abs_path = std::filesystem::absolute(filename).string();
        // And rethrow the exception
        throw runtime_error("Could not open the hdf5 file");
	}
}
template <typename T_data, typename T_src>
std::vector<T_data>* open_dataset(const H5std_string& name, T_src &src)
{
    try {
        DataSet dataset = src.openDataSet(name.c_str());
        DataSpace space = dataset.getSpace();
        hsize_t rank;
        hsize_t dims[1];
        rank = space.getSimpleExtentDims(dims);
        std::vector<T_data> *data = new std::vector<T_data>(dims[0]);
        try {
            // Choose the output PredType depending of T_data
            if (std::is_same<T_data, double>::value) {
                dataset.read((*data).data(), PredType::NATIVE_DOUBLE, space);
            }
            else if (std::is_same<T_data, int>::value) {
                dataset.read((*data).data(), PredType::NATIVE_INT, space);
            }
        }
        catch (DataSpaceIException& error) {
			throw runtime_error("Could not read the dataset " + name);
		}
        catch (DataTypeIException& error) {
			throw runtime_error("Could not read the dataset " + name);
		}   
        return data;
    }
    catch (DataSetIException& error) {
		throw runtime_error("Could not open the dataset "+ name);
	}
    catch (FileIException& error) {
        throw runtime_error("Could not open the dataset " + name);
    }
}
std::vector<double>* process_group(std::string g, std::string dataset, H5File &f)
{
    try {
        using namespace std;
        Group group_points(f.openGroup(g.c_str()));
        vector<double> *data = open_dataset<double>(dataset, group_points);
        return data;
    }
    catch (GroupIException& error) {
        throw runtime_error("Could not open the group " + g);
    }
    catch (FileIException& error) {
        throw runtime_error("Could not open the group " + g);
    }
}