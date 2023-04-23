#pragma once
#include "./abstract.hpp"
#include "../../types.h"

using namespace std;
class ClusteringSwap : public SwapChoice
{
public:
	ClusteringSwap() : point_id(-1), cluster_src(-1), cluster_dst(-1) {};
	ClusteringSwap(point_id_t point_id, cluster_id_t cluster_src, cluster_id_t cluster_dst) : point_id(point_id), cluster_src(cluster_src), cluster_dst(cluster_dst), SwapChoice() {};
	point_id_t point_id;
	cluster_id_t cluster_src;
	cluster_id_t cluster_dst;
	map<string,long> get_dict()
	{
		map<string,long> dict;
		dict["point_id"] = point_id;
		dict["cluster_src"] = cluster_src;
		dict["cluster_dst"] = cluster_dst;
		return dict;
	};
	bool operator==(const ClusteringSwap& other) const {
		return this->point_id == other.point_id && this->cluster_src == other.cluster_src && this->cluster_dst == other.cluster_dst;
	};
	bool operator!=(const ClusteringSwap& other) const
	{
		return !(*this == other);
	};
	// Overload the string operator
	string str() const
	{
		return "ClusteringSwap(point:" + to_string(point_id) + ", src:" + to_string(cluster_src) + ", dst:" + to_string(cluster_dst) + ")";
	};
};