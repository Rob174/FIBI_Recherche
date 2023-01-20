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
	virtual void on_glob_iter_end(Container_T *container) = 0;
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
		std::ofstream o(filename);
		o << jsons;
	};
};
template <typename Container_T, typename SwapType>
class AbstractLogger : public AlgorithmObserver<Container_T, SwapType>
{
protected:
	JSONStorage *out;
	nlohmann::json json;

public:
	AbstractLogger(JSONStorage *out) : out(out){};
	virtual void on_start(Container_T *container) = 0;
	void on_glob_iter_end(Container_T *container) { return; };
	virtual void on_iter_end(Container_T *container, const SwapType &s) = 0;
	virtual void on_end(Container_T *container) { this->out->add_json(json); };
};
static TSPSwap _dummy_swap_tsp;
class TSPLogger : public AbstractLogger<TSPContainer, TSPSwap>
{
public:
	TSPLogger(JSONStorage *out) : AbstractLogger(out){};
	void on_start(TSPContainer *container)
	{
		json["problem"] = "tsp";
		json["config"] = *container->conf->get_constants();
		json["towns"] = std::vector<point_t>();

		for (town_in_tour_id_t i = 0; i < container->conf->get("NUM_TOWNS"); i++)
		{
			point_t point;
			for (dim_id_t j = 0; j < container->conf->get("NUM_DIM"); j++)
			{
				point.push_back(container->get_towns_pos()[i * container->conf->get("NUM_DIM") + j]);
			}
			json["towns"].push_back(point);
		}
		json["tours"] = std::vector<tour_t>();
		json["annotations"] = std::vector<std::string>();
		json["quality"] = std::vector<quality_t>();
		json["moves"] = std::vector<std::map<std::string, int>>();
		this->on_iter_end(container);
	};
	void on_iter_end(TSPContainer *container, const TSPSwap &s = _dummy_swap_tsp)
	{
		tour_t tour;
		for (town_in_tour_id_t i = 0; i < container->conf->get("NUM_TOWNS"); i++)
		{
			tour.push_back(container->get_tour()[i]);
		}
		json["tours"].push_back(tour);
		json["annotations"].push_back("");
		json["quality"].push_back(container->get_quality_metric());
		if (s.i != -1)
		{
			json["moves"].push_back(std::map<std::string, int>{{"i", s.i}, {"j", s.j}});
		}
		else
		{
			json["moves"].push_back(std::map<std::string, int>{{"i", -1}, {"j", -1}});
		}
	};
};
static ClusteringSwap _dummy_swap_clustering;
class ClusteringLogger : public AbstractLogger<ClusteringContainer, ClusteringSwap>
{
public:
	ClusteringLogger(JSONStorage *out) : AbstractLogger(out){};
	void on_start(ClusteringContainer *container)
	{
		json["problem"] = "clustering";
		json["config"] = *container->conf->get_constants();
		json["points"] = std::vector<point_t>();
		for (point_id_t i = 0; i < container->conf->get("NUM_POINTS"); i++)
		{
			point_t point;
			for (dim_id_t j = 0; j < container->conf->get("NUM_DIM"); j++)
			{
				point.push_back(container->p_c[i * container->conf->get("NUM_DIM") + j]);
			}
			json["points"].push_back(point);
		}
		json["centroids"] = std::vector<std::vector<point_t>>();
		json["assignements"] = std::vector<cluster_assignment_t>();
		json["annotations"] = std::vector<std::string>();
		json["quality"] = std::vector<quality_t>();
		json["moves"] = std::vector<std::map<std::string, int>>();
		this->on_iter_end(container);
	};
	void on_iter_end(ClusteringContainer *container, const ClusteringSwap &s = _dummy_swap_clustering)
	{
		std::vector<point_t> centroids;
		for (cluster_id_t i = 0; i < container->conf->get("NUM_CLUST"); i++)
		{
			point_t centroid;
			for (dim_id_t j = 0; j < container->conf->get("NUM_DIM"); j++)
			{
				centroid.push_back(container->c_c[i * container->conf->get("NUM_DIM") + j]);
			}
			centroids.push_back(centroid);
		}

		json["centroids"].push_back(centroids);
		cluster_assignment_t cluster_assignment;
		for (int i = 0; i < container->conf->get("NUM_POINTS"); i++)
		{
			cluster_assignment.push_back(container->c_a[i]);
		}
		json["assignements"].push_back(cluster_assignment);
		json["annotations"].push_back("");
		json["quality"].push_back(container->get_quality_metric());
		if (s.point_id != -1)
		{
			json["moves"].push_back(std::map<std::string, int>{{"point_id", s.point_id}, {"cluster_src", s.cluster_src}, {"cluster_dst", s.cluster_dst}});
		}
		else
		{
			json["moves"].push_back(std::map<std::string, int>{{"point_id", -1}, {"cluster_src", -1}, {"cluster_dst", -1}});
		}
	};
};
static MAXSATSwap _dummy_swap_maxsat;
class MAXSATLogger : public AbstractLogger<MAXSATContainer, MAXSATSwap>
{
public:
	MAXSATLogger(JSONStorage *out) : AbstractLogger(out){};
	void on_start(MAXSATContainer *container)
	{
		json["problem"] = "maxsat";
		json["config"] = *container->config->get_constants();
		std::vector<clause_t> clauses = container->get_clauses();
		json["clauses_weights"] = std::vector<double>();
		for(int i=0; i<clauses.size(); i++) {
			json["clauses_weights"].push_back(container->get_pweights()[i]);
		}
		json["clauses"] = clauses;
		json["assignments"] = std::vector<std::vector<var_assignment_t>>();
		json["annotations"] = std::vector<std::string>();
		json["quality"] = std::vector<quality_t>();
		json["moves"] = std::vector<std::map<std::string, int>>();
		this->on_iter_end(container);
	};
	void on_iter_end(MAXSATContainer *container, const MAXSATSwap &s = _dummy_swap_maxsat)
	{
		std::vector<var_assignment_t> assignment;
		for (int i = 0; i < container->get_num_variables(); i++)
		{
			assignment.push_back(container->get_variables_val()[i]);
		}
		json["assignments"].push_back(assignment);
		// Annotation with the cost computed by the MAXSAT algorithm and the real cost
		json["annotations"].push_back("");
		json["quality"].push_back(container->get_quality_metric());
		if (s.var_id != -1)
		{
			json["moves"].push_back(std::map<std::string, int>{{"var_id", s.var_id}});
		}
		else
		{
			json["moves"].push_back(std::map<std::string, int>{{"var_id", -1}});
		}
	};
};

#endif