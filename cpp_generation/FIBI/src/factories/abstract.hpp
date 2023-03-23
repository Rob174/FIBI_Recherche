#pragma once
#include "../data/constants/tsp.hpp"

using namespace std;
template <typename T_Config>
class AbstractFactory {
	virtual void run(T_Config& cf, string root_data = "./", string out_folder = "./", bool clean = false) = 0;
};