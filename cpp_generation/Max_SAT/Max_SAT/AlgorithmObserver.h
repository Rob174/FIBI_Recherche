#ifndef ALGORITHMOBSERVER_H
#define ALGORITHMOBSERVER_H

#include "Container.h"
#include <vector>
#include <string>
#include <fstream>
#include "json.hpp"
#include "types.h"

template <typename Container_T, typename SwapType>
class AlgorithmObserver
{
public:
	virtual void on_start(Container_T *container) = 0;
	virtual void on_glob_iter_end(Container_T *container, quality_delta_t delta,const SwapType &s) = 0;
	virtual void on_iter_end(Container_T *container, const SwapType &s) = 0;
	virtual void on_end(Container_T *container) = 0;
};
class JSONStorage
{
private:
	nlohmann::json jsons;

public:
	JSONStorage(){};
	void add_json(const nlohmann::json &json) { jsons.push_back(json); };
	void save_json(std::string filename = "actions.json")
	{
		// Check if the file exists
		std::ifstream
			ifs(filename);
		if (ifs.good())
		{
			// If yes read the json file and append data to the JSON
			std::ifstream i(filename);
			nlohmann::json j;
			i >> j;
			for (auto &e : jsons)
			{
				j.push_back(e);
			}
			std::ofstream o(filename);
			o << j;
		} else {
			// If not create a new json file
			std::ofstream o(filename);
			o << jsons;
		}
	};
};
using swap_t = std::map<std::string, int>;
union MoveElem
{
	swap_t swap;
	quality_t quality;
	std::map<std::string, std::vector<swap_t>> neighbours;
};
template <typename Container_T, typename SwapType>
class AbstractLogger : public AlgorithmObserver<Container_T, SwapType>
{
protected:
	JSONStorage *out;
	nlohmann::json json;
	bool log_neigbours;
	std::vector<nlohmann::json> neighbours;
public:
	AbstractLogger(JSONStorage *out, bool log_neigbours = false) : out(out), log_neigbours(log_neigbours) {};
	virtual void on_start(Container_T *container) = 0;
	void on_glob_iter_end(Container_T *container, quality_delta_t delta, const SwapType &s) = 0;
	virtual void on_iter_end(Container_T *container, const SwapType &s) = 0;
	virtual void on_end(Container_T *container) { this->out->add_json(json); };
};

