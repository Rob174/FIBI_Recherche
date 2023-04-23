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
	map<string,long> get_dict()
	{
		map<string,long> dict;
		dict["var_id"] = var_id;
		return dict;
	};
	bool operator==(const MAXSATSwap& other) const
	{
		return var_id == other.var_id;
	};
	bool operator!=(const MAXSATSwap& other) const
	{
		return !(*this == other);
	};
	// Overload the string operator
	string str() const
	{
		return "MAXSATSwap(var:" + to_string(var_id) + ")";
	};
};