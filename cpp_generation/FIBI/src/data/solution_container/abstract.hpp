#pragma once
#include <vector>
#include <map>
#include <set>
#include "../../utils.h"
#include <string>
#include <iostream>
#include "../../../src/data/constants/abstract.hpp"
#include "../../algorithm/swaps/abstract.hpp"
#include "../../types.h"


using namespace std;

template <typename T_Swap>
class AbstractSolutionContainer
{
public:
	T_Swap last_choice;
	quality_t quality_metric;

public:
	AbstractSolutionContainer() {};
	AbstractSolutionContainer(const AbstractSolutionContainer& other) {
		this->quality_metric = other.quality_metric;
		this->last_choice = other.last_choice;
	};
	// Setup
	void on_improvement_done() {
		this->quality_metric = this->compute_quality_metric();
	};
	// Flips
	virtual quality_delta_t test_flip(const T_Swap& test_swap) const = 0;
	virtual void flip(const T_Swap& swap, const quality_delta_t delta) = 0;
	// Objective
	[[nodiscard]] quality_t get_quality_metric() const noexcept {
		return quality_metric;
	};
	void set_quality_metric(const quality_t q) noexcept {
		this->quality_metric = q;
	}
	virtual quality_t compute_quality_metric() = 0;
	void update_with_delta_cost(const quality_delta_t delta) noexcept {
		quality_metric += delta;
	};
	// Utility
	virtual void print() = 0;
	std::map<std::string, int> get_last_choice() {
		return last_choice->get_dict();
	};
};