static TSPSwap _dummy_swap_tsp;
class TSPLogger : public AbstractLogger<TSPContainer, TSPSwap>
{
public:
	TSPLogger(JSONStorage *out, bool log_neigbours = false) : AbstractLogger(out, log_neigbours){};
	void on_start(TSPContainer *container)
	{
		json["problem"] = "tsp";
		json["config"] = container->conf->get_constants();
		json["towns"] = std::vector<point_t>();

		for (town_in_tour_id_t i = 0; i < container->conf->num_choices(); i++)
		{
			point_t point;
			for (dim_id_t j = 0; j < container->conf->NUM_DIM.get(); j++)
			{
				point.push_back(container->get_towns_pos()[i * container->conf->NUM_DIM.get() + j]);
			}
			json["towns"].push_back(point);
		}
		json["steps"] = std::vector<nlohmann::json>();
		this->on_iter_end(container);
	};
	void on_glob_iter_end(TSPContainer *container, quality_delta_t delta, const TSPSwap &s = _dummy_swap_tsp)
	{
		if (log_neigbours)
		{
			// Copy the container and its members
			TSPContainer container_copy(*container);
			container_copy.flip(s, delta);
			// Exchange the towns in the tour
			std::vector<town_id_t> tour;
			for (town_in_tour_id_t i = 0; i < container->conf->num_choices(); i++)
			{
				tour.push_back(container->get_tour()[i]);
			}
			nlohmann::json neighbour{
				{"move", {{"i", s.i}, {"j", s.j}}}, 
				{"quality", container->get_quality_metric()},
				{"tour", tour}
			};

			neighbours.push_back(neighbour);
		}
	};
	void on_iter_end(TSPContainer *container, const TSPSwap &s = _dummy_swap_tsp)
	{
		std::vector<town_id_t> tour;
		for (town_in_tour_id_t i = 0; i < container->conf->num_choices(); i++)
		{
			tour.push_back(container->get_tour()[i]);
		}
		nlohmann::json step;
		step["move"] = {{"i", s.i}, {"j", s.j}};
		step["tour"] = tour;
		step["quality"] = container->get_quality_metric();
		step["neighbours"] = neighbours;
		json["steps"].push_back(step);
		neighbours.clear();
	};
};
static ClusteringSwap _dummy_swap_clustering;
class ClusteringLogger : public AbstractLogger<ClusteringContainer, ClusteringSwap>
{
public:
	ClusteringLogger(JSONStorage *out, bool log_neigbours = false) : AbstractLogger(out, log_neigbours){};
	void on_start(ClusteringContainer *container)
	{
		json["problem"] = "clustering";
		json["config"] = container->conf->get_constants();
		json["points"] = std::vector<point_t>();
		for (point_id_t i = 0; i < container->conf->num_choices(); i++)
		{
			point_t point;
			for (dim_id_t j = 0; j < container->conf->NUM_DIM.get(); j++)
			{
				point.push_back(container->p_c[i * container->conf->NUM_DIM.get() + j]);
			}
			json["points"].push_back(point);
		}
		json["steps"] = std::vector<nlohmann::json>();
		this->on_iter_end(container);
	};
	void on_glob_iter_end(ClusteringContainer *container, quality_delta_t delta, const ClusteringSwap &s = _dummy_swap_clustering)
	{
		if (log_neigbours)
		{
			// Copy the container and its members
			ClusteringContainer container_copy(*container);
			container_copy.flip(s, delta);
			// Exchange the towns in the tour
			std::vector<point_t> centroids;
			for (cluster_id_t i = 0; i < container_copy.conf->NUM_CLUST.get(); i++)
			{
				point_t centroid;
				for (dim_id_t j = 0; j < container_copy.conf->NUM_DIM.get(); j++)
				{
					centroid.push_back(container_copy.c_c[i * container_copy.conf->NUM_DIM.get() + j]);
				}
				centroids.push_back(centroid);
			}
			cluster_assignment_t cluster_assignment;
			for (int i = 0; i < container_copy.conf->num_choices(); i++)
			{
				cluster_assignment.push_back(container_copy.c_a[i]);
			}
			nlohmann::json moveV = {
				{"move", {{"point_id", s.point_id}, {"cluster_src", s.cluster_src}, {"cluster_dst", s.cluster_dst}}}, 
				{"quality", container->get_quality_metric()},
				{"centroids", centroids},
				{"assignements", cluster_assignment}
				};
			neighbours.push_back(moveV);
		}
	};
	void on_iter_end(ClusteringContainer *container, const ClusteringSwap &s = _dummy_swap_clustering)
	{
		

		std::vector<point_t> centroids;
		for (cluster_id_t i = 0; i < container->conf->NUM_CLUST.get(); i++)
		{
			point_t centroid;
			for (dim_id_t j = 0; j < container->conf->NUM_DIM.get(); j++)
			{
				centroid.push_back(container->c_c[i * container->conf->NUM_DIM.get() + j]);
			}
			centroids.push_back(centroid);
		}
		cluster_assignment_t cluster_assignment;
		for (int i = 0; i < container->conf->num_choices(); i++)
		{
			cluster_assignment.push_back(container->c_a[i]);
		}
		nlohmann::json step;
		step["move"] = {{"point_id", s.point_id}, {"cluster_src", s.cluster_src}, {"cluster_dst", s.cluster_dst}};
		step["centroids"] = centroids;
		step["quality"] = container->get_quality_metric();
		step["neighbours"] = neighbours;
		step["assignements"] = (cluster_assignment);
		json["steps"].push_back(step);
	};
};
static MAXSATSwap _dummy_swap_maxsat;
class MAXSATLogger : public AbstractLogger<MAXSATContainer, MAXSATSwap>
{
public:
	MAXSATLogger(JSONStorage *out, bool log_neigbours) : AbstractLogger(out, log_neigbours){};
	void on_start(MAXSATContainer *container)
	{
		json["problem"] = "maxsat";
		json["config"] = container->config->get_constants();
		std::vector<clause_t> clauses = container->get_clauses();
		json["clauses_weights"] = std::vector<double>();
		for(int i=0; i<clauses.size(); i++) {
			json["clauses_weights"].push_back(container->get_pweights()[i]);
		}
		json["clauses"] = clauses;
		json["steps"] = std::vector<nlohmann::json>();
		this->on_iter_end(container);
	};
	void on_glob_iter_end(MAXSATContainer *container, quality_delta_t delta, const MAXSATSwap &s = _dummy_swap_maxsat)
	{
		if (log_neigbours)
		{
			// Copy the container and its members
			MAXSATContainer container_copy(*container);
			container_copy.flip(s, delta);
			// Exchange the towns in the tour
			std::vector<var_assignment_t> assignment;
			for (int i = 0; i < container->get_num_variables(); i++)
			{
				assignment.push_back(container->get_variables_val()[i]);
			}
			nlohmann::json neighbour = {
				{"move", {{"var_id", s.var_id}}}, 
				{"quality", container->get_quality_metric()},
				{"assignments", assignment}
				};
			neighbours.push_back(neighbour);
		}
	};
	void on_iter_end(MAXSATContainer *container, const MAXSATSwap &s = _dummy_swap_maxsat)
	{
		std::vector<var_assignment_t> assignment;
		for (int i = 0; i < container->get_num_variables(); i++)
		{
			assignment.push_back(container->get_variables_val()[i]);
		}
		nlohmann::json step;
		step["move"] = {{"var_id", s.var_id}};
		step["assignments"] = assignment;
		step["quality"] = container->get_quality_metric();
		step["neighbours"] = neighbours;
		json["steps"].push_back(step);
	};
};

#endif