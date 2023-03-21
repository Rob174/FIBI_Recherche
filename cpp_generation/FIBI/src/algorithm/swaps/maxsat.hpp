#pragma once
#include "./abstract.hpp"
#include "../../types.h"

using namespace std;
class MAXSATSwap : public SwapChoice
{
public:
	var_id_t var_id;
	MAXSATSwap() : var_id(-1) {};
	MAXSATSwap(var_id_t var_id) : var_id(var_id), SwapChoice() {};
	map<string, int> get_dict()
	{
		map<string, int> dict;
		dict["var_id"] = var_id;
		return dict;
	};
};