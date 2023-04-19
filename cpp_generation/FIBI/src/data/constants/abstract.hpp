#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <algorithm>

#include "../../../libs/json.hpp"
#include "../../utils.h"

using namespace std;
using json = nlohmann::json;
/*
 BEFORE PROBLEM GENERATION


					   MAXSAT                     Clustering                                     TSP
						  │
					 ┌────┴───────┐               ┌─────────────┐                          ┌─────────────┐
					 │SEED_GLOB   │               │ SEED_GLOB   │                          │ SEED_GLOB   │
					 │SEED_PROBLEMLEM│               │ SEED_PROBLEMLEM│                          │ SEED_PROBLEMLEM│
					 │SEED_ASSIGN │               │ SEED_ASSIGN │                          │ SEED_ASSIGN │
					 │DATASET     │               │ DATASET     │                          │ DATASET     │
					 │IMPR        │               │ IMPR        │                          │ IMPR        │
					 │FI_BI       │               │ FI_BI       │                          │ FI_BI       │
					 └────┬───────┘               └─────┬───────┘                          └─────┬───────┘
						  │                             │                                        │
						  │                             ▼                                        ▼
				  ┌───────┴──────┐       ┌──── uniform_points  ───┐               ┌──── uniform_points  ───┐
				  ▼              ▼       │                        │               │                        │
		   generated_clauses   ┌───┐     │   ┌──────────┐         ▼               │   ┌─────────┐          ▼
		   │                   │   │     └──►│NUM_POINTS│        ┌──┐             └──►│NUM_TOWNS│         ┌──┐
		   └►┌─────────────┐   └───┘         │NUM_DIM   │        │  │                 └─────────┘         │  │
			 │NUM_VARIABLES│                 │NUM_CLUST │        └──┘                                     └──┘
			 │NUM_CLAUSES  │                 └──────────┘
			 └─────────────┘
						 xx                            x                                     x
						  xx                           xx                                  xxx
		──────────────────xx────────────────────────────x─────────────────────────────────xx───────────
						  x                            xx                                 x
AFTER      ┌──────────────┐                  ┌────────────┐                       ┌────────────┐
		   │ SEED_GLOB    │                  │SEED_GLOB   │                       │SEED_GLOB   │
		   │ SEED_PROBLEMLEM │                  │SEED_PROBLEMLEM│                       │SEED_PROBLEMLEM│
		   │ SEED_ASSIGN  │                  │SEED_ASSIGN │                       │SEED_ASSIGN │
		   │ DATASET      │                  │DATASET     │                       │DATASET     │
		   │ IMPR         │                  │IMPR        │                       │IMPR        │
		   │ FI_BI        │                  │FI_BI       │                       │FI_BI       │
		   │ NUM_VARIABLES│                  │NUM_POINTS  │                       │NUM_TOWNS   │
		   │ NUM_CLAUSES  │                  │NUM_DIM     │                       └────────────┘
		   └──────────────┘                  │NUM_CLUST   │
											 └────────────┘
*/
#define stringify(name) #name
#define addCst(v) if(args.find(#v) != args.end() && args.at(#v) != -1){v = Uninitialized<int>(args.at(#v));}else {v = Uninitialized<int>();}

#define jMk(v) {#v,(double)v.get()}
class AbstractConfig
{

public:
	AbstractConfig() {};

	virtual const int num_choices() const = 0;

public:
	void print() {
		const vector<pair<string, double>> j = get_json();
		// On a single line 
		for (auto& [key, value] : j) {
			cout << key << ":" << value << ",";
		}
	};
	virtual vector<pair<string, double>> get_json() const = 0;
};