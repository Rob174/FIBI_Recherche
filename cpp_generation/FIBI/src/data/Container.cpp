#include "Container.h"

MAXSATContainer::MAXSATContainer(const double *weights, const int num_variables, bool *init_variables_val, std::vector<clause_t> clauses, MAXSATConfig* config) : num_variables(num_variables), clauses(clauses), num_clauses(clauses.size()), config(config)
{

    this->weights = copy_array<const double, double>(weights, num_clauses);
    this->variables_val = copy_array<bool, bool>(init_variables_val, num_variables);

    this->sat_clauses = new bool[this->num_clauses];
    double obj_init = 0;
    for (clause_id_t i = 0; i < num_clauses; i++)
    {
        const clause_t c = clauses.at(i);
        bool sat = false;
        for (int j = 0; j < c.size(); j++)
        {
            const clause_elem_t c_elem = c.at(j);
            const var_id_t var_id = c_elem.first;
            if (var_id >= num_variables)
            {
                throw std::runtime_error("var_id >= num_variables: " + std::to_string(var_id) + " >= " + std::to_string(num_variables));
            }
            if (this->var_in_clauses.find(var_id) == this->var_in_clauses.end())
            {
                std::vector<clause_id_t> clauses_ids;
                this->var_in_clauses.insert(std::make_pair(var_id, clauses_ids));
            }
            this->var_in_clauses.at(var_id).push_back(i);
            if (c_elem.second == variables_val[var_id])
            {
                sat = true;
            }
        }
        sat_clauses[i] = sat;
        if (sat)
        {
            obj_init += weights[i];
        }
    }
    std::vector<std::string> id_missing;
    for (int i = 0; i < this->num_variables; i++)
    {
        if (this->var_in_clauses.find(i) == this->var_in_clauses.end())
        {
            id_missing.push_back(std::to_string(i));
            this->var_in_clauses.insert(std::make_pair(i, std::vector<int>{}));
        }
    }
};
quality_t MAXSATContainer::compute_quality_metric()
{
    double obj = 0;
    for (clause_id_t i = 0; i < num_clauses; i++)
    {
        const clause_t c = clauses.at(i);
        bool sat = false;
        for (int j = 0; j < c.size(); j++)
        {
            const clause_elem_t c_elem = c.at(j);
            const var_id_t var_id = c_elem.first;
            if (c_elem.second == variables_val[var_id])
            {
                sat = true;
            }
        }
        if (sat)
        {
            obj += weights[i];
        }
    }
    return obj;
}
quality_delta_t MAXSATContainer::test_flip(const MAXSATSwap & test_swap)
{
    double objective_cost_delta = 0;
    if (this->var_in_clauses.find(test_swap.var_id) == this->var_in_clauses.end())
    {
        return 0.;
    }
    for (clause_id_t clause_id : this->var_in_clauses.at(test_swap.var_id))
    {
        const clause_t clause = this->clauses.at(clause_id);
        // check if clause is satisfied
        bool sat = false;
        for (clause_elem_t clause_elem : clause)
        {
            bool variable_val = this->variables_val[clause_elem.first];
            if (clause_elem.first == test_swap.var_id)
            {
                // invert the variable value if is the selected one to flip
                variable_val = !variable_val;
            }
            if (clause_elem.second == variable_val)
            {
                sat = true;
                break;
            }
        }
        // Compare if currently satisfied
        if ((sat && !this->sat_clauses[clause_id]))
        {
            objective_cost_delta += this->weights[clause_id];
        }
        else if (!sat && this->sat_clauses[clause_id])
        {
            objective_cost_delta -= this->weights[clause_id];
        }
    }
    return objective_cost_delta;
}

