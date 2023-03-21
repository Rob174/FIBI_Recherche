#pragma once
#include <optional>
#include <vector>
#include "AlgorithmObserver.hpp"

using namespace std;
template <typename T_Swap, typename T_Container>
class AlgorithmObservable
{
private:
	vector<AlgorithmObserver<T_Swap, T_Container>*>& observers;
public:
	AlgorithmObservable(vector<AlgorithmObserver<T_Swap, T_Container>*>& obs) : observers(obs) {};
	void add_observer(AlgorithmObserver<T_Swap, T_Container>* observer) {
		observers.push_back(observer);
	}
	void on_start(const T_Container& c, const optional<T_Swap>& s = nullopt) {
		for (auto observer : observers) {
			observer->on_start(c, s);
		}
	}
	void on_test_end(const T_Container& c, const optional<double> delta = nullopt, const optional<T_Swap>& s = nullopt) {
		for (auto observer : observers) {
			observer->on_test_end(c, delta, s);
		}
	}
	void on_glob_iter_end(const T_Container& c, const optional<double> delta = nullopt, const optional<T_Swap>& s = nullopt) {
		for (auto observer : observers) {
			observer->on_glob_iter_end(c, delta, s);
		}
	}
	void on_iter_end(const T_Container& c, const optional<T_Swap>& s = nullopt) {
		for (auto observer : observers) {
			observer->on_iter_end(c, s);
		}
	}
	void on_end(const T_Container& c, const optional<T_Swap>& s = nullopt) {
		for (auto observer : observers) {
			observer->on_end(c, s);
		}
	}
};