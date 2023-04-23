#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <exception>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <thread>

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <set>
#include <iomanip>
#ifdef _WIN32
#include <Windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#endif


using namespace std;

#ifdef HDF5save


#include "H5Cpp.h"
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
bool save_metadata(const unsigned long seed_glob, const vector < pair<string, double>>& values, string filename = "dataset.hdf5") {
	// Save current absolute path
	string current_path = filesystem::current_path().string();
	if constexpr (debug) {
		// print current folder
		cout << "Current folder: " << current_path << endl;
	}
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
		return true;
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
#else

/** @brief Remove the dataset folder
* */
void clean_dataset(string foldername = "dataset/") {
	// Remove the folder and all its content
	try {
		filesystem::remove_all(foldername);
	}
	catch (...) {
		cout << "Current folder: " << filesystem::current_path().string() << endl;
	}
}
class MergeMetadata {
	using json = nlohmann::json;
private:
	bool merged_exists_;
	const int size_merge;
	vector<vector<pair<string, double>>*> metrics;
	string filename;
	// Private helper function to get the full path of the merged file
	string merged_file_path() const {
		return filename;
	}

public:
	MergeMetadata(const int size_merge = 1000, const string filename = "merged.txt") : filename(filename), size_merge(size_merge) {
		// Check if the merged file already exists
		merged_exists_ = false;
		ifstream merged_file(merged_file_path());
		if (merged_file.good()) {
			merged_exists_ = true;
			merged_file.close();
		}
	}
	void add_metadata(vector<pair<string,double>>* value) {
		if (value->size() == 0) {
			throw runtime_error("Expected non empty metadata");
		}
		metrics.push_back(value);
	}
	bool ready_to_merge() const {
		return metrics.size() > size_merge;
	}
	void merge() {
		cout << "Merging..." << metrics.size() << " elements..." << endl;
		// Open the merged file in append mode
		ofstream merged_file(merged_file_path(), ios::app);

		// Loop over the seeds to merge
		int k = 0;
		for (const vector<pair<string, double>>* metadata : metrics) {
			// Write the metadata as a JSON-like string
			if (metadata->size() == 0) throw runtime_error("Expected non empty metadata");
			k++;
			merged_file << "{";
			int i = 0;
			for (auto it = metadata->begin(); it != metadata->end(); ++it) {

				merged_file << "\"" << (*it).first << "\"" << ":" << std::scientific << std::setprecision(25) << (*it).second;
				if (std::next(it) != metadata->end()) {
					merged_file << ", ";
				}
				i++;
			}
			merged_file << "},\n";
		}
		cout << "---> Merged " << k << " elements" << endl;
		for (vector<pair<string, double>>* metadata : metrics) {
			delete metadata;
		}
		metrics.clear();

		// Close the merged file
		merged_file.close();
	}

};
#endif // HDF5save
