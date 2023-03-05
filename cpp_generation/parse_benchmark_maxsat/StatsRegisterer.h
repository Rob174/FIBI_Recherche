#pragma once
#include "json.hpp"
#include <fstream>
using data_t = long long int;
using json = nlohmann::json;
class StatsRegisterer {
private:
    json j;
public:
    StatsRegisterer(const int id, std::string orig_name);
    void set_num_clauses(data_t n);
    void set_num_variables(data_t n);
	const data_t num_clauses() const;
	const data_t num_variables() const;
	const bool stats_ready() const;
	json get_json();
};

class JSONStorage
{
private:
	nlohmann::json jsons;

public:
	JSONStorage() {};
	void add_json(const nlohmann::json& json) { jsons.push_back(json); };
	void save_json(std::string filename = "stats.json")
	{
		// Check if the file exists
		std::ifstream ifs(filename);
		if (ifs.good())
		{
			// If yes read the json file and append data to the JSON
			std::ifstream i(filename);
			nlohmann::json j;
			i >> j;
			for (auto& e : jsons)
			{
				j.push_back(e);
			}
			std::ofstream o(filename);
			o << j;
		}
		else {
			// If not create a new json file
			std::ofstream o(filename);
			o << jsons;
		}
	};
};