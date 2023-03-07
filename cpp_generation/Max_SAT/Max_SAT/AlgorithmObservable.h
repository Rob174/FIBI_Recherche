#ifndef ALGORITHMOBSERVABLE_H
#define ALGORITHMOBSERVABLE_H
#include <vector>

#include "AlgorithmObserver.h"
#include "Container.h"
template <class T_Observer, class T_Container, class T_Swap>
class AlgorithmObservable
{
protected:
	std::vector<T_Observer *> obs;

public:
	AlgorithmObservable(std::vector<T_Observer *> observers)
	{
		this->obs = observers;
	};
	void on_start(T_Container *container)
	{
		for (T_Observer*o : this->obs)
		{
			o->on_start(container);
		}
	};
	void on_glob_iter_end(T_Container *container, quality_delta_t delta, const T_Swap& s)
	{
		for (T_Observer*o : this->obs)
		{
			o->on_glob_iter_end(container, delta, s);
		}
	};
	void on_iter_end(T_Container *container, const T_Swap& s)
	{
		for (T_Observer *o : this->obs)
		{
			o->on_iter_end(container,s);
		}
	};
	void on_end(T_Container *container)
	{
		for (T_Observer*o : this->obs)
		{
			o->on_end(container);
		}
	};
};
#endif