#include "StatsRegisterer.h"

void StatsRegisterer::set_num_clauses(data_t n) {
	this->j["num_clauses"] = n;
}
void StatsRegisterer::set_num_variables(data_t n) {
	this->j["num_variables"] = n;
}
StatsRegisterer::StatsRegisterer(const int id, std::string orig_name) {
	this->j["id"] = id;
	this->j["name"] = orig_name;
}
json StatsRegisterer::get_json() {
	return this->j;
}
const bool StatsRegisterer::stats_ready() const{
	// Check that we have id, num_clauses and num_variables
	if (this->j.find("num_clauses") == this->j.end()) return false;
	if (this->j.find("num_variables") == this->j.end()) return false;
	if (this->j.find("id") == this->j.end()) return false;
	return true;
}

const data_t StatsRegisterer::num_clauses() const {
	return this->j["num_clauses"];
}
const data_t StatsRegisterer::num_variables() const {
	return this->j["num_variables"];
}