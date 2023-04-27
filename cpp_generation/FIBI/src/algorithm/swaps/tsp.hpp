#pragma once
#include "./abstract.hpp"
#include "../../types.h"
#include <functional>

using namespace std;
class TSPSwap : public SwapChoice
{
public:
	town_in_tour_id_t i;
	town_in_tour_id_t j;
	TSPSwap() : i(-1), j(-1) {};
	TSPSwap(town_in_tour_id_t i, town_in_tour_id_t j) : i(i), j(j), SwapChoice() {};
	map<string,long> get_dict()
	{
		map<string,long> dict;
		dict["i"] = i;
		dict["j"] = j;
		return dict;
	};
	bool operator==(const TSPSwap& other) const
	{
		return (i == other.i && j == other.j);
	};
	bool operator!=(const TSPSwap& other) const
	{
		return !(*this == other);
	};
	// Overload the string operator
	string str() const
	{
		return "TSPSwap(i:" + to_string(this->i) + ",j:" + to_string(this->j) + ")";
	};
};
namespace std {

	template <>
	struct hash<TSPSwap>
	{
		std::size_t operator()(const TSPSwap& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<int>()(k.i)
				^ (hash<int>()(k.j) << 1)) >> 1);
		}
	};

}