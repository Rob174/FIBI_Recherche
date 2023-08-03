#pragma once
#include "./abstract.hpp"
#include "../../types.h"
#include <functional>
#include <map>

using namespace std;
class TSPTwoOptSwap : public SwapChoice
{
public:
    town_in_tour_id_t i;
    town_in_tour_id_t j;

    TSPTwoOptSwap() : i(-1), j(-1) {}
    TSPTwoOptSwap(const TSPTwoOptSwap &other) : i(other.i), j(other.j) {}
    TSPTwoOptSwap(town_in_tour_id_t i, town_in_tour_id_t j) : i(i), j(j), SwapChoice() {}

    std::map<std::string, long> get_dict() override
    {
        std::map<std::string, long> dict;
        dict["i"] = i;
        dict["j"] = j;
        return dict;
    }

    bool operator==(const TSPTwoOptSwap &other) const
    {
        return (i == other.i && j == other.j);
    }

    bool operator!=(const TSPTwoOptSwap &other) const
    {
        return !(*this == other);
    }

    // Overload the string operator
    string str() const
    {
        return "TSPTwoOptSwap(i:" + to_string(this->i) + ",j:" + to_string(this->j) + ")";
    }
};
class TSPThreeOptSwap : public SwapChoice
{
public:
    town_in_tour_id_t i;
    town_in_tour_id_t j;
    town_in_tour_id_t k;
    int possibility_idx;

    TSPThreeOptSwap() : i(-1), j(-1), k(-1), possibility_idx(-1) {}
    TSPThreeOptSwap(town_in_tour_id_t i, town_in_tour_id_t j, town_in_tour_id_t k) : i(i), j(j), k(k), possibility_idx(possibility_idx), SwapChoice() {}

    std::map<std::string, long> get_dict()
    {
        std::map<std::string, long> dict;
        dict["i"] = i;
        dict["j"] = j;
        dict["k"] = k;
        dict["possibility_idx"] = possibility_idx;
        return dict;
    }

    bool operator==(const TSPThreeOptSwap &other) const
    {
        return (i == other.i && j == other.j && k == other.k && possibility_idx == other.possibility_idx);
    }

    bool operator!=(const TSPThreeOptSwap &other) const
    {
        return !(*this == other);
    }

    // Overload the string operator
    string str() const
    {
        return "TSPThreeOptSwap(i:" + to_string(this->i) + ",j:" + to_string(this->j) + ",k:" + to_string(this->k) + ",possibility_idx:" + to_string(this->possibility_idx) + ")";
    }
};

class TSPInsertionSwap : public SwapChoice
{
public:
    town_in_tour_id_t i;
    town_in_tour_id_t new_pos;

    TSPInsertionSwap() : i(-1), new_pos(-1){};
    TSPInsertionSwap(town_in_tour_id_t i, town_in_tour_id_t new_pos) : i(i), new_pos(new_pos){};
    std::map<std::string, long> get_dict()
    {
        std::map<std::string, long> dict;
        dict["i"] = i;
        dict["new_pos"] = new_pos;
        return dict;
    }

    bool operator==(const TSPInsertionSwap &other) const
    {
        return (i == other.i && new_pos == other.new_pos);
    }

    bool operator!=(const TSPInsertionSwap &other) const
    {
        return !(*this == other);
    }

    // Overload the string operator
    string str() const
    {
        return "TSPInsertionSwap(i:" + to_string(this->i) + ",new_pos:" + to_string(this->new_pos) + ")";
    }
};
namespace std
{

    template <>
    struct hash<TSPTwoOptSwap>
    {
        std::size_t operator()(const TSPTwoOptSwap &k) const
        {
            using std::hash;
            using std::size_t;
            using std::string;

            // Compute individual hash values for first,
            // second and third and possibility and combine them using XOR
            // and bit shifting:

            return (hash<int>()(k.i) ^ (hash<int>()(k.j) << 1));
        }
    };
    template <>
    struct hash<TSPThreeOptSwap>
    {
        std::size_t operator()(const TSPThreeOptSwap &k) const
        {
            using std::hash;
            using std::size_t;
            using std::string;

            // Compute individual hash values

            return (hash<int>()(k.i) ^ (hash<int>()(k.j) << 1) ^ (hash<int>()(k.k) << 1) ^ (hash<int>()(k.possibility_idx) << 1));
        }
    };
    template <>
    struct hash<TSPInsertionSwap>
    {
        std::size_t operator()(const TSPInsertionSwap &k) const
        {
            using std::hash;
            using std::size_t;
            using std::string;

            // Compute individual hash values

            return (hash<int>()(k.i) ^ (hash<int>()(k.new_pos) << 1));
        }
    };

}