#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include <fstream>
#include <math.h>
#include <map>
#include "../launch/parseArgs.hpp"
#include "../../libs/json.hpp"
#include "./StatsRegisterer.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <string>
using namespace std;
using data_t = long long int;

void parse_header_line(const string line, StatsRegisterer &stats)
{
    istringstream iss(line);
    string problem_type, wcnf, num_vars, num_clauses;
    iss >> problem_type >> wcnf >> num_vars >> num_clauses;
    stats.set_num_clauses(stol(num_clauses.c_str()));
    stats.set_num_variables(stol(num_vars.c_str()));
}
vector<data_t> parse_clause(const string line, bool weighted, const StatsRegisterer &s)
{
    if (!s.stats_ready())
        throw runtime_error("Expected stats to be ready");
    using namespace std;
    istringstream iss(line);
    data_t weight = 1;
    vector<data_t> variables;

    if (weighted)
    {
        string weight_str;
        iss >> weight_str;
        weight = stoll(weight_str.c_str());
    }
    variables.push_back(weight);
    string var_str;
    while (iss >> var_str)
    {
        const data_t v = stoll(var_str.c_str());
        if (v == 0)
            continue; // Skip the final 0
        variables.push_back(v);
    }
    return variables;
}
bool keep(const StatsRegisterer &s)
{
    if (!s.stats_ready())
        return true; // To skip header lines in all cases before arriving to p wcnf ...
    if (!(1000 <= s.num_clauses() && s.num_clauses() <= 5000))
    {
        cout << "Too many clauses with " << s.num_clauses() << endl;
        return false;
    }
    if (!(1000 <= s.num_variables() && s.num_variables() <= 5000))
    {
        cout << "Too many variables with " << s.num_variables() << endl;
        return false;
    }
    return true;
}
void instance_to_file(const vector<data_t> &instance, const string path_out)
{
    // open file with name id.txt under folder
    std::ofstream file(path_out);
    // write wcnf to file with \n separator
    for (const auto &v : instance)
    {
        file << v << "\n";
    }
    file.close();
}
class FolderParser
{
private:
    string command_7z;
    JSONStorage s;
    int i_written;
    filesystem::path path_out;

public:
    FolderParser(filesystem::path path_out, filesystem::path path_7z) : path_out(path_out)
    {
        string path_7z_str = path_7z.string();
        command_7z = string("\"") + (path_7z_str) + string("\" e ");
        i_written = 0;
    }
    void parse_folder(filesystem::path folder)
    {
        for (const auto &entry : std::filesystem::directory_iterator(folder))
        {
            // If it is a file, parse it
            if (std::filesystem::is_regular_file(entry))
            {
                parse_file(entry.path());
            }
            // If it is a folder iterate in it
            else if (std::filesystem::is_directory(entry))
            {
                parse_folder(entry.path());
            }
        }
    }
    template <bool stats_only = true>
    void parse_file(filesystem::path file_name)
    {
        // Check that the extension of the file is gz
        if (file_name.extension() != ".gz")
        {
            return;
        }
        string out_file = "in.wcnf";
        // Extract the archive
        string file_path = filesystem::absolute(file_name).string();

        size_t pos = file_path.find('/');
        while (pos != std::string::npos)
        {
            file_path.replace(pos, 1, "--");
            pos = file_path.find('/', pos + 2);
        }
        string command = command_7z + file_path + string(" -so > " + out_file);
        // Run the command
        std::system(command.c_str());
        // Parse what is inside the folder created
        ifstream file(out_file);
        if (!file.is_open())
        {
            cout << "Tried to open " << out_file << " failed" << endl;
            return;
        }
        string line;
        vector<data_t> lines;
        set<char> skippables = {'c', 's', 'o', 'v'};
        string orig_name = file_name.string();
        size_t pos1 = orig_name.find('/');
        while (pos1 != std::string::npos)
        {
            orig_name.replace(pos1, 1, "--");
            pos1 = orig_name.find('/', pos1 + 2);
        }
        StatsRegisterer r(i_written, orig_name);
        while (getline(file, line))
        {
            if (skippables.find(line.at(0)) != skippables.end() || line == "")
            { // skip lines of headers except the p with stats
                // potential last line empty
                continue;
            }
            if (line.at(0) == 'p')
            {
                parse_header_line(line, r);
                // Check if keep or not
                if (!keep(r))
                {
                    file.close();
                    remove(filesystem::absolute(out_file));
                    return;
                }
                if constexpr (stats_only)
                {
                    cout << "Stopping because we only parse the header" << endl;
                    break;
                }
            }
            else
            {
                if constexpr (stats_only)
                {
                    file.close();
                    remove(filesystem::absolute(out_file));
                    return;
                }
                vector<data_t> line_v = parse_clause(line, true, r);
                lines.push_back(line_v.size());
                for (data_t v : line_v)
                {
                    lines.push_back(v);
                }
            }
        }
        if constexpr (!stats_only)
        {
            instance_to_file(lines, (path_out / (to_string(i_written) + ".txt")).string());
        }
        file.close();
        // Delete the file
        remove(filesystem::absolute(out_file));
        i_written += 1;
        cout << "Wrote instance " << i_written << "/" << file_name.string() << endl;
        // Add the json content of the stats
        s.add_json(r.get_json());
        s.save_json("stats.json");
    }
};
void main_write_data(int argc, char **argv)
{
    std::string concatenatedArgs;

    for (int i = 1; i < argc; ++i)
    {
        concatenatedArgs += argv[i];
        if (i < argc - 1)
        {
            concatenatedArgs += " ";
        }
    }
    // 1. Get the arguments
    if (!cmdOptionExists(argv, argv + argc, "-maxsatParse"))
    {
        return;
    }
    optional<string> folder_in = parseVar<string>("folder_in", argc, argv);
    if (!folder_in.has_value())
    {
        throw runtime_error("Must provide the flag folder_in with flags " + concatenatedArgs);
    }
    optional<string> folder_out = parseVar<string>("folder_out", argc, argv);
    if (!folder_out.has_value())
    {
        throw runtime_error("Must provide the flag folder_out with flags " + concatenatedArgs);
    }
    optional<string> path_to_7z = parseVar<string>("path_7z", argc, argv);
    if (!path_to_7z.has_value())
    {
        throw runtime_error("Must provide the flag path_7z with flags " + concatenatedArgs);
    }
    // 2. Check the arguments
    // Does the source folder exists ? If no error
    filesystem::path path_in(folder_in.value());
    if (!filesystem::exists(path_in))
    {
        throw runtime_error(string("Folder in path does not exists: ") + string(folder_in.value()));
    }
    // Does the destination folder exists ? If no create it with a warning
    filesystem::path path_out = folder_out.value();
    if (!filesystem::exists(path_out))
    {
        // If the destination folder doesn't exist, create it
        filesystem::create_directory(path_out);
        // Output a warning message to the console
        cout << "Warning: Destination folder did not exist and was created." << endl;
    }
    // Does the path to 7z exists ?
    filesystem::path path_7z = path_to_7z.value();
    if (!filesystem::exists(path_7z))
    {
        throw runtime_error(string("7z path does not exists: ") + string(path_7z.string()));
    }
    // Then recursive call to explore the folder and subfolders
    FolderParser p(path_out, path_7z);
    p.parse_folder(path_in);
}