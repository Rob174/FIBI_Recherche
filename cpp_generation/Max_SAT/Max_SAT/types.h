#ifndef TYPES_H
#define TYPES_H
#include <vector>
#include <map>
#include <string>
/*************General to all problems*********************/
class SwapChoice
{
public:
    SwapChoice(){};
    virtual std::map<std::string, int> get_dict() = 0;
};
using quality_delta_t = double;
using quality_t = double;
using dim_id_t = int;
/*************TSP*********************/
using town_id_t = int;
using town_in_tour_id_t = int;
using tour_t = std::vector<town_id_t>;
class TSPSwap : public SwapChoice
{
public:
    town_in_tour_id_t i;
    town_in_tour_id_t j;
    TSPSwap() : i(-1), j(-1){};
    TSPSwap(town_in_tour_id_t i, town_in_tour_id_t j) : i(i), j(j), SwapChoice(){};
    std::map<std::string, int> get_dict()
    {
        std::map<std::string, int> dict;
        dict["i"] = i;
        dict["j"] = j;
        return dict;
    };
};
/*************Clustering*********************/
using point_id_t = int;
using cluster_id_t = int;
using point_t = std::vector<double>;
using cluster_assignment_t = std::vector<int>;
class ClusteringSwap : public SwapChoice
{
public:
    ClusteringSwap() : point_id(-1), cluster_src(-1), cluster_dst(cluster_dst){};
    ClusteringSwap(point_id_t point_id, cluster_id_t cluster_src, cluster_id_t cluster_dst) : point_id(point_id), cluster_src(cluster_src), cluster_dst(cluster_dst), SwapChoice(){};
    point_id_t point_id;
    cluster_id_t cluster_src;
    cluster_id_t cluster_dst;
    std::map<std::string, int> get_dict()
    {
        std::map<std::string, int> dict;
        dict["point_id"] = point_id;
        dict["cluster_src"] = cluster_src;
        dict["cluster_dst"] = cluster_dst;
        return dict;
    };
};
/*************Max_SAT*********************/

using var_id_t = int;
using clause_id_t = int;
using var_negation_t = bool;
using clause_elem_t = std::pair<var_id_t, var_negation_t>;
using clause_t = std::vector<clause_elem_t>;
using var_assignment_t = bool;
using vars_assignments_t = std::vector<var_assignment_t>;

class MAXSATSwap : public SwapChoice
{
public:
    var_id_t var_id;
    MAXSATSwap() : var_id(-1){};
    MAXSATSwap(var_id_t var_id) : var_id(var_id), SwapChoice(){};
    std::map<std::string, int> get_dict()
    {
        std::map<std::string, int> dict;
        dict["var_id"] = var_id;
        return dict;
    };
};

#endif