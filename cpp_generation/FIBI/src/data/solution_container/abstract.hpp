#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include "../../algorithm/swaps/abstract.hpp"
#include "../../utils.h"
#include "../../../src/data/constants/abstract.hpp"
#include "../../types.h"

using namespace std;

class AbstractSolutionContainer
{
public:
	optional<SwapChoice> last_choice; 
	quality_t quality_metric;

public: 
	AbstractSolutionContainer(){};
	AbstractSolutionContainer(const AbstractSolutionContainer &other)
	{
		this->quality_metric = other.quality_metric;
		last_choice = optional<SwapChoice>(other.last_choice);
	};
	std::map<std::string, long> get_last_choice() 
	{
		if (last_choice.has_value()) {
			return last_choice.value().get_dict();
		}

		else
			throw std::runtime_error("No last choice available");
	};
	// Setup
	void on_improvement_done()
	{
		this->quality_metric = this->compute_quality_metric();
	};
	// Objective
	[[nodiscard]] quality_t get_quality_metric() const noexcept
	{
		return quality_metric;
	};
	void set_quality_metric(const quality_t q) noexcept
	{
		this->quality_metric = q;
	}
	virtual quality_t compute_quality_metric() = 0;
	void update_with_delta_cost(const quality_delta_t delta) noexcept
	{
		quality_metric += delta;
	};
	// Utility
	virtual void print() = 0;
};