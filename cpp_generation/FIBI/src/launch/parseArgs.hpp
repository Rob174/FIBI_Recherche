#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
char* getCmdOption(char** begin, char** end, const std::string& option)
{
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

using namespace std;
class Args {
public:
    string problem;
    string fileout;
    string root_data;
    int num_threads;
    int merge_size;
    int start_seed;
    int end_seed;
    int num_rep;

    Args(optional<string> problem = nullopt, optional<string> fileout = nullopt, optional<string> root_data = nullopt, optional<int> num_threads = nullopt, optional<int> merge_size = nullopt, optional<int> start_seed = nullopt, optional<int> end_seed = nullopt, optional<int> num_rep = nullopt) :
        problem(problem.value_or("tsp")), fileout(fileout.value_or("./merged.txt")), root_data(root_data.value_or("./data/folder/")), num_threads(num_threads.value_or(-1)), merge_size(merge_size.value_or(100)), start_seed(start_seed.value_or(0)), end_seed(end_seed.value_or(-1)), num_rep(num_rep.value_or(1000)) {
        // Check if fileout exists
        if (!std::filesystem::exists(this->fileout)) {
            cout << "Creating output file " << this->fileout << endl;
            std::ofstream file(this->fileout);
            file.close();
        }
    }

    void set_fileout(const char* value) {
        fileout = value;
    }

    void set_root_data(const char* value) {
        root_data = value;
    }

    void set_num_threads(const char* value) {
        num_threads = atoi(value);
    }

    void set_merge_size(const char* value) {
        merge_size = atoi(value);
    }

    void set_start_seed(const char* value) {
        start_seed = atoi(value);
    }

    void set_end_seed(const char* value) {
        end_seed = atoi(value);
    }

    void set_num_rep(const char* value) {
        num_rep = atoi(value);
    }
};
template <typename T>
constexpr std::optional<T> parseVar(const string& name, const int argc, char** argv) {
    static_assert(std::is_same_v<T, int> || std::is_same_v<T, std::string>, "T must be either int or string");
    std::string argName = "-" + name;
    if (!cmdOptionExists(argv, argv + argc, argName.c_str())) {
        return std::nullopt;
    }

    std::cout << "Using " << name << ": ";
    if constexpr (std::is_same_v<T, int>) {
        int v = std::stoi(getCmdOption(argv, argv + argc, argName.c_str()));
        std::cout << v << std::endl;
        return v;
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        std::string v = getCmdOption(argv, argv + argc, argName.c_str());
        std::cout << v << std::endl;
        return v;
    }
    return std::nullopt;
}

Args parseArguments(const int argc, char** argv) {
    Args args(
        parseVar<string>("problem", argc, argv),
        parseVar<string>("path_out", argc, argv),
        parseVar<string>("root_data", argc, argv),
        parseVar<int>("n_treads", argc, argv),
        parseVar<int>("merge_size", argc, argv),
        parseVar<int>("seed_start", argc, argv),
        parseVar<int>("seed_end", argc, argv),
        parseVar<int>("num_rep", argc, argv)
    );
    return args;
}