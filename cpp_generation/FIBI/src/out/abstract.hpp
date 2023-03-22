#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <exception>
#include "H5Cpp.h"
#include <cstdio>
#include <mutex>


using namespace std;
using namespace H5;

/** @brief Remove the dataset file
* */
void clean_dataset(string filename = "dataset.hdf5") {
	remove(filename.c_str());
}
/** @brief Save the metadata of the algorithm
* * @param seed_glob The seed of the algorithm
* * @param values The values to save
* * @param filename The name of the file to save the data
* * @tparam debug If true, print debug information (current folder)
* * @return void
* */
template <bool debug = false>
void save_metadata(const unsigned long seed_glob, const vector < pair<string, double>>& values, string filename = "dataset.hdf5") {
	static mutex my_mutex;
	// Save current absolute path
	string current_path = filesystem::current_path().string();
	if constexpr (debug) {
		// print current folder
		cout << "Current folder: " << current_path << endl;
	}
	lock_guard<mutex> my_lock(my_mutex);
	Exception::dontPrint();
	try
	{
		// Create a new file using the default property lists. check if exists and if not create
		bool file_exists = filesystem::exists(filename);
		if (!file_exists)
		{
			cout << "file does not exist, creating it..." << endl;
		}
		H5File file(filename.c_str(), !file_exists ? H5F_ACC_TRUNC : H5F_ACC_RDWR);

		// Save input output of algorithm iterating over the elements to save (ex: metadata, intermediate solutions)
		const int RANK = 1;
		string gpe = "metadata";
		Group group;
		if (file.nameExists(gpe.c_str()))
		{
			group = file.openGroup(gpe.c_str());
		}
		else
		{
			cout << "Creating group " << gpe.c_str() << endl;
			group = file.createGroup(gpe.c_str());
		}
		hsize_t dims[RANK];
		dims[0] = values.size();
		DataSpace dataspace(RANK, dims);
		// identifier is the seed_glob converted to string
		string identifier = to_string(seed_glob);
		if (group.nameExists(identifier.c_str()))
		{
			throw runtime_error("Dataset " + identifier + " already exists.");
		}
		DataSet dataset = group.createDataSet(identifier.c_str(), PredType::NATIVE_DOUBLE, dataspace);
		double* res = new double[dims[0]];
		for (size_t j = 0; j < dims[0]; j++)
		{
			res[j] = values.at(j).second;
		}
		dataset.write(res, PredType::NATIVE_DOUBLE);
	}
	catch (FileIException error)
	{
		error.printErrorStack();
		throw runtime_error("Error writing dataset: File operations");
	}
	catch (DataSetIException error)
	{
		error.printErrorStack();
		throw runtime_error("Error writing dataset: DataSet operations");
	}
	catch (DataSpaceIException error)
	{
		error.printErrorStack();
		throw runtime_error("Error writing dataset: DataSpace operations");
	}
	catch (const exception& exc)
	{
		throw runtime_error("Error writing dataset: " + string(exc.what()));
	}
}