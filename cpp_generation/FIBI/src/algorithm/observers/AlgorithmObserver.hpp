#pragma once
#include <optional>
#include "../swaps/abstract.hpp"

using namespace std;
template <typename T_Container>
class AlgorithmObserver
{
public:
	virtual void on_start(const T_Container& c, const optional<SwapChoice>& s = nullopt) = 0;
	virtual void on_test_end(const T_Container& c, const optional<double>& delta = nullopt, const optional<SwapChoice>& s = nullopt) = 0;
	virtual void on_glob_iter_end(const T_Container& c, const optional<double>& delta = nullopt, const optional<SwapChoice>& s = nullopt) = 0;
	virtual void on_iter_end(const T_Container& c, const optional<SwapChoice>& s = nullopt) = 0;
	virtual void on_end(const T_Container& c, const optional<SwapChoice>& s = nullopt) = 0;
};