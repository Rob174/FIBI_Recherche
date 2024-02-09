#pragma once
#include <map>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include "../../src/launch/threapool.hpp"
#include "../../src/out/abstract.hpp"
#include "../../src/data/get_data.hpp"

#include <unordered_map>
#include <map>
#include "../../src/factories/tsp.hpp"
#include "../../src/data/constants/tsp.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <filesystem>
#include "../../libs/json.hpp"
#include "../../src/launch/generators/tsp.hpp"
#include "../../src/launch/generators/clustering.hpp"
#include "../../src/launch/generators/maxsat.hpp"
#include "../../src/launch/generators/abstract.hpp"

void test_gather_metadata(const int argc, char **argv)
{
	Args arguments = parseArguments(argc, argv);
	const string command = "python3 " + arguments.root + "check/check.py " + arguments.root + "metadata.json " + arguments.root + "out/merged_dataset.txt";
	int r = system(command.c_str());
	cout << "Return code " << r << endl;
	unique_ptr<set<int>> missing(read_missing(arguments));
	cout << missing->size() << " missing seeds found" << endl;
	if (arguments.problem == "tsp")
	{
		cout << "tsp " << arguments.all << endl;
		if (arguments.all == "true")
		{
			run_tsp_full(arguments, missing);
		}
		else
		{
			run_tsp<-1>(arguments, missing);
		}
	}
	else if (arguments.problem == "clustering")
	{
		cout << "clustering " << arguments.all << endl;
		if (arguments.all == "true")
		{
			run_clustering_full(arguments, missing);
		}
		else
		{
			run_clustering<-1>(arguments, missing);
		}
	}
	else if (arguments.problem == "maxsat")
	{
		cout << "maxsat " << arguments.all << endl;
		if (arguments.all == "true")
		{
			run_maxsat_full(arguments, missing);
		}
		else
		{
			run_maxsat<-1>(arguments, missing);
		}
	}
	else
	{
		cout << "Unknown problem " << arguments.problem << endl;
		throw runtime_error("Unknown problem " + arguments.problem);
	}
}