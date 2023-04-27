#pragma once
#include <optional>

using namespace std;
template <typename T_Swap, typename T_Container>
class AlgorithmObserver
{
public:
	virtual void on_start(const T_Container& c, const optional<T_Swap>& s = nullopt) = 0;
	virtual void on_test_end(const T_Container& c, const optional<double>& delta = nullopt, const optional<T_Swap>& s = nullopt) = 0;
	virtual void on_glob_iter_end(const T_Container& c, const optional<double>& delta = nullopt, const optional<T_Swap>& s = nullopt) = 0;
	virtual void on_iter_end(const T_Container& c, const optional<T_Swap>& s = nullopt) = 0;
	virtual void on_end(const T_Container& c, const optional<T_Swap>& s = nullopt) = 0;
};