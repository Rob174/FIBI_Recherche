// parse_benchmark_maxsat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include "H5Cpp.h"
#include <filesystem>
#include <fstream>
#include <math.h>
#include <map>
#include "argParser.hpp"
#include "json.hpp"
#include "StatsRegisterer.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace H5;
using data_t = long long int;
void create_dataset(std::vector<data_t> values, std::string identifier)
{
    const H5std_string FILE_NAME("dataset.hdf5");
    Exception::dontPrint();
    try
    {
        // Create a new file using the default property lists. check if exists and if not create
        bool exists = std::filesystem::exists(FILE_NAME);
        if (!exists)
        {
            std::cout << "file does not exist, creating it..." << std::endl;
        }
        H5File file(FILE_NAME, !exists ? H5F_ACC_TRUNC : H5F_ACC_RDWR);
        // Save input output of algorithm iterating over the elements to save (ex: metadata, intermediate solutions)
        const int RANK = 1;
        hsize_t dims[RANK];
        dims[0] = values.size();
        DataSpace dataspace(RANK, dims);
        if (file.nameExists(identifier))
        {
            std::cout << "Dataset " << identifier << " already exists" << std::endl;
            exit(10);
        }
        DataSet dataset = file.createDataSet(identifier, PredType::NATIVE_DOUBLE, dataspace);
        double* res = new double[dims[0]];
        for (std::size_t j = 0; j < dims[0]; j++)
        {
            res[j] = values.at(j);
        }
        dataset.write(res, PredType::NATIVE_DOUBLE);
        delete[] res;
    }
    catch (FileIException error)
    {
        error.printErrorStack();
        std::cout << "Error writing dataset: H5File operations" << std::endl;
        exit(-1);
    }
    catch (DataSetIException error)
    {
        error.printErrorStack();
        std::cout << "Error writing dataset: DataSet operations" << std::endl;
        exit(-1);
    }
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
        std::cout << "Error writing dataset: DataSpace operations" << std::endl;
        exit(-1);
    }
    catch (const std::exception& exc)
    {
        std::cout << "Other exception" << std::endl;
        std::cerr << exc.what();
        exit(-1);
    }
}
void parse_header_line(const std::string line, StatsRegisterer & stats) {
    std::istringstream iss(line);
    std::string problem_type, wcnf, num_vars, num_clauses;
    iss >> problem_type >> wcnf >> num_vars >> num_clauses;
    stats.set_num_clauses(std::stol(num_clauses.c_str()));
    stats.set_num_variables(std::stol(num_vars.c_str()));
}
std::vector<data_t> parse_clause(const std::string line, bool weighted, const StatsRegisterer& s) {
    if (!s.stats_ready()) error_msg("Expected stats to be ready");
    using namespace std;
    istringstream iss(line);
    data_t weight = 1;
    vector<data_t> variables;

    if (weighted) {
        string weight_str;
        iss >> weight_str;
        weight = stoll(weight_str.c_str());
    }
    variables.push_back(weight);
    std::string var_str;
    while (iss >> var_str) {
        const data_t v = stoll(var_str.c_str());
        if (v == 0) continue;// Skip the final 0
        variables.push_back(v);
    }
    return variables;
}
bool keep(const StatsRegisterer& s) {
    if (!s.stats_ready()) return true; // To skip header lines in all cases before arriving to p wcnf ...
    if (!(1000 <= s.num_clauses() && s.num_clauses() <= 5000)) {
        std::cout << "Too many clauses with " << s.num_clauses() << std::endl;
        return false;
    }
    if (!(1000 <= s.num_variables() && s.num_variables() <= 5000)) {
        std::cout << "Too many variables with " << s.num_variables() << std::endl;
        return false;
    }
    return true;
}
int main_write_data(int argc, char** argv) {
    bool weighted = false;
    if(P_FLAG_SEEN("-weighted")) {
        weighted = true;
    }
    char* orig_name = P_GET_VAL("-orig_name");
    std::ifstream file("in.wcnf");
    // Get the key for the hdf5 dataset
    char* name = P_GET_VAL("-id");
    std::string identifier(name);
    if (!file.is_open()) {
        std::cout << "Tried to open " << argv[1] << " failed" << std::endl;
        std::cout << "Args found ";
        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << ", ";
        }
        std::cout << std::endl;
        return 2;
    }
    std::string line;
    std::vector<data_t> lines;
    std::set<char> skippables = { 'c','s','o','v' };
    JSONStorage s;
    StatsRegisterer r(std::atoi(name), std::string(orig_name));
    while (std::getline(file, line)) {
        if (skippables.find(line.at(0)) != skippables.end()) {//skip lines of headers except the p with stats
            continue;
        }
        if (line == "") { // potential last line empty
            continue;
        }
        if (line.at(0) == 'p') {
            parse_header_line(line, r);
            // Check if keep or not
            if (!keep(r))
            {
                return 1;
            }
        }
        else {
            std::vector<data_t> line_v = parse_clause(line, weighted, r);
            lines.push_back(line_v.size());
            for (data_t v : line_v) {
                lines.push_back(v);
            }
        }
    }
    file.close();
    s.add_json(r.get_json());
    s.save_json("stats.json");
    create_dataset(lines, identifier);
    return 0;
}
int main(int argc,char** argv)
{
    return main_write_data(argc,argv);
}
