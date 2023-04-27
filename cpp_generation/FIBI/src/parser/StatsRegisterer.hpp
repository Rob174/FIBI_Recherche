#pragma once
#include "json.hpp"
#include <fstream>
using data_t = long long int;
using json = nlohmann::json;
class StatsRegisterer {
private:
	json j;
public:
	StatsRegisterer(const int id, std::string orig_name) {
		this->j["id"] = id;
		this->j["name"] = orig_name;
	};
	void set_num_clauses(data_t n) {
		this->j["num_clauses"] = n;
	};
	void set_num_variables(data_t n) {
		this->j["num_variables"] = n;
	};
	const data_t num_clauses() const {
		return this->j["num_clauses"];
	};
	const data_t num_variables() const {
		return this->j["num_variables"];
	};
	const bool stats_ready() const {
		// Check that we have id, num_clauses and num_variables
		if (this->j.find("num_clauses") == this->j.end()) return false;
		if (this->j.find("num_variables") == this->j.end()) return false;
		if (this->j.find("id") == this->j.end()) return false;
		return true;
	};
	json get_json() {
		return this->j;
	};
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