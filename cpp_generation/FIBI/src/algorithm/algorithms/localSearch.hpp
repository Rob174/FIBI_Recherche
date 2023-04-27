#pragma once
#include "../observers/AlgorithmObservable.hpp"

/** @brief Local search algorithm
* @tparam T_Swap The type of the swap
* @tparam T_Container The type of the container
* @tparam debug If true, the container will be printed at each iteration
* @tparam num_iter_max The maximum number of iterations. If -1, the algorithm will run until the container is not modified anymore
* */
template <typename T_Swap, typename T_Container, typename T_Neighbourhood, typename T_Config, bool debug = false, const int num_iter_max = -1>
class LocalSearch {
private:
	const T_Neighbourhood* n;
	AlgorithmObservable<T_Swap, T_Container>* o;
public:
	AlgorithmObservable<T_Swap, T_Container>* get_observable() {
		return o;
	}
	LocalSearch(const T_Neighbourhood* n, AlgorithmObservable<T_Swap, T_Container>* o) : n(n), o(o) {}
	~LocalSearch() {
		delete n;
	}
	/** @brief Run the local search algorithm
	*	@param c The container to run the algorithm on
	* 	*/
	void run(T_Container& c, const T_Config& cf) {
		c.set_quality_metric(c.compute_quality_metric());
		o->on_start(c);
		// Local seach
		bool stop_loop = false;
		int num_iter = 0;
		while (!stop_loop)
		{
			stop_loop = n->explore_flips(c, cf);
			num_iter++;
			if constexpr (debug) {
				c.print();
			}
			if constexpr (num_iter_max != -1) {
				stop_loop = stop_loop || num_iter >= num_iter_max;
			}
		}
		o->on_end(c);
	}
};