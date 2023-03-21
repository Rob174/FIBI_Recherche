#pragma once
#include "./abstract.hpp"
#include "../../types.h"

using namespace std;
class TSPSwap : public SwapChoice
{
public:
	town_in_tour_id_t i;
	town_in_tour_id_t j;
	TSPSwap() : i(-1), j(-1) {};
	TSPSwap(town_in_tour_id_t i, town_in_tour_id_t j) : i(i), j(j), SwapChoice() {};
	map<string, int> get_dict()
	{
		map<string, int> dict;
		dict["i"] = i;
		dict["j"] = j;
		return dict;
	};
};