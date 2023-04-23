#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "Container.h"
#include "constants.h"
#include "AlgorithmObserver.h"
#include "AlgorithmObservable.h"
#include "types.h"
#define delta_t double
#define best_id_t int
//#define DEBUG_LV1
template <class T_Observer, class T_Container, class T_Config, class T_Swap>
class Algorithm : public AlgorithmObservable<T_Observer, T_Container, T_Swap>
{
private:
	const int num_iter_max;
public:
	Algorithm(std::vector<T_Observer *> observers) : AlgorithmObservable<T_Observer, T_Container, T_Swap>(observers), num_iter_max(-1) {};
	Algorithm(std::vector<T_Observer*> observers, const int num_iter_max) : AlgorithmObservable<T_Observer, T_Container, T_Swap>(observers), num_iter_max(num_iter_max) {};
	void run(T_Container *container, T_Config *config, bool FI)
	{
		this->on_start(container);
		container->set_quality_metric(container->compute_quality_metric());
		// Local seach
		bool stop_loop = false;
		int num_iter = 0;
		while (!stop_loop && (num_iter < this->num_iter_max || this->num_iter_max == -1))
		{
			stop_loop = this->explore_flips(container, config, FI);
			num_iter++;
#ifdef DEBUG_LV1
			container->print();
#endif // DEBUG_LV1
		} 
		this->on_end(container);
	}
	virtual bool explore_flips(T_Container *container, T_Config *config, bool FI) = 0;
};
class MAXSATAlgorithm : public Algorithm<AlgorithmObserver<MAXSATContainer, MAXSATSwap>, MAXSATContainer, MAXSATConfig, MAXSATSwap>
{
public:
	MAXSATAlgorithm(std::vector<AlgorithmObserver<MAXSATContainer, MAXSATSwap>*> observers) : Algorithm(observers) {};
	MAXSATAlgorithm(std::vector<AlgorithmObserver<MAXSATContainer, MAXSATSwap>*> observers, const int num_iter_max) : Algorithm(observers,num_iter_max){};
	bool explore_flips(MAXSATContainer *container, MAXSATConfig *config, bool FI);
};
class ClusteringAlgorithm : public Algorithm<AlgorithmObserver<ClusteringContainer, ClusteringSwap>, ClusteringContainer, ClusteringConfig, ClusteringSwap>
{
public:
	ClusteringAlgorithm(std::vector<AlgorithmObserver<ClusteringContainer, ClusteringSwap>*> observers) : Algorithm(observers){};
	ClusteringAlgorithm(std::vector<AlgorithmObserver<ClusteringContainer, ClusteringSwap>*> observers, const int num_iter_max) : Algorithm(observers, num_iter_max) {};
	bool explore_flips(ClusteringContainer *container, ClusteringConfig *config, bool FI);
};
class TSPAlgorithm : public Algorithm<AlgorithmObserver<TSPContainer, TSPSwap>, TSPContainer, TSPConfig,TSPSwap>
{
	public:
	TSPAlgorithm(std::vector<AlgorithmObserver<TSPContainer, TSPSwap>*> observers) : Algorithm(observers){};
	TSPAlgorithm(std::vector<AlgorithmObserver<TSPContainer, TSPSwap>*> observers, const int num_iter_max) : Algorithm(observers, num_iter_max) {};
	bool explore_flips(TSPContainer *container, TSPConfig *config, bool FI);
};
double clustering_cost_modif(ClusteringContainer *c, const int f, const int t, const double *point, ClusteringConfig *config);
#endif