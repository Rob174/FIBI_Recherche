#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <random>
#include <stdio.h>
#include <map>
#include <memory>

#include "test.h"
#include "MAXSATgenerator.h"
#include "Algorithm.h"
#include "AlgorithmObservable.h"
#include "AlgorithmObserver.h"
#include "result.h"
#include "Metrics.h"
#include "factories.h"
#include "progress.h"
// https://stackoverflow.com/questions/865668/parsing-command-line-arguments-in-c
char* getCmdOption(char** begin, char** end, const std::string& option)
{
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}
std::pair<int, int> parse_interval(int argc, char** argv)
{
	if (argc == 3)
	{
		return std::make_pair(std::atoi(argv[1]), std::atoi(argv[2]));
	}
	THROW_EXC2("Invalid number of parameters: expected 3 received ", std::to_string(argc), 2000);
}
#define P_FLAG_SEEN(flag) cmdOptionExists(argv, argv + argc, flag)
#define P_GET_VAL(flag) getCmdOption(argv, argv + argc, flag)