#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <map>
#include <exception>
#include <iostream>
#include <fstream>
using namespace std;

#ifdef HDF5save
#include "H5Cpp.h"
using namespace H5;
template <typename T, typename F>
T open_hdf5(string filename, F func)
{
	string abs_path = filesystem::absolute(filename).string();
	// Check if path exists
	if (!filesystem::exists(abs_path))
	{
		throw runtime_error("Could not open the hdf5 file " + filename + " because it does not exist \nAbsolute path:\n" + abs_path);
	}
	try
	{
		// load the file using the C API of hdf5
		H5File file(filename.c_str(), H5F_ACC_RDONLY);
		return func(file);
	}
	catch (FileIException &error)
	{
		// Print current folder and target file path
		cerr << "Current folder: " << filesystem::current_path() << endl;
		cerr << "Target file: " << filename << endl;
		cerr << "Absolute path: " << filesystem::absolute(filename) << endl;
		string abs_path = filesystem::absolute(filename).string();
		// And rethrow the exception
		throw runtime_error("Could not open the hdf5 file");
	}
}
template <typename T_data, typename T_src>
vector<T_data> *open_dataset(const H5std_string &name, T_src &src)
{
	try
	{
		DataSet dataset = src.openDataSet(name.c_str());
		DataSpace space = dataset.getSpace();
		hsize_t rank;
		hsize_t dims[1];
		rank = space.getSimpleExtentDims(dims);
		vector<T_data> *data = new vector<T_data>(dims[0]);
		try
		{
			// Choose the output PredType depending of T_data
			if (is_same<T_data, double>::value)
			{
				dataset.read((*data).data(), PredType::NATIVE_DOUBLE, space);
			}
			else if (is_same<T_data, int>::value)
			{
				dataset.read((*data).data(), PredType::NATIVE_INT, space);
			}
		}
		catch (DataSpaceIException &error)
		{
			throw runtime_error("Could not read the dataset " + name);
		}
		catch (DataTypeIException &error)
		{
			throw runtime_error("Could not read the dataset " + name);
		}
		return data;
	}
	catch (DataSetIException &error)
	{
		throw runtime_error("Could not open the dataset " + name);
	}
	catch (FileIException &error)
	{
		throw runtime_error("Could not open the dataset " + name);
	}
}
vector<double> *process_group(string g, string dataset, H5File &f)
{
	try
	{
		using namespace std;
		Group group_points(f.openGroup(g.c_str()));
		vector<double> *data = open_dataset<double>(dataset, group_points);
		return data;
	}
	catch (GroupIException &error)
	{
		throw runtime_error("Could not open the group " + g);
	}
	catch (FileIException &error)
	{
		throw runtime_error("Could not open the group " + g);
	}
}
#else
#include <filesystem>
template <bool debug = true>
vector<double> *read_file_txt_multiprocessing(const long instance, string foldername)
{
	// Full path of the file
	string filename = foldername + "/" + to_string(instance) + ".txt";
	if constexpr (debug)
	{
		// Print current folder and target file path
		// cout << "Current folder: " << filesystem::current_path() << endl;
		// cout << "Target file: " << filename << endl;
		// cout << "Absolute path: " << filesystem::absolute(filename) << endl;
	}
	ifstream file(filename);
	if (!file.is_open())
	{
		if constexpr (debug)
		{
			cout << "Unable to open file " << filename << endl;
		}
		throw runtime_error("Unable to open file " + filename);
	}
	vector<double> *points = new vector<double>();
	string elem;
	// Each element is separated by a space
	try
	{
		while (file >> elem)
		{
			points->push_back(stod(elem));
		}
		file.close();
	}
	catch (std::exception &e)
	{
		cout << "Unable to read file " << filename << endl;
		cout << e.what() << endl;
		cout << "step12" << endl;
		throw runtime_error("Unable to read file " + filename + "\n" + e.what());
	}
	return points;
}
#endif