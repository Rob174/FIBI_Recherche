#pragma once
#include "../../observers/AlgorithmObservable.hpp"
template <typename T_Container, typename T_Config, typename T_Flip>
class NeighbourhoodExplorer {
public:
	NeighbourhoodExplorer() {};
	virtual bool explore_flips(T_Container& co, const T_Config& cf, T_Flip& f) const = 0;
};