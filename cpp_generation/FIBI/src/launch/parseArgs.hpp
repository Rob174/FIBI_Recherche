#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>
#include <iostream>
#include <optional>
char *getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

using namespace std;
class Args
{
public:
    string problem;
    string fileout;
    string root_data;
    string root;
    string all;
    int dataset;
    int impr;
    int num_threads;
    int merge_size;
    int start_seed;
    int end_seed;
    int num_rep;
    int opt;
    int town_max;
    int do_missings;// 0 do not do the missings, 1 do the missing no matter seed start and end, 2 do missing in interval [start,end]

    Args(optional<string> problem = nullopt, optional<string> fileout = nullopt, optional<string> root_data = nullopt,
         optional<int> num_threads = nullopt, optional<int> merge_size = nullopt, optional<int> start_seed = nullopt,
         optional<int> end_seed = nullopt, optional<int> num_rep = nullopt, optional<int> dataset = nullopt, optional<int> impr = nullopt,
         optional<string> all = nullopt, optional<int> opt = nullopt, optional<int> town_max = nullopt, optional<int> do_missings = nullopt,
         optional<string> root = nullopt) : 
         problem(problem.value_or("tsp")), fileout(fileout.value_or("./merged.txt")),
         root_data(root_data.value_or("./data/folder/")), num_threads(num_threads.value_or(-1)),
        merge_size(merge_size.value_or(100)), start_seed(start_seed.value_or(0)), end_seed(end_seed.value_or(-1)),
        num_rep(num_rep.value_or(1000)), dataset(dataset.value_or(-1)), impr(impr.value_or(-1)), all(all.value_or("")), 
        opt(opt.value_or(2)), town_max(town_max.value_or(-1)), do_missings(do_missings.value_or(0)), root(root.value_or("./"))
    {
        // Check if fileout exists
        if (!std::filesystem::exists(this->fileout))
        {
            cout << "Creating output file " << this->fileout << endl;
            std::ofstream file(this->fileout);
            file.close();
        }
    }

    void set_fileout(const char *value)
    {
        fileout = value;
    }

    void set_root_data(const char *value)
    {
        root_data = value;
    }

    void set_num_threads(const char *value)
    {
        num_threads = atoi(value);
    }

    void set_merge_size(const char *value)
    {
        merge_size = atoi(value);
    }

    void set_start_seed(const char *value)
    {
        start_seed = atoi(value);
    }

    void set_end_seed(const char *value)
    {
        end_seed = atoi(value);
    }

    void set_num_rep(const char *value)
    {
        num_rep = atoi(value);
    }
    void set_dataset(const int value)
    {
        cout << "Setting DATASET " << value << endl;
        dataset = value;
    }
    void set_impr(const int value)
    {
        cout << "Setting IMPR " << value << endl;
        impr = value;
    }
    void set_opt(const int value)
    {
        cout << "Setting OPT " << value << endl;
        opt = value;
    }
};
template <typename T>
constexpr std::optional<T> parseVar(const string &name, const int argc, char **argv)
{
    static_assert(std::is_same_v<T, int> || std::is_same_v<T, std::string>, "T must be either int or string");
    std::string argName = "-" + name;
    if (!cmdOptionExists(argv, argv + argc, argName.c_str()))
    {
        return std::nullopt;
    }

    std::cout << "Using " << name << ": ";
    if constexpr (std::is_same_v<T, int>)
    {
        std::string v_str = getCmdOption(argv, argv + argc, argName.c_str());
        try
        {
            int v = std::stoi(v_str);
            std::cout << v << std::endl;
            return v;
        }
        catch (std::exception &e)
        {
            std::cout << "Error: " << e.what() << std::endl;
            throw std::runtime_error("Invalid value for " + name);
        }
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        std::string v = getCmdOption(argv, argv + argc, argName.c_str());
        std::cout << v << std::endl;
        return v;
    }
    return std::nullopt;
}

Args parseArguments(const int argc, char **argv)
{
    Args args(
        parseVar<string>("problem", argc, argv),
        parseVar<string>("path_out", argc, argv),
        parseVar<string>("root_data", argc, argv),
        parseVar<int>("n_treads", argc, argv),
        parseVar<int>("merge_size", argc, argv),
        parseVar<int>("seed_start", argc, argv),
        parseVar<int>("seed_end", argc, argv),
        parseVar<int>("num_rep", argc, argv),
        parseVar<int>("dataset", argc, argv),
        parseVar<int>("impr", argc, argv),
        parseVar<string>("all", argc, argv),
        parseVar<int>("opt", argc, argv),
        parseVar<int>("town_max", argc, argv),
        parseVar<int>("do_missings", argc, argv),
        parseVar<string>("root", argc, argv));
    return args;
}