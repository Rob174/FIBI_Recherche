#pragma once
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
#include <filesystem>
#include <zlib.h>
using namespace std;
using data_t = long long int;

void parse_header_line(const string line, StatsRegisterer& stats) {
    istringstream iss(line);
    string problem_type, wcnf, num_vars, num_clauses;
    iss >> problem_type >> wcnf >> num_vars >> num_clauses;
    stats.set_num_clauses(stol(num_clauses.c_str()));
    stats.set_num_variables(stol(num_vars.c_str()));
}
vector<data_t> parse_clause(const string line, bool weighted, const StatsRegisterer& s) {
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
    string var_str;
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
        cout << "Too many clauses with " << s.num_clauses() << endl;
        return false;
    }
    if (!(1000 <= s.num_variables() && s.num_variables() <= 5000)) {
        cout << "Too many variables with " << s.num_variables() << endl;
        return false;
    }
    return true;
}
class FolderParser {
private:
    string command_7z;
public:
    FolderParser(filesystem::path root_folder, filesystem::path path_7z){
        command_7z = "\"" + path_7z + "\" e ";
    }
    void parse_folder(filesystem::path folder) {
        for (const auto& entry : std::filesystem::directory_iterator(folder)) {
            // If it is a file, parse it
            if (std::filesystem::is_regular_file(entry)) {
                parse_file(entry.path());
            }
            // If it is a folder iterate in it
            else if (std::filesystem::is_directory(entry)) {
                parse_folder(entry.path());
            }
        }
    }
    void parse_file(filesystem::path file) {
        // Check that the extension of the file is gz
        if (file.extension() != ".gz") {
            return;
        }
        // Extract the archive
        string command = command_7z + 
    }
};
void main_write_data(int argc, char** argv) {
    // 1. Get the arguments
    if (!cmdOptionExists(argv, argv + argc, "-maxsatParse")) {
        return;
    }
    optional<string> folder_in = parseVar<string>("-folder_in", argc, argv);
    if (!folder_in.has_value()) {
        throw runtime_error("Must provide the flag folder_in");
    }
    optional<string> folder_out = parseVar<string>("-folder_out", argc, argv);
    if (!folder_out.has_value()) {
        throw runtime_error("Must provide the flag folder_out");
    }
    optional<string> path_to_7z = parseVar<string>("-path_7z", argc, argv);
    if (!path_7z.has_value()) {
        throw runtime_error("Must provide the flag path_7z");
    }
    // 2. Check the arguments
    // Does the source folder exists ? If no error
    using fs = filesystem;
    fs::path path_in = folder_in;
    if (!fs::exists(path_in)) {
        throw runtime_error("Folder in path does not exists: " + folder_in);
    }
    // Does the destination folder exists ? If no create it with a warning
    fs::path path_out = folder_out;
    if (!fs::exists(path_out)) {
        // If the destination folder doesn't exist, create it
        fs::create_directory(path_out);

        // Output a warning message to the console
        cout << "Warning: Destination folder did not exist and was created." << endl;
    }
    // Does the path to 7z exists ?
    fs::path path_7z = path_to_7z;
    if (!fs::exists(path_7z)) {
        throw runtime_error("7z path does not exists: " + path_7z);
    }
    // Then recursive call to explore the folder and subfolders
    char* orig_name = P_GET_VAL("-orig_name");
    ifstream file("in.wcnf");
    // Get the key for the hdf5 dataset
    char* name = P_GET_VAL("-id");
    string identifier(name);
    if (!file.is_open()) {
        cout << "Tried to open " << argv[1] << " failed" << endl;
        cout << "Args found ";
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << ", ";
        }
        cout << endl;
        return 2;
    }
    string line;
    vector<data_t> lines;
    set<char> skippables = { 'c','s','o','v' };
    JSONStorage s;
    StatsRegisterer r(atoi(name), string(orig_name));
    while (getline(file, line)) {
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
            vector<data_t> line_v = parse_clause(line, weighted, r);
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