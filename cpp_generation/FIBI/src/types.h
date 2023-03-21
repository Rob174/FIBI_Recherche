#ifndef TYPES_H
#define TYPES_H
#include <vector>
#include <map>
#include <string>

// Enumeration of problems
enum class ProblemType
{
	TSP,
	Clustering,
	MaxSAT
};
/*************General to all problems*********************/
using quality_delta_t = double;
using quality_t = double;
using dim_id_t = int;
/*************TSP*********************/
// namespace TSP {
using town_id_t = int;
using town_in_tour_id_t = int;
using tour_t = std::vector<town_id_t>;

//}
/*************Clustering*********************/
// namespace Clustering {
using point_id_t = int;
using cluster_id_t = int;
using point_t = std::vector<double>;
using cluster_assignment_t = std::vector<int>;

//};

/*************Max_SAT*********************/
// namespace MAXSAT {
using var_id_t = unsigned int;
using clause_id_t = unsigned int;
using var_negation_t = bool;
using clause_elem_t = std::pair<var_id_t, var_negation_t>;
using clause_t = std::vector<clause_elem_t>;
using var_assignment_t = bool;
using vars_assignments_t = std::vector<var_assignment_t>;
using weight_t = double;
using n_vars_t = unsigned int;


//}
#endif