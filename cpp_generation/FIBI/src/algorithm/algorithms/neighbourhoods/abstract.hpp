#pragma once
#include "../../observers/AlgorithmObservable.hpp"
template <typename T_Container, typename T_Config, typename T_Swap>
class NeighbourhoodExplorer : public AlgorithmObservable<T_Swap, T_Container> {
public:
	NeighbourhoodExplorer() : AlgorithmObservable< T_Swap, T_Container>() {};
	virtual bool explore_flips(T_Container& co, const T_Config& cf) = 0;
};