void MAXSATContainer::flip(const MAXSATSwap & test_swap, const quality_delta_t delta)
{
    // update variable value
    this->variables_val[test_swap.var_id] = !this->variables_val[test_swap.var_id];

    if (this->var_in_clauses.find(test_swap.var_id) == this->var_in_clauses.end())
    {
        return;
    }
    // Update sat clauses
    for (clause_id_t clause_id : this->var_in_clauses.at(test_swap.var_id))
    {
        const clause_t clause = this->clauses.at(clause_id);
        // check if clause is satisfied
        bool sat = false;
        for (clause_elem_t clause_elem : clause)
        {
            bool variable_val = this->variables_val[clause_elem.first];
            if (clause_elem.first == test_swap.var_id)
            {
                // invert the variable value if is the selected one to flip
                variable_val = !variable_val;
            }
            if (clause_elem.second == variable_val)
            {
                sat = true;
                break;
            }
        }
        this->sat_clauses[clause_id] = sat;
    }
    this->update_with_delta_cost(delta);
    this->last_choice = test_swap;
}
const double *MAXSATContainer::get_weights()
{
    return copy_array<const double, double>(weights, num_clauses);
}
const double *MAXSATContainer::get_pweights()
{
    return weights;
}
const std::vector<clause_t> MAXSATContainer::get_clauses()
{
    return this->clauses;
}
std::string MAXSATContainer::clauses_str(std::vector<clause_t> clauses)
{
    std::string msg = "\033[0m Clauses formula: ";
    bool first_clause = true;
    for (clause_t c : clauses)
    {
        if (!first_clause)
        {
            msg.append("\x1B[32m AND \033[0m ");
        }
        msg.append("[");
        bool first = true;
        for (clause_elem_t l : c)
        {
            if (!first)
            {
                msg.append("\x1B[32m OR \033[0m ");
            }
            if (!l.second)
            {
                msg.append("NOT(");
            }
            msg.append("\x1B[33mvar");
            msg.append(std::to_string(l.first));
            msg.append("\033[0m ");
            if (!l.second)
            {
                msg.append(")");
            }
            first = false;
        }
        msg.append("] ");
        first_clause = false;
    }
    return msg;
}

