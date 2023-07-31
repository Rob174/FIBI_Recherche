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
    town_in_tour_id_t k;

    TSPSwap() : i(-1), j(-1), k(-1) {}
    TSPSwap(town_in_tour_id_t i, town_in_tour_id_t j, town_in_tour_id_t k) : i(i), j(j), k(k), SwapChoice() {}
	TSPSwap(town_in_tour_id_t i, town_in_tour_id_t j) : i(i), j(j), k(-1), SwapChoice() {}

    map<string, long> get_dict()
    {
        map<string, long> dict;
        dict["i"] = i;
        dict["j"] = j;
        dict["k"] = k;
        return dict;
    }

    bool operator==(const TSPSwap& other) const
    {
        return (i == other.i && j == other.j && k == other.k);
    }

    bool operator!=(const TSPSwap& other) const
    {
        return !(*this == other);
    }

    // Overload the string operator
    string str() const
    {
        return "TSPSwap(i:" + to_string(this->i) + ",j:" + to_string(this->j) + ",k:" + to_string(this->k) + ")";
    }
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

			return ( (hash<int>()(k.i)
				^ (hash<int>()(k.j) << 1)) >> 1)
				^ (hash<int>()(k.k) << 1);
		}
	};

}