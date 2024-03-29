#pragma once
#include "../data/constants/tsp.hpp"

using namespace std;
template <typename T_Config>
class AbstractFactory {
	virtual vector<pair<string, double>>* run(T_Config& cf, string root_data = "./") = 0;
};