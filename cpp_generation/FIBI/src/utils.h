#pragma once
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <sstream>
#include <fstream>
#include "../libs/json.hpp"
#include "algorithm/stats/metrics.hpp"

const double EPSILON = 1e-5;
template <typename T>
class Uninitialized
{
private:
	T value;
	bool initialized = false;
public:
	Uninitialized() {}
	Uninitialized(T value) {
		this->value = value;
		this->initialized = true;
	}
	void operator=(T value) {
		this->value = value;
		this->initialized = true;
	}
	void set(T value) {
		this->value = value;
		this->initialized = true;
	}
	T get() const {
		if (!initialized) {
			cout << "Uninitialized value" << endl;
			throw std::runtime_error("Uninitialized value");
		}
		return value;
	}
	bool is_initialized() const {
		return initialized;
	}
};
std::wstring to_wstring_p(std::string s);

std::string to_string(const char* v);
int binomial_coeff(int k, int n);
template <typename T, typename Tout>
Tout* copy_array(T* array, int size)
{
	Tout* new_array = new Tout[size];
	for (int i = 0; i < size; i++)
	{
		new_array[i] = array[i];
	}
	return new_array;
};
// equivalent of python join
std::string join(std::vector<std::string> texts, std::string separator = " ");
template <typename T>
int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
};
template <typename T_data = double, typename T_cont = const std::vector<double>&, bool safeAccess = false>
class Slice {
private:
	const T_cont data;
	const unsigned int offset;
public:
	Slice(const T_cont data, const unsigned int offset) : offset(offset), data(data) {
	}
	T_data operator[](const unsigned int index) const {
		if constexpr (safeAccess) {
			return data.at(offset + index);
		}
		else
		{
			return data[offset + index];
		}
	}
};
template <typename T>
double dist_squared(const T p1, const T p2, const int num_dim)
{
	double d = 0;
	for (int i = 0; i < num_dim; i++)
	{
		const double v = (p1[i] - p2[i]);
		d += v * v;
	}
	return d;
};
template <typename T>
double dist(const T p1, const T p2, const int num_dim)
{
	return sqrt(dist_squared(p1, p2, num_dim));
};
template <typename T>
T between(T start, T end, T value) {
	if (value < start) {
		return start;
	}
	else if (value > end) {
		return end;
	}
	else {
		return value;
	}
};
using namespace std;
using json = nlohmann::json;


template <typename T_Swap, typename T_Container>
vector<pair<string, double>>* get_results(const Metrics < T_Swap, T_Container>* m, const AbstractConfig* c) {
	vector<pair<string, double>>* res = new vector<pair<string, double>>{};
	for (pair<string, double> p : m->get_data()) {
		res->push_back(p);
	}
	for (pair<string, double> p : c->get_json()) {
		res->push_back(p);
	}
	return res;
}
template <typename T_Config, typename T_Swap, typename T_Container>
void save_mapping(const Metrics < T_Swap, T_Container>* m, const AbstractConfig* c, const string filename = "mapping.json") {

	int i = 0;
	json mapping = vector<pair<string, double>>{};
	for (pair<string, double> p : m->get_data()) {
		const auto k = p.first;
		mapping.push_back({ {"index", i}, {"name", k} });
		i++;
	}
	for (pair<string, double> p : c->get_json()) {
		const auto k = p.first;
		mapping.push_back({ {"index", i}, {"name", k} });
		i++;
	}
	std::ofstream o(filename);
	if (!o)
	{
		throw std::runtime_error("Could not open file " + filename);
	}
	o << mapping.dump() << std::endl;
	o.close();
}