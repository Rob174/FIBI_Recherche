#pragma once
#include "../data/constants/tsp.hpp"

using namespace std;
template <typename T_Config>
class AbstractFactory {
	virtual void run(T_Config& cf, bool dump_mapping = false, string root_data = "./") = 0;
};