void MAXSATContainer::set_var_value(const var_id_t var_id, bool value)
{
    this->variables_val[var_id] = value;
    for (clause_id_t i : this->var_in_clauses.at(var_id))
    {
        bool sat = false;
        for (auto &lit_p : this->clauses.at(i))
        {
            if (this->variables_val[lit_p.first] == lit_p.second)
            {
                sat = true;
                break;
            }
        }
        this->sat_clauses[i] = sat;
    }
}
void MAXSATContainer::print()
{
    std::cout << "Weights: ";
    for (int i = 0; i < this->num_clauses; i++)
    {
        const double v = this->weights[i];
        if (i != 0)
        {
            std::cout << ", ";
        }
        std::cout << v;
    }
    std::cout << std::endl;

    std::cout << "Variable values: ";
    for (int i = 0; i < this->num_variables; i++)
    {
        const bool v = this->variables_val[i];
        if (i != 0)
        {
            std::cout << ", ";
        }
        if (v)
        {
            std::cout << "\x1B[33mtrue\033[0m ";
        }
        else
        {
            std::cout << "\x1B[33mfalse\033[0m ";
        }
    }
    std::cout << std::endl;
    std::cout << MAXSATContainer::clauses_str(this->clauses);
    std::cout << "Clauses with values of variables: ";
    bool first_clause = true;
    for (clause_t c : this->clauses)
    {
        if (!first_clause)
        {
            std::cout << "\x1B[32m AND \033[0m ";
        }
        std::cout << "[ ";
        bool first = true;
        for (clause_elem_t l : c)
        {
            if (!first)
            {
                std::cout << "\x1B[32m OR \033[0m ";
            }
            if (!l.second)
            {
                std::cout << "NOT(";
            }
            const bool v = this->variables_val[l.first];
            if (v)
            {
                std::cout << "\x1B[33mtrue\033[0m ";
            }
            else
            {
                std::cout << "\x1B[33mfalse\033[0m ";
            }
            if (!l.second)
            {
                std::cout << ")";
            }
            first = false;
        }
        std::cout << "] ";
        first_clause = false;
    }
    std::cout << std::endl;
    std::cout << "Saturated clauses: ";
    for (int i = 0; i < this->num_clauses; i++)
    {
        const bool v = this->sat_clauses[i];
        if (i != 0)
        {
            std::cout << ", ";
        }
        if (v)
        {
            std::cout << "\x1B[33mtrue\033[0m ";
        }
        else
        {
            std::cout << "\x1B[33mfalse\033[0m ";
        }
    }
    std::cout << std::endl;
    std::cout << "Objective (weighted num of clauses satisfied): " << this->quality_metric;
    std::cout << std::endl;
}
ClusteringContainer::ClusteringContainer(const double *points, int *assignements, ClusteringConfig *conf) : p_c(points), c_a(assignements), conf(conf), AbstractContainer<ClusteringSwap>()
{
    double *c_c = new double[conf->NUM_CLUST.get() * conf->NUM_DIM.get()];
    for (int i = 0; i < conf->NUM_CLUST.get() * conf->NUM_DIM.get(); i++)
    {
        c_c[i] = 0;
    }
    int *n_p_p_c = new int[conf->NUM_CLUST.get()];
    for (int i = 0; i < conf->NUM_CLUST.get(); i++)
    {
        n_p_p_c[i] = 0;
    }
    for (int i = 0; i < conf->NUM_POINTS.get(); i++)
    {
        n_p_p_c[assignements[i]]++;
        for (int d = 0; d < conf->NUM_DIM.get(); d++)
        {
            const double v = points[i * conf->NUM_DIM.get() + d];
            c_c[assignements[i] * conf->NUM_DIM.get() + d] += v;
        }
    }
    for (int a = 0; a < conf->NUM_CLUST.get() * conf->NUM_DIM.get(); a++)
    {
        c_c[a] /= n_p_p_c[a / conf->NUM_DIM.get()];
    }
    this->c_c = c_c;
    this->n_p_p_c = n_p_p_c;
}
//#define DEBUG
#define EPSILON 1e-5
void ClusteringContainer::flip(const ClusteringSwap& swap, quality_delta_t delta)
{
#ifdef DEBUG
    for (int i = 0; i < this->conf->NUM_POINTS.get(); i++)
    {
        std::cout << this->c_a[i] << ",";
    }
    std::cout << std::endl;
    const int past_cost = this->compute_quality_metric();
#endif
    // f: source cluster
    // t: destination cluster
    // update centroids
    const double *point = &(this->p_c[swap.point_id * this->conf->NUM_DIM.get()]);
    for (int dim = 0; dim < this->conf->NUM_DIM.get(); dim++)
    {
        if (this->n_p_p_c[swap.cluster_src] <= 1) // If we had one point
            this->c_c[swap.cluster_src * this->conf->NUM_DIM.get() + dim] = 0.;
        else
            this->c_c[swap.cluster_src * this->conf->NUM_DIM.get() + dim] = (double)(this->n_p_p_c[swap.cluster_src] * this->c_c[swap.cluster_src * this->conf->NUM_DIM.get() + dim] - point[dim]) /
                                                              (double)(this->n_p_p_c[swap.cluster_src] - 1);
        this->c_c[swap.cluster_dst * this->conf->NUM_DIM.get() + dim] = (double)(this->n_p_p_c[swap.cluster_dst] * this->c_c[swap.cluster_dst * this->conf->NUM_DIM.get() + dim] + point[dim]) /
                                                          (double)(this->n_p_p_c[swap.cluster_dst] + 1);
    }
    // update num_pts_per_clust
    this->n_p_p_c[swap.cluster_dst] = this->n_p_p_c[swap.cluster_dst] + 1;
    this->n_p_p_c[swap.cluster_src] = this->n_p_p_c[swap.cluster_src] - 1;
    this->c_a[swap.point_id] = swap.cluster_dst;

#ifdef DEBUG
    const double real_cost = this->compute_quality_metric();
    const double delta_modif = this->test_flip(swap);
    const double orig_cost = this->get_quality_metric();
    this->update_with_delta_cost(delta);
    const double error = abs(this->get_quality_metric() - real_cost);
    if (error > EPSILON)
    {
        this->test_flip(swap);
        this->compute_quality_metric();
        for (int i = 0; i < this->conf->NUM_POINTS.get() * this->conf->NUM_DIM.get(); i++)
        {
            std::cout << this->p_c[i] << ",";
        }
        std::cout << std::endl;
        std::cout << "After" << std::endl;
        for (int i = 0; i < this->conf->NUM_POINTS.get(); i++)
        {
            // If we are at the point_id change color
            if (i == swap.point_id)
            {
                std::cout << "\033[1;31m" << this->c_a[i] << "\033[0m,";
            }
            else
            {
                std::cout << this->c_a[i] << ",";
            }
        }
        std::cout << std::endl;
        throw std::exception("Non matching costs");
    }
#else
    this->update_with_delta_cost(delta);
#endif
    this->last_choice = swap;
}
quality_t ClusteringContainer::compute_quality_metric()
{
    // compute cost
    double cost = 0;
    for (int i = 0; i < this->conf->NUM_POINTS.get(); i++)
    {
        const double *point = &(this->p_c[i * this->conf->NUM_DIM.get()]);
        double *centroid = &(this->c_c[this->c_a[i] * this->conf->NUM_DIM.get()]);
        const double d = dist_squared(point, centroid, this->conf->NUM_DIM.get());
        cost += d;
    }
    return cost;
}
quality_delta_t ClusteringContainer::test_flip(const ClusteringSwap & test_swap)
{
    // f: source cluster
    // t: destination cluster
    ClusteringConfig *conf = this->conf;
    const double * point = &(this->p_c[test_swap.point_id * this->conf->NUM_DIM.get()]);
    double part1 = (double)(this->n_p_p_c[test_swap.cluster_dst]) / (double)(this->n_p_p_c[test_swap.cluster_dst] + 1) * dist_squared(&(this->c_c[test_swap.cluster_dst * conf->NUM_DIM.get()]), point, conf->NUM_DIM.get());
    double part2 = 0;
    if (this->n_p_p_c[test_swap.cluster_src] > 1)
    {
        part2 = (double)(this->n_p_p_c[test_swap.cluster_src]) / (double)(this->n_p_p_c[test_swap.cluster_src] - 1) * dist_squared(&(this->c_c[test_swap.cluster_src * conf->NUM_DIM.get()]), point, conf->NUM_DIM.get());
    }
    return part1 - part2;
}
void ClusteringContainer::print()
{
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "\tPoints: " << std::endl;
    for (int i = 0; i < this->conf->NUM_POINTS.get(); i++)
    {
        for (int j = 0; j < this->conf->NUM_DIM.get(); j++)
        {
            std::cout << this->p_c[i * this->conf->NUM_DIM.get() + j] << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "\tCentroids: " << std::endl;
    for (int i = 0; i < this->conf->NUM_CLUST.get(); i++)
    {
        for (int j = 0; j < this->conf->NUM_DIM.get(); j++)
        {
            std::cout << this->c_c[i * this->conf->NUM_DIM.get() + j] << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "\tAssignements: " << std::endl;
    for (int i = 0; i < this->conf->NUM_POINTS.get(); i++)
    {
        std::cout << this->c_a[i] << ",";
    }
    std::cout << std::endl;
    std::cout << "\tCost: " << this->cost << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
DistanceMatrix::DistanceMatrix(const int num_towns, const int num_dim, const vector<double>& towns_pos) : num_towns(num_towns), towns_pos(towns_pos)
{
    vector<vector<double>> d(num_towns - 1);
    for (int i_pt = 1; i_pt < num_towns; i_pt++)
    { // 1 not an error (distance from a point to the same point trivial)
        d.at(i_pt - 1) = vector<double>(i_pt);
        for (int j_pt = 0; j_pt < i_pt; j_pt++)
        { // distance from a point to the same point trivial so excluded here
            d.at(i_pt - 1).at(j_pt) = dist(&towns_pos[i_pt * num_dim], &towns_pos[j_pt * num_dim], num_dim);
        }
    }
    this->distances = d;
}

const double DistanceMatrix::get(const town_id_t pt1_idx, const town_id_t pt2_idx) const 
{
    int id1, id2;
    if (pt2_idx == pt1_idx)
        return 0;
    else if (pt2_idx > pt1_idx)
    {
        id1 = pt2_idx;
        id2 = pt1_idx;
    }
    else
    {
        id1 = pt1_idx;
        id2 = pt2_idx;
    }
    // Correct ids due to the fact that we erased the first line of the distance matrix
    id1--;
    return this->distances.at(id1).at(id2);
}
/*
//#define DETAILS
quality_delta_t TSPContainer::test_flip(const TSPSwap & test_swap)
{
    const town_id_t pi = this->tour.at(test_swap.i),
              pj = this->tour.at(test_swap.j),
              pi_1 = this->tour.at(this->cycle_id(test_swap.i + 1)),
              pj_1 = this->tour.at(this->cycle_id(test_swap.j + 1));
#ifdef DETAILS
    const double new1 = this->dist->get(pi, pj),
                 new2 = this->dist->get(pi_1, pj_1),
                 past1 = this->dist->get(pi, pi_1),
                 past2 = this->dist->get(pj, pj_1);
    return new1 + new2 - past1 - past2;
#else
    return this->dist->get(pi, pj) + this->dist->get(pi_1, pj_1) - this->dist->get(pi, pi_1) - this->dist->get(pj, pj_1);
#endif
}
//#define DEBUG_COST
void TSPContainer::flip(const TSPSwap& swap, const quality_delta_t delta)
{
#ifdef DEBUG_COST
    const double delta_cmpt = test_flip(swap);
    const double past_cost = this->compute_quality_metric();
#endif
    const int num_towns = this->conf->NUM_TOWNS.get();
    int buffer;
    if (swap.i >= this->conf->NUM_TOWNS.get() || swap.i < 0)
    {
        std::cout << "Wrong i with :" << swap.i << std::endl;
        exit(1);
    }
    if (swap.j >= this->conf->NUM_TOWNS.get() || swap.j < 0)
    {
        std::cout << "Wrong j with :" << swap.j << std::endl;
        exit(1);
    }
    if (swap.i == swap.j)
    {
        std::cout << "Wrong ids with :i:" << swap.i << ";j:" << swap.j << std::endl;
        exit(1);
    }
    const int length = swap.j > swap.i ? abs(swap.j - swap.i) : this->conf->NUM_TOWNS.get() - swap.i + swap.j;
    for (int l = 1; l <= length / 2; l++)
    {
        buffer = this->tour.at(cycle_id((swap.i + l)));
        this->tour[cycle_id((swap.i + l))] = this->tour.at(cycle_id(swap.j - l + 1));
        this->tour[cycle_id(swap.j - l + 1)] = buffer;
    }
#ifdef DEBUG_COST
    const double real_cost = this->compute_quality_metric();
    this->update_with_delta_cost(delta);
    if (abs(real_cost - this->get_quality_metric()) > EPSILON)
    {
        throw std::exception("Different costs");
    }
#else
    this->update_with_delta_cost(delta);
#endif
    this->last_choice = swap;
}

quality_t TSPContainer::compute_quality_metric()
{
    double cost = 0;
    for (town_in_tour_id_t i = 0; i < this->conf->NUM_TOWNS.get(); i++)
    {
        const town_id_t p = this->cycle_id(i), pnext = this->cycle_id(i + 1);
        cost += this->dist->get(this->tour.at(p), this->tour.at(pnext));
    }
    return cost;
}

void TSPContainer::print()
{
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "\tTour: " << std::endl;
    for (int i = 0; i < this->conf->NUM_TOWNS.get(); i++)
    {
        std::cout << this->tour[i] << ",";
    }
    std::cout << std::endl;
    std::cout << "\tCost: " << this->get_quality_metric() << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
*/